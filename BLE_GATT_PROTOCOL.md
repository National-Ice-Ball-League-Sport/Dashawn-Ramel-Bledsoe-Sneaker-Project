# BLE 5.3 GATT Protocol (`HW-01`)
**Service UUID:** `0000db01-0000-1000-8000-00805f9b34fb`  
**Telemetry Characteristic (`Notify`):** `0000db02-0000-1000-8000-00805f9b34fb` (20-byte little-endian packed struct)

| Offset | Size | Field | Unit |
| :--- | :--- | :--- | :--- |
| `0` | 2B | `magic` | `0xDB01` |
| `2` | 1B | `mode` | `0..3` |
| `3` | 1B | `battery_pct` | `%` |
| `4` | 4B | `timestamp_ms` | `ms` |
| `8` | 2B | `jump_height_mm` | `mm` (1D Kalman fused) |
| `10` | 2B | `step_count` | `steps` |
| `12` | 2B | `foot_speed_cms` | `cm/s` |
| `14` | 2B | `peak_impact_cg` | `0.01 G` |
| `16` | 2B | `takeoff_vel_cms` | `cm/s` (Kalman takeoff velocity) |
| `18` | 2B | `crc16` | CCITT-FALSE |
