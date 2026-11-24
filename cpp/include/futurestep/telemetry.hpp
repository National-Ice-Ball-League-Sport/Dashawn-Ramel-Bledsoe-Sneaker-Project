#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace futurestep {

inline constexpr std::string_view kVersion = "0.1.0-prerelease";
inline constexpr std::uint16_t kServiceUuid16 = 0xDB01;
inline constexpr std::size_t kFrameSize = 20;

struct TelemetryFrame {
    std::uint8_t mode{};
    std::uint8_t batteryPercent{};
    std::uint32_t timestampMs{};
    float jumpHeightCm{};
    std::uint16_t stepCount{};
    float footSpeedMps{};
    float peakImpactG{};
    float takeoffVelocityMps{};
};

std::string_view modeName(std::uint8_t mode) noexcept;
std::uint16_t crc16CcittFalse(const std::uint8_t* data, std::size_t length) noexcept;

class TelemetryDecoder {
public:
    [[nodiscard]] std::optional<TelemetryFrame>
    decode(const std::uint8_t* data, std::size_t length) const;

    [[nodiscard]] std::optional<TelemetryFrame>
    decode(const std::vector<std::uint8_t>& data) const;

    [[nodiscard]] bool validate(const std::uint8_t* data, std::size_t length) const noexcept;
};

} // namespace futurestep
