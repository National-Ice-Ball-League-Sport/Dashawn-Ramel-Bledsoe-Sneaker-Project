#ifndef HW01_BOARD_ADAPTER_H
#define HW01_BOARD_ADAPTER_H

#include <stdint.h>
#include "../dial_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Board-specific implementations bridge this contract to the selected SDK. */
typedef struct {
    int (*read_battery_pct)(void *context); /* 0..100, negative when unavailable */
    int (*notify_telemetry)(void *context, const uint8_t *frame, uint16_t length);
    void *context;
} hw01_board_ops_t;

typedef struct {
    hw01_board_ops_t ops;
    uint32_t publish_interval_ms;
    uint32_t last_publish_ms;
    uint8_t has_published;
} hw01_adapter_t;

/* Initialize processing in the existing firmware core and this board adapter. */
int hw01_adapter_init(hw01_adapter_t *adapter,
                      const hw01_board_ops_t *ops,
                      dial_mode_t initial_mode,
                      uint32_t publish_interval_ms);

/* Call serially from the sensor task with one converted IMU sample. */
void hw01_adapter_on_sample(hw01_adapter_t *adapter, const imu_sample_t *sample);

/* Invoke from the debounced rotary encoder handler. */
void hw01_adapter_on_detent(dial_mode_t mode);

#ifdef __cplusplus
}
#endif
#endif
