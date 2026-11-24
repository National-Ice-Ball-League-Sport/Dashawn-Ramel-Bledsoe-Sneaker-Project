# Future-Step Smart Pivot Dial (`HW-01`)

**Creation by Dashawn Ramel Bledsoe**  
*Founder | Sports Product Designer | Entrepreneur Inventor at [National Ice Ball League Sport Inc.](https://niblsinc.wordpress.com)*

## National Ice Ball League Sport introduction: Future-Step DB-01 V1 / V2

**Dashawn Ramel Bledsoe Sneaker Project — Future-Step DB-01 V1 / V2**

Future-Step DB-01 is a National Ice Ball League Sport footwear and athlete-value concept. It connects the Future-Step smart sneaker, Biogrip smart-fabric insole, Hyper-Gel sole system, and Goalwrestling performance environment with National Ice Ball League Sport's proposed NIL financial index methodology.

National Ice Ball League Sport distinguishes **Model NIL** from **Market NIL**:

- **Model NIL** estimates an athlete's intrinsic Name, Image, and Likeness potential using consented biometric indices, athletic development, career stage, audience, brand fit, and documented assumptions.
- **Market NIL** measures actual marketplace value using verified sponsorship activity, licensing revenue, engagements, campaign results, merchandise demand, and comparable contracts.

### How the shoe works, step by step

1. **Establish the athlete profile.**  
   With the athlete's informed consent, National Ice Ball League Sport records the athlete's identity, sport, approved biography, career stage, and professional interests.

2. **Capture movement through the Future-Step system.**  
   The DB-01 concept combines a smart-fabric insole, Hyper-Gel cushioning and sole geometry, and the HW-01 Smart Pivot Dial to support approved movement measurements. The repository's current readings are software demonstrations until physical sensors and the complete shoe are independently validated.

3. **Create biometric indices.**  
   Validated measurements may cover workload, jump events, foot motion, consistency, change of direction, or session trends. Every index should publish its inputs, units, collection method, confidence level, and limitations.

4. **Calculate intrinsic NIL value.**  
   National Ice Ball League Sport may combine approved athlete data and documented assumptions into a transparent Model NIL estimate. A score expresses potential—not personal worth, guaranteed earnings, or compensation.

5. **Observe the actual marketplace.**  
   Market NIL uses verified sponsorship inquiries, signed agreements, licensed campaign fees, audience engagement, merchandise sales, public appearances, and comparable contracts.

6. **Compare Model NIL with Market NIL.**  
   Model NIL above Market NIL may indicate unmet exposure or commercial-development opportunities. Market NIL above Model NIL may indicate demand that is ahead of the underlying assumptions. Similar values may indicate alignment; differences require explanation.

7. **Build Goalwrestling and Tech + Ball pathways.**  
   With athlete approval, National Ice Ball League Sport can convert the analysis into event participation, sponsor packages, licensed products, education, or athlete storytelling.

8. **Protect the athlete and the data.**  
   Require informed consent, clear access controls, retention limits, deletion or correction procedures, and review rights. No biometric index or NIL estimate should determine eligibility, compensation, health status, or personal worth without lawful review and appropriate professional validation.

### DB-01 V1 and V2

- **V1 — Concept and prototype foundation:** footwear and sensor concepts, firmware scaffolding, BLE telemetry format, and a browser dashboard with a clearly labeled simulator.
- **V2 — Validation and refinement path:** proposed improvements to fit, smart-fabric sensing, Hyper-Gel cushioning, dial control, telemetry quality, manufacturing, and the National Ice Ball League Sport NIL methodology—subject to testing and review.

This project is a development and research framework, not a guaranteed valuation, investment product, medical system, or finished certified shoe. Model NIL and Market NIL require responsible methodology, independent review, athlete consent, and clear disclosure of uncertainty.

## Defensive publication and priority concerns

**Published:** 2026-11-24

This public repository is intended as a defensive disclosure of the Future-Step Smart Pivot Dial (`HW-01`), Future-Step footwear concepts, associated telemetry/protocol ideas, and related software and hardware documentation as of the publication date above. It may support public-availability and prior-art searches, subject to the laws and record requirements of the relevant jurisdiction.

A public repository does not by itself establish ownership, inventorship, validity, enforceability, patent eligibility, or a filing priority date. No patent or patent application status is guaranteed. Existing patents, applications, third-party rights, trade secrets, trademarks, contractual obligations, or earlier or later disclosures may affect patentability or priority. A GitHub timestamp or this notice is not a substitute for an official patent filing, USPTO or WIPO record, legal advice, or a documented invention record.

For priority analysis, preserve commit history, commit hashes, GitHub publication timestamps, and dated source and specification files. Before relying on this repository for a priority or prior-art position, consult a qualified patent professional to verify authorship, reduction to practice, enablement, disclosure dates, applicable grace periods, filing relationships, and jurisdiction-specific exceptions.

## Biogrip Smart Fabric, Hyper-Gel Sole, and derivatives

The National Ice Ball League Sport concept disclosed here may integrate Biogrip smart-fabric insole technology, Hyper-Gel sole technology, pressure-sensing fabric routes, cushioning and impact-response geometry, outsole grip surfaces, fit and retention features, the Smart Pivot Dial, and software and firmware used to interpret or present biomechanical measurements.

No open-source, permission-to-copy, patent, trademark, trade-secret, or other intellectual-property license is granted by this repository merely because code or documentation is public. Without a separate written agreement, viewers and downstream users should not presume authorization to copy, reverse-engineer, manufacture, modify, integrate, distribute, commercialize, or create derivative works from this project's distinctive materials, structures, arrangements, algorithms, designs, drawings, specifications, names, or systems.

This notice describes the project's claimed concepts and desired reservation of rights; it does not determine copyright ownership, inventorship, patentability, validity, priority, enforceability, or freedom to operate. Independent development, pre-existing public information, third-party rights, and separate factual questions may remain relevant.

Before using, adapting, manufacturing, or commercializing the concepts described here, obtain written authorization and independent intellectual-property advice. Preserve dated CAD files, schematics, bills of materials, material specifications, biomechanical calculations, simulations, test data, prototype records, communications, and inventor records.

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

The dashboard consumes the existing 20-byte telemetry frame described in `BLE_GATT_PROTOCOL.md`. It does not claim that a real shoe has been manufactured or that a device-specific sensor/BLE driver has been validated on target hardware.

### Run locally

```sh
python -m http.server 8000
```

Visit `http://localhost:8000` in a browser that supports Web Bluetooth. Browser/device support varies; HTTPS is required outside localhost. Use **Start demo** to explore without hardware.

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
