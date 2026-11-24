#include "board_adapter.h"
#include "../dial_config.h"
#include <stddef.h>
#include <string.h>

/* Firmware core API implemented in the repository's root main.c. */
extern void dial_firmware_init(dial_mode_t initial_mode);
extern void dial_on_bezel_detent(dial_mode_t new_mode);
extern void dial_process_imu_sample(const imu_sample_t *sample,
                                    dial_telemetry_packet_t *out_packet);

static uint16_t read_le16(const uint8_t *p) {
    return (uint16_t)p[0] | (uint16_t)((uint16_t)p[1] << 8);
}

static void write_le16(uint8_t *p, uint16_t value) {
    p[0] = (uint8_t)(value & 0xffU);
    p[1] = (uint8_t)(value >> 8);
}

static void write_le32(uint8_t *p, uint32_t value) {
    p[0] = (uint8_t)(value & 0xffU);
    p[1] = (uint8_t)((value >> 8) & 0xffU);
    p[2] = (uint8_t)((value >> 16) & 0xffU);
    p[3] = (uint8_t)(value >> 24);
}

static void encode_frame(const dial_telemetry_packet_t *packet, uint8_t frame[TELEMETRY_PACKET_BYTES]) {
    /* Serialize explicitly: do not rely on MCU endianness or packed-struct layout. */
    write_le16(&frame[0], packet->magic);
    frame[2] = packet->mode;
    frame[3] = packet->battery_pct;
    write_le32(&frame[4], packet->timestamp_ms);
    write_le16(&frame[8], packet->jump_height_mm);
    write_le16(&frame[10], packet->step_count);
    write_le16(&frame[12], packet->foot_speed_cms);
    write_le16(&frame[14], packet->peak_impact_cg);
    write_le16(&frame[16], packet->takeoff_vel_cms);
    write_le16(&frame[18], packet->crc16);
}

int hw01_adapter_init(hw01_adapter_t *adapter,
                      const hw01_board_ops_t *ops,
                      dial_mode_t initial_mode,
                      uint32_t publish_interval_ms) {
    if (adapter == NULL || ops == NULL || ops->notify_telemetry == NULL ||
        initial_mode > DIAL_MODE_FULL_COURT) {
        return -1;
    }
    memset(adapter, 0, sizeof(*adapter));
    adapter->ops = *ops;
    adapter->publish_interval_ms = publish_interval_ms == 0U ? 100U : publish_interval_ms;
    dial_firmware_init(initial_mode);
    return 0;
}

void hw01_adapter_on_sample(hw01_adapter_t *adapter, const imu_sample_t *sample) {
    if (adapter == NULL || sample == NULL || adapter->ops.notify_telemetry == NULL) {
        return;
    }

    dial_telemetry_packet_t packet;
    memset(&packet, 0, sizeof(packet));
    dial_process_imu_sample(sample, &packet);

    if (adapter->ops.read_battery_pct != NULL) {
        int battery = adapter->ops.read_battery_pct(adapter->ops.context);
        if (battery >= 0 && battery <= 100) {
            packet.battery_pct = (uint8_t)battery;
        }
    }

    /* The core checksum covers bytes 0..17; battery is byte 3, so recompute it. */
    uint8_t frame[TELEMETRY_PACKET_BYTES];
    encode_frame(&packet, frame);
    /* CRC-16/CCITT-FALSE, polynomial 0x1021, initial value 0xffff. */
    uint16_t crc = 0xffffU;
    for (uint8_t i = 0; i < 18U; ++i) {
        crc ^= (uint16_t)frame[i] << 8;
        for (uint8_t bit = 0; bit < 8U; ++bit) {
            crc = (crc & 0x8000U) ? (uint16_t)((crc << 1) ^ 0x1021U)
                                  : (uint16_t)(crc << 1);
        }
    }
    write_le16(&frame[18], crc);

    if (adapter->has_published != 0U &&
        (uint32_t)(sample->timestamp_ms - adapter->last_publish_ms) < adapter->publish_interval_ms) {
        return;
    }
    adapter->last_publish_ms = sample->timestamp_ms;
    adapter->has_published = 1U;
    (void)adapter->ops.notify_telemetry(adapter->ops.context, frame, TELEMETRY_PACKET_BYTES);
}

void hw01_adapter_on_detent(dial_mode_t mode) {
    if (mode <= DIAL_MODE_FULL_COURT) {
        dial_on_bezel_detent(mode);
    }
}
