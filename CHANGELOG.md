# Changelog

## 0.1.0-prerelease

- Added C++17 host SDK for 20-byte HW-01 telemetry parsing.
- Added CRC-16/CCITT-FALSE validation and mode decoding.
- Added a transport-neutral BLE service abstraction.
- Added a C++ firmware wrapper around the existing C DSP core.
- Added CMake targets and a demo executable.

Prerelease limitations: no platform-specific Bluetooth implementation is included; the firmware wrapper requires board-specific sensor and BLE adapter integration.
