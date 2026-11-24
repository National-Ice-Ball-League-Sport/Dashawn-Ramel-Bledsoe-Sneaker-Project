#include "kalman_jump.h"
#include "dial_config.h"
#include <math.h>

/*
 * 1D Vertical Kinematic Kalman Filter Implementation
 * Creation By Dashawn Ramel Bledsoe, Founder | Sports Product Designer |
 * Entrepreneur Inventor at National Ice Ball League Sport Inc.
 */

#define PROPULSION_ACCEL_THRESH_MPS2   3.20f   /* Upward push threshold */
#define FREEFALL_ACCEL_THRESH_MPS2    -6.80f   /* Linear az approaches -g in freefall */
#define LANDING_IMPACT_THRESH_MPS2    14.50f   /* High-G heel/forefoot strike */
#define ZUPT_ACCEL_BAND_MPS2           0.85f   /* Quiet stance window */
#define ZUPT_GYRO_BAND_RPS             0.45f   /* Quiet angular rate window */
#define ZUPT_CONFIRM_SAMPLES           12U     /* Consecutive quiet samples for ZUPT */
#define MIN_FLIGHT_MS                  110U    /* Reject micro-hops < 110 ms (~1.5 cm) */
#define MAX_FLIGHT_MS                  1350U   /* Physical upper bound */

void kalman_jump_init(kalman_jump_t *kf) {
    if (!kf) return;
    kf->z_pos = 0.0f;
    kf->z_vel = 0.0f;

    kf->P00 = 0.01f;
    kf->P01 = 0.0f;
    kf->P10 = 0.0f;
    kf->P11 = 0.01f;

    /* Tuned for 500 Hz - 1000 Hz 6-axis court IMU */
    kf->q_accel    = 0.35f;
    kf->r_zupt_vel = 0.0025f;
    kf->r_zupt_pos = 0.0004f;

    kf->phase = JUMP_PHASE_STANCE;
    kf->takeoff_ms = 0U;
    kf->takeoff_vel_mps = 0.0f;
    kf->last_jump_height_m = 0.0f;
    kf->peak_landing_g = 1.0f;
    kf->stance_stable_count = 0U;
}

void kalman_jump_reset_stance(kalman_jump_t *kf) {
    if (!kf) return;
    kf->z_pos = 0.0f;
    kf->z_vel = 0.0f;
    kf->P00 = 0.001f;
    kf->P01 = 0.0f;
    kf->P10 = 0.0f;
    kf->P11 = 0.001f;
    kf->phase = JUMP_PHASE_STANCE;
}

static void kalman_zupt_correct(kalman_jump_t *kf) {
    /*
     * Sequential scalar Kalman measurement updates for z_vel = 0 and z_pos = 0
     * during verified court stance, eliminating double-integration drift.
     */
    /* 1. Velocity ZUPT (H_v = [0, 1]) */
    float y_v = 0.0f - kf->z_vel;
    float S_v = kf->P11 + kf->r_zupt_vel;
    float K0_v = kf->P01 / S_v;
    float K1_v = kf->P11 / S_v;

    kf->z_pos += K0_v * y_v;
    kf->z_vel += K1_v * y_v;

    float P00_tmp = kf->P00 - K0_v * kf->P10;
    float P01_tmp = kf->P01 - K0_v * kf->P11;
    float P10_tmp = kf->P10 - K1_v * kf->P10;
    float P11_tmp = kf->P11 - K1_v * kf->P11;

    kf->P00 = P00_tmp;
    kf->P01 = P01_tmp;
    kf->P10 = P10_tmp;
    kf->P11 = P11_tmp;

    /* 2. Ground-plane position ZUPT (H_p = [1, 0]) */
    float y_p = 0.0f - kf->z_pos;
    float S_p = kf->P00 + kf->r_zupt_pos;
    float K0_p = kf->P00 / S_p;
    float K1_p = kf->P10 / S_p;

    kf->z_pos += K0_p * y_p;
    kf->z_vel += K1_p * y_p;

    P00_tmp = kf->P00 - K0_p * kf->P00;
    P01_tmp = kf->P01 - K0_p * kf->P01;
    P10_tmp = kf->P10 - K1_p * kf->P00;
    P11_tmp = kf->P11 - K1_p * kf->P01;

    kf->P00 = P00_tmp;
    kf->P01 = P01_tmp;
    kf->P10 = P10_tmp;
    kf->P11 = P11_tmp;
}

bool kalman_jump_update(kalman_jump_t *kf, float lin_az_mps2, float gyro_norm_rps, float dt_s, uint32_t now_ms) {
    if (!kf || dt_s <= 0.0f || dt_s > 0.05f) {
        return false;
    }

    bool jump_completed = false;
    float dt2 = dt_s * dt_s;
    float dt3 = dt2 * dt_s;
    float dt4 = dt2 * dt2;

    /* 1. Time Update (Kinematic Prediction Step) */
    kf->z_pos += kf->z_vel * dt_s + 0.5f * lin_az_mps2 * dt2;
    kf->z_vel += lin_az_mps2 * dt_s;

    /* Covariance prediction: P = F * P * F^T + Q */
    float P00 = kf->P00 + dt_s * (kf->P10 + kf->P01) + dt2 * kf->P11 + 0.25f * dt4 * kf->q_accel;
    float P01 = kf->P01 + dt_s * kf->P11 + 0.5f * dt3 * kf->q_accel;
    float P10 = kf->P10 + dt_s * kf->P11 + 0.5f * dt3 * kf->q_accel;
    float P11 = kf->P11 + dt2 * kf->q_accel;

    kf->P00 = P00;
    kf->P01 = P01;
    kf->P10 = P10;
    kf->P11 = P11;

    /* 2. Stance Zero-Velocity Update (ZUPT) Detection */
    bool is_quiet = (fabsf(lin_az_mps2) < ZUPT_ACCEL_BAND_MPS2) &&
                    (gyro_norm_rps < ZUPT_GYRO_BAND_RPS);

    if (is_quiet && (kf->phase == JUMP_PHASE_STANCE || kf->phase == JUMP_PHASE_LANDING)) {
        kf->stance_stable_count++;
        if (kf->stance_stable_count >= ZUPT_CONFIRM_SAMPLES) {
            kalman_zupt_correct(kf);
            kf->phase = JUMP_PHASE_STANCE;
        }
    } else {
        kf->stance_stable_count = 0U;
    }

    /* 3. Biomechanical Jump Phase State Machine */
    switch (kf->phase) {
        case JUMP_PHASE_STANCE:
            if (lin_az_mps2 > PROPULSION_ACCEL_THRESH_MPS2 && kf->z_vel > 0.25f) {
                kf->phase = JUMP_PHASE_PROPULSION;
                kf->peak_landing_g = 1.0f;
            }
            break;

        case JUMP_PHASE_PROPULSION:
            /* Toe-off occurs when upward acceleration drops below freefall threshold while z_vel > 0 */
            if (lin_az_mps2 < FREEFALL_ACCEL_THRESH_MPS2 && kf->z_vel > 0.65f) {
                kf->phase = JUMP_PHASE_FLIGHT;
                kf->takeoff_ms = now_ms;
                kf->takeoff_vel_mps = kf->z_vel;
            } else if (kf->z_vel < -0.20f) {
                /* Aborted countermovement without leaving ground */
                kf->phase = JUMP_PHASE_STANCE;
            }
            break;

        case JUMP_PHASE_FLIGHT: {
            uint32_t flight_ms = now_ms - kf->takeoff_ms;
            if (lin_az_mps2 > LANDING_IMPACT_THRESH_MPS2 || flight_ms > MAX_FLIGHT_MS) {
                kf->phase = JUMP_PHASE_LANDING;
                float total_g = (lin_az_mps2 + GRAVITY_MPS2) / GRAVITY_MPS2;
                if (total_g > kf->peak_landing_g) {
                    kf->peak_landing_g = total_g;
                }

                if (flight_ms >= MIN_FLIGHT_MS && flight_ms <= MAX_FLIGHT_MS) {
                    float t_flight_s = (float)flight_ms * 0.001f;
                    /* Hybrid fusion: 65% ballistic flight-time + 35% Kalman takeoff velocity */
                    float h_flight = 0.125f * GRAVITY_MPS2 * t_flight_s * t_flight_s;
                    float h_vel    = (kf->takeoff_vel_mps * kf->takeoff_vel_mps) / (2.0f * GRAVITY_MPS2);
                    kf->last_jump_height_m = 0.65f * h_flight + 0.35f * h_vel;
                    jump_completed = true;
                }
            }
            break;
        }

        case JUMP_PHASE_LANDING: {
            float total_g = (lin_az_mps2 + GRAVITY_MPS2) / GRAVITY_MPS2;
            if (total_g > kf->peak_landing_g) {
                kf->peak_landing_g = total_g;
            }
            break;
        }
    }

    return jump_completed;
}
