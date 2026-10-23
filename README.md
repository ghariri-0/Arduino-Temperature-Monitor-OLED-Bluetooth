# Arduino Temperature Monitor — OLED + Bluetooth

An **Arduino Uno** reads temperature from a **DS18B20** waterproof probe, shows it on a **0.96" SSD1306 OLED** with a live history graph, and sends every reading to a phone over **HC-06 Bluetooth**.

> Training project at Ardunic (2026).

---

## Features

- 12-bit readings (0.0625 °C steps)
- **Non-blocking** sensor reads: `setWaitForConversion(false)` — the Arduino starts a conversion and checks back after 750 ms instead of waiting
- OLED screen:
  - large temperature with °C
  - highest (H) and lowest (L) value since start, and a reading counter (TX)
  - live graph of the last 64 points, auto-scaled (minimum span 1.00 °C)
- Bluetooth: every reading sent as text; send `R` from the phone to reset min/max
- Error handling: if the sensor is disconnected (−127 °C), the screen shows `SENSOR ERROR` and Bluetooth sends `T=ERR`
- Power-on value filter: the DS18B20 starts with **85.00 °C** in its memory. The first reading after power-up or after re-plugging the probe is skipped if it is exactly 85.00, so it cannot get stuck as the maximum or spike the graph. A real 85 °C is still accepted from the next reading.
- Low RAM use:
  - U8g2 **page mode** (128-byte display buffer instead of 1 KB)
  - history stored as `int16_t` (temperature × 100) in a 64-point **circular buffer** = 128 bytes

## Parts

| Part | Interface |
|---|---|
| Arduino Uno | — |
| DS18B20 waterproof probe + screw-terminal adapter board | 1-Wire |
| SSD1306 0.96" 128×64 OLED | I2C |
| HC-06 Bluetooth module | UART (SoftwareSerial, 9600 baud) |
| 3 equal resistors (for example 3 × 10 kΩ) | HC-06 RXD voltage divider |

## Wiring

| Module | Pin | Arduino |
|---|---|---|
| DS18B20 adapter | DAT | D2 |
| DS18B20 adapter | VCC / GND | 5 V / GND |
| OLED | SDA / SCL | A4 / A5 |
| OLED | VCC / GND | 5 V / GND |
| HC-06 | TXD | D10 |
| HC-06 | RXD | D11 **through the voltage divider** |
| HC-06 | VCC / GND | 5 V / GND |

### Why the HC-06 needs a voltage divider

The HC-06 RXD pin works at **3.3 V logic**, but Arduino D11 outputs **5 V**. Three equal resistors in series from D11 to GND, with RXD connected after the first resistor:

```
D11 ──[ R ]──┬──[ R ]──[ R ]── GND
             │
           HC-06 RXD
```

```
Vout = 5 V × 2R / (R + 2R) = 5 × 2/3 = 3.33 V
```

R cancels out — only the 1 : 2 ratio matters (the same as using 1 kΩ and 2 kΩ).
With 3 × 10 kΩ: current = 5 V / 30 kΩ ≈ 0.167 mA, 1.67 V across each resistor.

## Libraries

Install from the Arduino Library Manager:

- `OneWire`
- `DallasTemperature`
- `U8g2`

## Upload

1. Open `temperature_monitor/temperature_monitor.ino` in Arduino IDE.
2. Board: **Arduino Uno**. Select the port and upload.
3. USB Serial Monitor: **115200** baud.

## Output

Bluetooth (pair with the HC-06, open a Bluetooth serial terminal app):

```
T=24.56C
T=24.62C
T=ERR
```

Send `R` → `MIN/MAX RESET`.

OLED layout:

```
┌──────────────────────────────┐
│▌TEMP MONITOR        DS18B20 ▐│  ← header bar (inverted)
│                     H 25.12  │
│ 24.56 °C            L 23.88  │
│                     TX 128   │
│┌────────────────────────────┐│
││        ╱‾‾╲___╱‾‾          ││  ← last 64 points
│└────────────────────────────┘│
└──────────────────────────────┘
```

## How the code works

| Part | What it does |
|---|---|
| `setup()` | Starts serial ports, sensor (12-bit, non-blocking), OLED; starts the first conversion |
| `loop()` step 1 | When 750 ms have passed: read the sensor, update min/max, add every 3rd reading to the history, send over Bluetooth and USB, redraw, start the next conversion |
| `loop()` step 2 | Reads Bluetooth commands (`R` resets min/max) |
| `addToHistory()` | Writes into the circular buffer, overwriting the oldest point |
| `drawGraph()` | Finds min/max of stored points, scales them to 21 px, draws lines (newest on the right) |
| `drawScreen()` | U8g2 page loop: header, value, H/L/TX column, graph — or the error screen |

## Tests

```bash
sh tests/host/run_tests.sh
```

Compiles the real sketch on a PC (g++ only) with stub libraries and a scripted sensor, then checks: normal readings, the 85.00 °C power-on value at boot, unplugging and re-plugging the probe, that a real 85 °C is still accepted, and the full −55…125 °C range with a full 64-point history (every graph line must stay inside its frame). The version without the 85 °C filter fails two of these tests.

## License

MIT — see [LICENSE](LICENSE).
