#include "dial_config.h"
#include "kalman_jump.h"
#include <math.h>
#include <string.h>

/*
 * Future-Step Smart Pivot Dial (HW-01) - Main Embedded DSP Loop
 * Creation By Dashawn Ramel Bledsoe, Founder | Sports Product Designer |
 * Entrepreneur Inventor at National Ice Ball League Sport Inc.
 */

typedef struct {
    dial_mode_t active_mode;
    bool calibrated;
    uint32_t cal_count;
    float bias_ax;
    float bias_ay;
    float bias_az;

    kalman_jump_t jump_kf;
    uint16_t step_count;
    float planar_vx;
    float planar_vy;
    float inst_speed_mps;
    uint32_t last_sample_ms;
    uint32_t last_step_ms;
} dial_runtime_t;

static dial_runtime_t g_dial;

static uint16_t crc16_ccitt(const uint8_t *data, uint32_t len) {
    uint16_t crc = 0xFFFFU;
    for (uint32_t i = 0; i < len; ++i) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t b = 0; b < 8; ++b) {
            crc = (crc & 0x8000U) ? (uint16_t)((crc << 1) ^ 0x1021U) : (uint16_t)(crc << 1);
        }
    }
    return crc;
}

void dial_firmware_init(dial_mode_t initial_mode) {
    memset(&g_dial, 0, sizeof(g_dial));
    g_dial.active_mode = initial_mode;
    kalman_jump_init(&g_dial.jump_kf);
}

void dial_on_bezel_detent(dial_mode_t new_mode) {
    g_dial.active_mode = new_mode;
    kalman_jump_reset_stance(&g_dial.jump_kf);
}

void dial_process_imu_sample(const imu_sample_t *s, dial_telemetry_packet_t *out_pkt) {
    if (!s || !out_pkt) return;

    /* 3-second standstill gravity & bias calibration */
    if (!g_dial.calibrated) {
        g_dial.bias_ax += s->ax_mps2;
        g_dial.bias_ay += s->ay_mps2;
        g_dial.bias_az += (s->az_mps2 - GRAVITY_MPS2);
        g_dial.cal_count++;
        if (g_dial.cal_count >= CALIBRATION_SAMPLES_3S) {
            float inv = 1.0f / (float)CALIBRATION_SAMPLES_3S;
            g_dial.bias_ax *= inv;
            g_dial.bias_ay *= inv;
            g_dial.bias_az *= inv;
            g_dial.calibrated = true;
            g_dial.last_sample_ms = s->timestamp_ms;
        }
        return;
    }

    float dt_s = (s->timestamp_ms > g_dial.last_sample_ms)
                     ? ((float)(s->timestamp_ms - g_dial.last_sample_ms) * 0.001f)
                     : 0.001f;
    g_dial.last_sample_ms = s->timestamp_ms;

    float lin_ax = s->ax_mps2 - g_dial.bias_ax;
    float lin_ay = s->ay_mps2 - g_dial.bias_ay;
    float lin_az = (s->az_mps2 - GRAVITY_MPS2) - g_dial.bias_az;
    float gyro_norm = sqrtf(s->gx_rps * s->gx_rps + s->gy_rps * s->gy_rps + s->gz_rps * s->gz_rps);

    /* 1D Kalman Filter update for vertical jump takeoff & landing detection */
    (void)kalman_jump_update(&g_dial.jump_kf, lin_az, gyro_norm, dt_s, s->timestamp_ms);

    /* Step cadence & planar foot speed integration with stance ZUPT damping */
    float accel_mag = sqrtf(lin_ax * lin_ax + lin_ay * lin_ay + lin_az * lin_az);
    if (accel_mag > 11.5f && (s->timestamp_ms - g_dial.last_step_ms) > 260U &&
        g_dial.jump_kf.phase == JUMP_PHASE_STANCE) {
        g_dial.step_count++;
        g_dial.last_step_ms = s->timestamp_ms;
    }

    if (g_dial.jump_kf.stance_stable_count >= 12U) {
        g_dial.planar_vx *= 0.25f;
        g_dial.planar_vy *= 0.25f;
    } else {
        g_dial.planar_vx = 0.985f * g_dial.planar_vx + lin_ax * dt_s;
        g_dial.planar_vy = 0.985f * g_dial.planar_vy + lin_ay * dt_s;
    }
    g_dial.inst_speed_mps = sqrtf(g_dial.planar_vx * g_dial.planar_vx +
                                  g_dial.planar_vy * g_dial.planar_vy);

    /* Pack 20-byte BLE 5.3 telemetry frame */
    out_pkt->magic          = 0xDB01U;
    out_pkt->mode           = (uint8_t)g_dial.active_mode;
    out_pkt->battery_pct    = 96U;
    out_pkt->timestamp_ms   = s->timestamp_ms;
    out_pkt->jump_height_mm = (uint16_t)(g_dial.jump_kf.last_jump_height_m * 1000.0f + 0.5f);
    out_pkt->step_count     = g_dial.step_count;
    out_pkt->foot_speed_cms = (uint16_t)(g_dial.inst_speed_mps * 100.0f + 0.5f);
    out_pkt->peak_impact_cg = (uint16_t)(g_dial.jump_kf.peak_landing_g * 100.0f + 0.5f);
    out_pkt->takeoff_vel_cms= (uint16_t)(fmaxf(0.0f, g_dial.jump_kf.takeoff_vel_mps) * 100.0f + 0.5f);
    out_pkt->crc16          = crc16_ccitt((const uint8_t *)out_pkt, TELEMETRY_PACKET_BYTES - 2U);
}

#ifdef DIAL_HOST_TEST
#include <stdio.h>
int main(void) {
    dial_firmware_init(DIAL_MODE_VERTICAL_JUMP);
    dial_telemetry_packet_t pkt;
    memset(&pkt, 0, sizeof(pkt));

    /* 1. 3s standstill calibration at 100 Hz */
    for (uint32_t i = 0; i < CALIBRATION_SAMPLES_3S; ++i) {
        imu_sample_t s = {0.0f, 0.0f, GRAVITY_MPS2, 0.0f, 0.0f, 0.0f, i * 10U};
        dial_process_imu_sample(&s, &pkt);
    }

    uint32_t t = 3000U;
    /* 2. 140 ms upward propulsion (+18 m/s^2 -> v_to ~ 2.52 m/s) */
    for (int i = 0; i < 70; ++i) {
        t += 2U;
        imu_sample_t s = {0.1f, 0.0f, GRAVITY_MPS2 + 18.0f, 0.05f, 0.0f, 0.0f, t};
        dial_process_imu_sample(&s, &pkt);
    }
    /* 3. 520 ms ballistic flight (lin_az = -g) */
    for (int i = 0; i < 260; ++i) {
        t += 2U;
        imu_sample_t s = {0.05f, 0.0f, 0.0f, 0.02f, 0.0f, 0.0f, t};
        dial_process_imu_sample(&s, &pkt);
    }
    /* 4. Landing impact spike (+38 m/s^2 -> ~4.87G) */
    t += 2U;
    imu_sample_t land = {0.0f, 0.0f, GRAVITY_MPS2 + 38.0f, 0.1f, 0.0f, 0.0f, t};
    dial_process_imu_sample(&land, &pkt);

    printf("Kalman Jump Test -> Height: %u mm | Takeoff Vel: %u cm/s | Impact: %.2f G\n",
           pkt.jump_height_mm, pkt.takeoff_vel_cms, pkt.peak_impact_cg / 100.0f);
    return (pkt.jump_height_mm > 300U) ? 0 : 1;
}
#endif
