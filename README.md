# Future-Step Smart Pivot Dial (`HW-01`)

**Creation by Dashawn Ramel Bledsoe**  
*Founder | Sports Product Designer | Entrepreneur Inventor at [National Ice Ball League Sport Inc.](https://niblsinc.wordpress.com)*

Prototype firmware, hardware integration contract, BLE telemetry SDK, and browser application for the **Future-Step Signature Edition (`DB-01-FS-GRN`)**, a tech-enabled footwear concept for National Ice Ball League Sport Inc.

## Defensive publication and priority concerns

**Published:** 2026-11-24

This public repository is intended as a defensive disclosure of the Future-Step Smart Pivot Dial (`HW-01`), Future-Step footwear concepts, associated telemetry/protocol ideas, and related software and hardware documentation as of the publication date above. It may be used to support public-availability and prior-art searches concerning these concepts, subject to the laws and record requirements of the relevant jurisdiction.

A public repository does not by itself establish ownership, inventorship, validity, enforceability, patent eligibility, or a filing priority date. No patent or patent application status is guaranteed. Existing patents, applications, third-party rights, trade secrets, trademarks, contractual obligations, or earlier or later disclosures may affect patentability or priority. A GitHub commit timestamp, repository date, or this notice is not a substitute for an official patent filing, USPTO or WIPO record, legal advice, or a documented invention record.

For priority analysis, preserve the following repository evidence: the commit history, commit hashes, GitHub publication timestamps, and dated source/specification files. Before relying on this repository for a priority or prior-art position, an intellectual-property attorney or qualified patent professional should verify authorship, reduction to practice, enablement, the governing disclosure date, and whether an applicable grace period, filing relationship, or other jurisdiction-specific exception applies.

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

The dashboard consumes the existing 20-byte telemetry frame described in `BLE_GATT_PROTOCOL.md`. It does not claim that a real shoe has been manufactured or that a device-specific sensor/BLE driver has been validated on the target hardware.

### Run locally

```sh
python -m http.server 8000
```

Then visit `http://localhost:8000` in a browser that supports Web Bluetooth. Browser/device support varies; HTTPS is required outside localhost. Use **Start demo** to explore without hardware.

## Hardware / embedded foundation

- **Sensor and processing:** 6-axis IMU sampling, standstill calibration, jump estimation, step detection, and telemetry packing are represented by the existing C core (`main.c`, `kalman_jump.c`, `kalman_jump.h`). These implementations are conceptual or prerelease and require validation.
- **Board boundary:** `firmware/board_adapter.h` and `.c` define a small platform-neutral adapter. Board-specific code supplies sensor reads, battery readings, and BLE notifications. Use one task/loop context and keep callback behavior non-blocking.
- **Physical product:** the five-layer stack and dial modes below are design targets, not verified ratings. IP67, impact, fit/retention, battery life, thermal, charging, and sensor accuracy require testing and certification.

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

Service `0000db01-0000-1000-8000-00805f9b34fb`; notify characteristic `0000db02-0000-1000-8000-00805f9b34fb`. Payload fields and units are specified in `BLE_GATT_PROTOCOL.md`. The browser verifies frame headers and CRC-16/CCITT-FALSE values; a valid frame does not prove that a physical shoe or sensor is present.

## Prototype validation and safety

This repository is a software/hardware prototype, not a medical product or certified protective equipment. The dashboard's demo readings are synthetic. Do not use prototype measurements to make health, training, or safety decisions. Hardware, firmware, fit, impact protection, battery safety, and sensor accuracy remain unverified until independently tested.
