import argparse
import struct
from dataclasses import dataclass

MODES = {0: "Daily Gait (100Hz)", 1: "Vertical Jump (500Hz)", 2: "Foot Speed (500Hz)", 3: "Full Court (1000Hz)"}

@dataclass
class DialFrame:
    mode: str
    battery_pct: int
    timestamp_ms: int
    jump_height_cm: float
    step_count: int
    foot_speed_mps: float
    peak_impact_g: float
    takeoff_vel_mps: float

def parse_packet(raw: bytes) -> DialFrame:
    if len(raw) != 20:
        raise ValueError(f"Expected 20-byte frame, got {len(raw)}")
    magic, mode, batt, ts, jump_mm, steps, speed_cms, impact_cg, to_vel_cms, _crc = struct.unpack("<HBB I HHHHH H", raw)
    if magic != 0xDB01:
        raise ValueError(f"Invalid magic header: 0x{magic:04X}")
    return DialFrame(
        mode=MODES.get(mode, "Unknown"),
        battery_pct=batt,
        timestamp_ms=ts,
        jump_height_cm=round(jump_mm / 10.0, 1),
        step_count=steps,
        foot_speed_mps=round(speed_cms / 100.0, 2),
        peak_impact_g=round(impact_cg / 100.0, 2),
        takeoff_vel_mps=round(to_vel_cms / 100.0, 2),
    )

if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--demo", action="store_true")
    args = ap.parse_args()
    if args.demo:
        demo_pkt = struct.pack("<HBB I HHHHH H", 0xDB01, 1, 96, 4250, 334, 412, 485, 487, 252, 0x1234)
        print(parse_packet(demo_pkt))
