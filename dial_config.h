#ifndef DIAL_CONFIG_H
#define DIAL_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Future-Step Smart Pivot Dial (HW-01) - Firmware Configuration
 * Creation By Dashawn Ramel Bledsoe, Founder | Sports Product Designer |
 * Entrepreneur Inventor at National Ice Ball League Sport Inc.
 */

#define DIAL_FW_VERSION_MAJOR       1
#define DIAL_FW_VERSION_MINOR       2
#define DIAL_FW_VERSION_PATCH       0

#define BLE_SERVICE_UUID_STR        "0000db01-0000-1000-8000-00805f9b34fb"
#define BLE_CHAR_TELEMETRY_UUID_STR "0000db02-0000-1000-8000-00805f9b34fb"
#define BLE_CHAR_CONTROL_UUID_STR   "0000db03-0000-1000-8000-00805f9b34fb"

#define GRAVITY_MPS2                9.80665f
#define CALIBRATION_SAMPLES_3S      300U    /* 3.0s standstill at 100 Hz */
#define TELEMETRY_PACKET_BYTES      20U

typedef enum {
    DIAL_MODE_DAILY_GAIT    = 0,    /* Detent 1 (0 deg)   - 100 Hz  */
    DIAL_MODE_VERTICAL_JUMP = 1,    /* Detent 2 (45 deg)  - 500 Hz  */
    DIAL_MODE_FOOT_SPEED    = 2,    /* Detent 3 (90 deg)  - 500 Hz  */
    DIAL_MODE_FULL_COURT    = 3     /* Detent 4 (135 deg) - 1000 Hz */
} dial_mode_t;

typedef struct {
    float ax_mps2;
    float ay_mps2;
    float az_mps2;
    float gx_rps;
    float gy_rps;
    float gz_rps;
    uint32_t timestamp_ms;
} imu_sample_t;

typedef struct __attribute__((packed)) {
    uint16_t magic;                 /* 0xDB01 */
    uint8_t  mode;                  /* dial_mode_t */
    uint8_t  battery_pct;           /* 0 - 100 */
    uint32_t timestamp_ms;          /* Session uptime (ms) */
    uint16_t jump_height_mm;        /* Latest jump height in mm */
    uint16_t step_count;            /* Cumulative session steps */
    uint16_t foot_speed_cms;        /* Instantaneous foot speed (cm/s) */
    uint16_t peak_impact_cg;        /* Landing load in centi-G (e.g., 420 = 4.20G) */
    uint16_t takeoff_vel_cms;       /* Kalman vertical takeoff velocity (cm/s) */
    uint16_t crc16;                 /* CCITT-FALSE checksum */
} dial_telemetry_packet_t;

#endif /* DIAL_CONFIG_H */
