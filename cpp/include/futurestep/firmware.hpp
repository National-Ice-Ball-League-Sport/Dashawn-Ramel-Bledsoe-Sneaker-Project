#pragma once

#include <cstdint>

#include "futurestep/telemetry.hpp"

extern "C" {
#include "dial_config.h"
}

namespace futurestep {

class FirmwareWrapper {
public:
    explicit FirmwareWrapper(std::uint8_t initialMode);

    void setMode(std::uint8_t mode);

    std::uint16_t processSample(float ax, float ay, float az,
                                float gx, float gy, float gz,
                                std::uint32_t timestampMs,
                                std::uint16_t sampleCount = 1);

private:
    std::uint32_t timestampMs_{};
};

} // namespace futurestep
