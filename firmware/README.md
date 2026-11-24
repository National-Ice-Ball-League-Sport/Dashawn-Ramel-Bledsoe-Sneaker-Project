# HW-01 board adapter integration contract

This adapter connects the repository's existing sensor-processing core to board-specific drivers without pretending a particular MCU, IMU, or BLE SDK has been selected. Implement the callbacks for the actual schematic and vendor SDK, then call `hw01_adapter_on_sample()` from a single sensor task.

`hw01_board_ops_t` callbacks:

- `read_battery_pct`: return 0–100, or a negative value if unavailable.
- `notify_telemetry`: send a 20-byte notification on the documented telemetry characteristic; return 0 on success.

The caller provides calibrated/converted IMU samples and timestamps in milliseconds. Use a monotonic clock and honor the selected IMU sample rate. The adapter throttles BLE publication to 10 Hz by default while the DSP core may process samples faster. Adjust this only after checking radio throughput, power consumption, and the receiving app. `dial_firmware_init`, `dial_on_bezel_detent`, and `dial_process_imu_sample` are implemented in the existing root `main.c`.

The core is global-state based; do not call it concurrently from multiple tasks. Hardware driver initialization, sensor range / ODR configuration, interrupt handling, flash persistence, secure pairing, connection lifecycle, and battery safety are intentionally board/application responsibilities.
