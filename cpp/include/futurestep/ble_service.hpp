#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "telemetry.hpp"

namespace futurestep {

class BleTransport {
public:
    virtual ~BleTransport() = default;

    virtual bool connect(const std::string& serviceUuid,
                         const std::string& telemetryUuid) = 0;
    virtual void disconnect() = 0;
    virtual bool subscribe(std::function<void(const std::vector<std::uint8_t>&)> callback) = 0;
};

class DeviceService {
public:
    explicit DeviceService(BleTransport& transport);

    bool connect();
    void disconnect();

    using FrameCallback = std::function<void(const TelemetryFrame&)>;
    bool startNotifications(FrameCallback callback);

private:
    BleTransport& transport_;
    FrameCallback callback_;
};

} // namespace futurestep
