#include "futurestep/telemetry.hpp"

#include <cstring>

namespace futurestep {

namespace {

std::uint16_t readLe16(const std::uint8_t* data) noexcept {
    return static_cast<std::uint16_t>(data[0]) |
           (static_cast<std::uint16_t>(data[1]) << 8);
}

std::uint32_t readLe32(const std::uint8_t* data) noexcept {
    return static_cast<std::uint32_t>(data[0]) |
           (static_cast<std::uint32_t>(data[1]) << 8) |
           (static_cast<std::uint32_t>(data[2]) << 16) |
           (static_cast<std::uint32_t>(data[3]) << 24);
}

}

std::string_view modeName(std::uint8_t mode) noexcept {
    switch (mode) {
        case 0: return "Daily gait";
        case 1: return "Vertical jump";
        case 2: return "Foot speed";
        case 3: return "Full court";
        default: return "Unknown";
    }
}

std::uint16_t crc16CcittFalse(const std::uint8_t* data, std::size_t length) noexcept {
    std::uint16_t crc = 0xffff;
    for (std::size_t i = 0; i < length; ++i) {
        crc ^= static_cast<std::uint16_t>(data[i]) << 8;
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc & 0x8000u)
                    ? static_cast<std::uint16_t>((crc << 1) ^ 0x1021u)
                    : static_cast<std::uint16_t>(crc << 1);
        }
    }
    return crc;
}

bool TelemetryDecoder::validate(const std::uint8_t* data, std::size_t length) const noexcept {
    if (data == nullptr || length != kFrameSize) {
        return false;
    }
    if (readLe16(data) != 0xdb01u) {
        return false;
    }
    const std::uint16_t supplied = readLe16(data + 18);
    return supplied == crc16CcittFalse(data, 18);
}

std::optional<TelemetryFrame>
TelemetryDecoder::decode(const std::uint8_t* data, std::size_t length) const {
    if (!validate(data, length) || data[2] > 3) {
        return std::nullopt;
    }

    TelemetryFrame frame;
    frame.mode = data[2];
    frame.batteryPercent = data[3];
    frame.timestampMs = readLe32(data + 4);
    frame.jumpHeightCm = static_cast<float>(readLe16(data + 8)) / 10.0f;
    frame.stepCount = readLe16(data + 10);
    frame.footSpeedMps = static_cast<float>(readLe16(data + 12)) / 100.0f;
    frame.peakImpactG = static_cast<float>(readLe16(data + 14)) / 100.0f;
    frame.takeoffVelocityMps = static_cast<float>(readLe16(data + 16)) / 100.0f;
    return frame;
}

std::optional<TelemetryFrame>
TelemetryDecoder::decode(const std::vector<std::uint8_t>& data) const {
    return decode(data.data(), data.size());
}

} // namespace futurestep
