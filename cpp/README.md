# C++ SDK and Firmware Wrapper

The prerelease C++ layer provides:

- `0.1.0-prerelease` host SDK packet parsing and CRC validation.
- A transport-neutral BLE interface that can be implemented with platform BLE SDKs.
- A C++ RAII-style wrapper around the existing C firmware core.
- Demo mode without hardware.
- CMake build instructions and a host executable.

This is integration code, not a substitute for a physical BLE stack or board drivers.
