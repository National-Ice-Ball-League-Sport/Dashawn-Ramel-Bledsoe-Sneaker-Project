#include "futurestep/ble_service.hpp"

#include <utility>

namespace futurestep {

namespace {
constexpr char kServiceUuid[] = "0000db01-0000-1000-8000-00805f9b34fb";
constexpr char kTelemetryUuid[] = "0000db02-0000-1000-8000-00805f9b34fb";
}

DeviceService::DeviceService(BleTransport& transport) : transport_(transport) {}

bool DeviceService::connect() {
    return transport_.connect(kServiceUuid, kTelemetryUuid);
}

void DeviceService::disconnect() {
    transport_.disconnect();
}

bool DeviceService::startNotifications(FrameCallback callback) {
    callback_ = std::move(callback);
    TelemetryDecoder decoder;

    return transport_.subscribe([this, decoder](const std::vector<std::uint8_t>& bytes) mutable {
        if (const auto frame = decoder.decode(bytes); frame && callback_) {
            callback_(*frame);
        }
    });
}

} // namespace futurestep
