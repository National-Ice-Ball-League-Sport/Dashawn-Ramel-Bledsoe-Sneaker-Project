#include <iomanip>
#include <iostream>

#include "futurestep/telemetry.hpp"

int main() {
    std::cout << "Future-Step C++ SDK " << futurestep::kVersion << "\n";

    std::array<std::uint8_t, futurestep::kFrameSize> frame{};
    frame[0] = 0x01;
    frame[1] = 0xdb;
    frame[2] = 1;
    frame[3] = 96;
    frame[4] = 0x52;
    frame[5] = 0x10;
    frame[6] = 0x00;
    frame[7] = 0x00;
    frame[8] = 0x4e;
    frame[9] = 0x01;
    frame[10] = 0x9c;
    frame[11] = 0x01;
    frame[12] = 0xe5;
    frame[13] = 0x01;
    frame[14] = 0xe7;
    frame[15] = 0x01;
    frame[16] = 0xfc;
    frame[17] = 0x00;

    const auto crc = futurestep::crc16CcittFalse(frame.data(), 18);
    frame[18] = static_cast<std::uint8_t>(crc);
    frame[19] = static_cast<std::uint8_t>(crc >> 8);

    futurestep::TelemetryDecoder decoder;
    if (const auto packet = decoder.decode(frame.data(), frame.size())) {
        std::cout << std::fixed << std::setprecision(2)
                  << "Mode: " << futurestep::modeName(packet->mode) << '\n'
                  << "Battery: " << static_cast<unsigned>(packet->batteryPercent) << "%\n"
                  << "Jump: " << packet->jumpHeightCm << " cm\n"
                  << "Steps: " << packet->stepCount << '\n'
                  << "Speed: " << packet->footSpeedMps << " m/s\n"
                  << "Impact: " << packet->peakImpactG << " G\n"
                  << "Takeoff: " << packet->takeoffVelocityMps << " m/s\n";
        return 0;
    }

    std::cerr << "Demo telemetry frame failed validation.\n";
    return 1;
}
