# Future-Step Smart Pivot Dial (`HW-01`)

**Creation by Dashawn Ramel Bledsoe**  
*Founder | Sports Product Designer | Entrepreneur Inventor at [National Ice Ball League Sport Inc.](https://niblsinc.wordpress.com)*

Prototype firmware, hardware integration contract, BLE telemetry SDK, and browser application for the **Future-Step Signature Edition (`DB-01-FS-GRN`)**, a tech-enabled footwear concept for National Ice Ball League Sport Inc.

## Organization affiliation

This project is affiliated with **National Ice Ball League Sport Inc.**, led by Dashawn Ramel Bledsoe as Founder and Sports Product Designer.

- Organization: [National Ice Ball League Sport Inc.](https://niblsinc.wordpress.com)
- Creator: Dashawn Ramel Bledsoe
- Role: Founder and Sports Product Designer

## Athlete application and dashboard

Open `index.html` from a local HTTPS host or deploy this repository with GitHub Pages to use the responsive browser dashboard. It supports:

- BLE pairing with the HW-01 telemetry service and display of jump height, steps, estimated planar speed, impact, battery, and takeoff velocity.
- Frame header and CRC-16/CCITT-FALSE validation before telemetry is displayed.
- A clearly labeled simulator, recent session history in browser local storage, trend chart, session reset, and CSV export.
- Daily gait, vertical jump, foot speed, and full-court display modes.

The dashboard consumes the existing 20-byte telemetry frame described in `BLE_GATT_PROTOCOL.md`. It does not claim that a real shoe has been manufactured or that a device-specific sensor/BLE driver has been validated.

### Run locally

```sh
python -m http.server 8000
```

Then visit `http://localhost:8000` in a browser that supports Web Bluetooth. Browser/device support varies; HTTPS is required outside localhost. Use **Start demo** to explore without hardware.

## Hardware / embedded foundation

- **Sensor and processing:** 6-axis IMU sampling, standstill calibration, jump estimation, step detection, and telemetry packing are represented by the existing C core (`main.c`, `kalman_jump.c`, `telemetry.c`, and `frame_codec.c`).
- **Board boundary:** `firmware/board_adapter.h` and `.c` define a small platform-neutral adapter. Board-specific code supplies sensor reads, battery readings, and BLE notifications. Use one task/loop owner for the C core.
- **Physical product:** the five-layer stack and dial modes below are design targets, not verified ratings. IP67, impact, fit/retention, battery life, thermal, charging, and sensor accuracy require engineering validation.

## 5-layer hardware concept (`HW-01`)
- **L1 — CNC rotary bezel:** anodized palladium-black aluminum, tactile detents at `0°`, `45°`, `90°`, and `135°`.
- **L2 — Environmental / optical ring:** proposed silicone gasket and LED diffuser.
- **L3 — Edge sensor / logic PCB:** proposed 6-axis IMU, encoder, MCU/DSP, flash, and BLE radio.
- **L4 — Power / docking:** proposed Li-Po cell and magnetic charging contacts. Battery protection and charging circuitry must be designed and tested for the actual cell.
- **L5 — Chassis:** proposed structural housing at the lateral ankle collar.

## Four rotary modes

| Detent | Angle | Mode | Target sample rate | Example metrics |
| --- | ---: | --- | ---: | --- |
| 1 | `0°` | Daily gait | 100 Hz | Steps, cadence, stride symmetry |
| 2 | `45°` | Vertical jump | 500 Hz | Takeoff / landing, jump estimate |
| 3 | `90°` | Foot speed | 500 Hz | Planar foot-speed estimate |
| 4 | `135°` | Full court | 1000 Hz | Combined movement and impact estimates |

## BLE telemetry

Service `0000db01-0000-1000-8000-00805f9b34fb`; notify characteristic `0000db02-0000-1000-8000-00805f9b34fb`. Payload fields and units are specified in `BLE_GATT_PROTOCOL.md`. The browser verifies the frame header, checks CRC-16/CCITT-FALSE, and rejects malformed or CRC-failed packets.

## Prototype validation and safety

This repository is a software/hardware prototype, not a medical product or certified protective equipment. The dashboard's demo readings are synthetic. Do not use prototype measurements to make health, coaching, or safety decisions until validated by appropriate testing.
