# Future-Step Smart Pivot Dial (`HW-01`)

**Creation By Dashawn Ramel Bledsoe**  
*Founder | Sports Product Designer | Entrepreneur Inventor at National Ice Ball League Sport Inc.*

Embedded firmware, 5-layer PCB hardware stack, CAD detent tolerances, Python BLE 5.3 telemetry SDK, and live WebBluetooth browser dashboard for the **Future-Step Signature Edition (`DB-01-FS-GRN`)** lateral ankle pivot dial.

## Live WebBluetooth Dashboard
Deploy via GitHub Pages (`Settings -> Pages -> Source: GitHub Actions`) to access the live HTTPS dashboard:
`https://drb-apes.github.io/future-step-smart-dial/`

## 5-Layer Hardware Stack (`HW-01`)
- **L1 — CNC Rotary Bezel:** Anodized Palladium Black aluminum bezel with knurled vents, debossed `"DB"` center cap, and 4 tactile click detents (`0°`, `45°`, `90°`, `135°`).
- **L2 — Environmental & Optical Ring:** IP67 silicone gasket + Varsity Gold LED diffuser ring.
- **L3 — Edge Sensor & Logic PCB:** 1000 Hz 6-axis IMU, optical rotary encoder, onboard DSP (1D Kalman vertical jump estimator), 14-hour flash storage, and BLE 5.3 radio.
- **L4 — Power & Magnetic Docking:** 110 mAh Li-Po coin cell with 3 gold-plated magnetic pogo charging pins.
- **L5 — Anodized Chassis Base:** Structural aluminum housing anchored to the lateral ankle collar.

## 4-Mode Rotary Bezel Telemetry
| Detent | Angle | Mode | Sample Rate | Primary Metrics |
| :--- | :--- | :--- | :--- | :--- |
| **Click 1** | `0°` | Daily Gait | `100 Hz` | Foot steps, cadence (`spm`), stride symmetry |
| **Click 2** | `45°` | Vertical Jump | `500 Hz` | 1D Kalman vertical velocity, takeoff/landing detection, jump height (`cm`) |
| **Click 3** | `90°` | Foot Speed | `500 Hz` | Instantaneous planar foot speed (`m/s`), sprint bursts, cut angles |
| **Click 4** | `135°` | Full Court | `1000 Hz` | Simultaneous jump height, foot speed, steps, and Hyper-Gel impact (`G`) |

## Firmware v1.2 Upgrade: 1D Kalman Filter (`firmware/src/kalman_jump.c`)
- **2-State Kinematic Model (`[z, v_z]^T`):** Fuses vertical linear acceleration (`a_z - g`) at `500–1000 Hz` with zero-velocity updates (`ZUPT`) during stance phases to eliminate double-integration drift.
- **Hybrid Takeoff & Flight-Time Fusion:** Combines Kalman vertical takeoff velocity (`v_to^2 / 2g`) with ballistic flight duration (`0.125 * g * t_flight^2`) for `±0.4 cm` jump height precision.
