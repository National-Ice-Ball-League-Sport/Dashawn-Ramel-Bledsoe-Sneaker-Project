#include "futurestep/firmware.hpp"

extern "C" {
void dial_firmware_init(dial_mode_t initial_mode);
void dial_on_bezel_detent(dial_mode_t new_mode);
void dial_process_imu_sample(const imu_sample_t* sample,
                             dial_telemetry_packet_t* out_packet);
}

namespace futurestep {

FirmwareWrapper::FirmwareWrapper(std::uint8_t initialMode) {
    dial_firmware_init(static_cast<dial_mode_t>(initialMode));
}

void FirmwareWrapper::setMode(std::uint8_t mode) {
    dial_on_bezel_detent(static_cast<dial_mode_t>(mode));
}

std::uint16_t FirmwareWrapper::processSample(float ax, float ay, float az,
                                             float gx, float gy, float gz,
                                             std::uint32_t timestampMs,
                                             std::uint16_t) {
    timestampMs_ = timestampMs;
    imu_sample_t sample{ax, ay, az, gx, gy, gz, timestampMs};
    dial_telemetry_packet_t packet{};
    dial_process_imu_sample(&sample, &packet);
    return packet.jump_height_mm;
}

} // namespace futurestep
