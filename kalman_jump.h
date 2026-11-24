#ifndef KALMAN_JUMP_H
#define KALMAN_JUMP_H

#include <stdint.h>
#include <stdbool.h>

/*
 * 1D Vertical Kinematic Kalman Filter for Jump Takeoff & Height Estimation
 * State vector x = [ z_pos (m), z_vel (m/s) ]^T
 * Control input u = linear vertical acceleration a_z (m/s^2)
 *
 * Creation By Dashawn Ramel Bledsoe, Founder | Sports Product Designer |
 * Entrepreneur Inventor at National Ice Ball League Sport Inc.
 */

typedef enum {
    JUMP_PHASE_STANCE = 0,
    JUMP_PHASE_PROPULSION,
    JUMP_PHASE_FLIGHT,
    JUMP_PHASE_LANDING
} jump_phase_t;

typedef struct {
    /* State estimates */
    float z_pos;            /* Estimated vertical displacement (m) */
    float z_vel;            /* Estimated vertical velocity (m/s) */

    /* 2x2 Error covariance matrix P */
    float P00;
    float P01;
    float P10;
    float P11;

    /* Tuning parameters */
    float q_accel;          /* Process noise spectral density (m^2/s^3) */
    float r_zupt_vel;       /* Stance zero-velocity measurement variance (m^2/s^2) */
    float r_zupt_pos;       /* Stance zero-position measurement variance (m^2) */

    /* Jump phase & event metrics */
    jump_phase_t phase;
    uint32_t takeoff_ms;
    float takeoff_vel_mps;
    float last_jump_height_m;
    float peak_landing_g;
    uint32_t stance_stable_count;
} kalman_jump_t;

void kalman_jump_init(kalman_jump_t *kf);
void kalman_jump_reset_stance(kalman_jump_t *kf);
bool kalman_jump_update(kalman_jump_t *kf, float lin_az_mps2, float gyro_norm_rps, float dt_s, uint32_t now_ms);

#endif /* KALMAN_JUMP_H */
