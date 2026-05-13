# Teensy 4.1 Drum Machine

A hardware drum machine built on the **Teensy 4.1** (ARM Cortex-M7, 600 MHz) featuring a 16-step sequencer, 6 sampled instruments, real-time parameter control, USB MIDI I/O, and EEPROM pattern persistence.

---

## Features

| Category | Details |
|---|---|
| **Sequencer** | 16-step pattern, 6 drum instruments, 60–180 BPM |
| **Play modes** | LivePlay · SeqEdit · SeqPlay (with live recording) |
| **Audio** | I2S output via SGTL5000 codec, stereo mixer with state-variable filter |
| **Filter** | LPF / BPF / HPF / High-resonance LP — cutoff swept by pot |
| **MIDI** | USB MIDI Note On/Off (GM ch 10), Start/Stop, 24-PPQ clock output |
| **Storage** | Pattern + BPM saved to flash-emulated EEPROM (hold STOP 2 s to save) |
| **Clock** | Hardware `IntervalTimer` (PIT) — microsecond precision, zero drift |
| **Debug** | `DEBUG_ENABLED 0/1` macro in `Config.h` — zero overhead in production |

---

## Hardware

### Microcontroller
- **Teensy 4.1** — ARM Cortex-M7 @ 600 MHz, Teensyduino 1.58+

### I/O Expansion
| Chip | I2C Address | Function |
|---|---|---|
| MCP23017 #1 | 0x20 | Step buttons 0–7 (Port A) / Step LEDs 0–7 (Port B) |
| MCP23017 #2 | 0x21 | Step buttons 8–15 (Port A) / Step LEDs 8–15 (Port B) |
| MCP23017 #3 | 0x22 | Mode buttons (Port A) / Instrument buttons + Metronome (Port B) |

I²C bus: **Wire2** — SDA pin 25, SCL pin 24

### Analog Inputs (10-bit ADC)
| Pin | Control |
|---|---|
| 26 | Filter cutoff |
| 27 | Master volume |
| 38 | Decay |
| 39 | Pitch |
| 40 | Level |
| 41 | BPM (60–180) |

### Digital Inputs (direct GPIO, active-low)
| Pins | Function |
|---|---|
| 28–31 | Play / Stop / Right / Left |
| 33–37 | Filter mode buttons (Off / LPF / BPF / HPF / Reso-LP) |

### Audio Output
- SGTL5000 codec over I2S (standard Teensy audio shield pinout)

---

## Firmware Architecture

```
DrumMachine.ino          ← main loop, wires all managers together
│
├── ClockEngine          ← hardware IntervalTimer ISR → step advance flag
├── Sequencer            ← 6×16 pattern matrix, step pointer
├── ButtonManager        ← debounce state machine (25 ms), edge detection
├── PotManager           ← 8-sample exponential smoothing, log filter mapping
├── ModeManager          ← LivePlay / SeqEdit / SeqPlay state machine
├── InstrumentManager    ← per-instrument decay / pitch / level params
├── MCPManager           ← I2C register R/W for MCP23017 chips + LED state
│
├── AudioEngine          ← Teensy Audio Library graph
│     voices (×6 AudioPlayMemory) → sub-mixers → master L/R
│     → AudioFilterStateVariable (LP/BP/HP) → selector mixer → I2S
│
├── MidiEngine           ← USB MIDI Note On/Off, transport, 24-PPQ clock
│     second IntervalTimer ISR for MIDI clock pulses
│
└── StorageManager       ← EEPROM.put/get, 16-byte packed save format
```

### Audio Signal Flow

```
kick  ──┐
snare   ├─► mix1/mix2 (stereo) ──┐
hat-cl  │                         ├─► mixer5/6 (master L/R)
hat-op  ┘                         │       │
                                  │       ▼
tom-lo ─┐                     mixer3/4  filterL/R (StateVariable)
tom-mid ┘─► mixer3/4 ─────────────┘       │  └──── LP / BP / HP outputs
                                           ▼
                                  filterSelectL/R  (gain-based output select)
                                           │
                                           ▼
                                      i2s_out L/R  →  SGTL5000
```

---

## Building

1. Install [Teensyduino](https://www.pjrc.com/teensy/td_download.html)
2. Open `Firmware/DrumMachine.ino` in Arduino IDE
3. Set **Tools → Board → Teensy 4.1**
4. Set **Tools → USB Type → Serial + MIDI** *(required for USB MIDI)*
5. Upload

### Debug output
In `Config.h`, set `#define DEBUG_ENABLED 1` and open the Serial Monitor at 115200 baud. All `DEBUG_PRINT` / `DEBUG_PRINTF` calls are compiled out when `DEBUG_ENABLED 0`.

---

## Usage

### Play modes
| Mode | Instrument buttons | Step buttons | Transport |
|---|---|---|---|
| **LivePlay** | Trigger sound instantly | — | — |
| **SeqEdit** | Select instrument to edit | Toggle step on/off | Edit only |
| **SeqPlay** | Trigger + live-record on current step | Toggle step | Play / Stop |

### Filter
Press any filter button (pins 33–37) to activate:
- **Off** — dry signal (bypass)
- **LPF** — low-pass sweep
- **BPF** — band-pass resonance
- **HPF** — high-pass (remove bass)
- **Reso-LP** — low-pass with high Q (Q = 3.5) for wub/synth-bass effect

Cutoff frequency is controlled by the filter pot (pin 26) with exponential mapping (200 Hz → 18 kHz).

### Saving & loading
- **Auto-load**: on power-up the last saved pattern is restored from EEPROM automatically.
- **Save**: hold **STOP** for 2 seconds while the sequencer is stopped. All step LEDs flash twice as confirmation.

---

## MIDI

The device appears as a USB MIDI device named **Teensy MIDI** when connected to a host.

| Event | MIDI message |
|---|---|
| Instrument trigger | Note On (ch 10, GM drum note, vel 100) + Note Off 20 ms later |
| PLAY pressed | MIDI Start (0xFA) |
| STOP pressed | MIDI Stop (0xFC) |
| Sequencer running | MIDI Clock (0xF8) at 24 PPQ |

GM drum note mapping: Kick 36 · Snare 38 · Hi-Hat Closed 42 · Hi-Hat Open 46 · Tom Low 45 · Tom Mid 47

---

## License

MIT
