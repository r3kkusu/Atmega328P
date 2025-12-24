# ATmega328P UART Assembly (Bare-Metal, macOS)

This project demonstrates **bare‑metal UART transmission** on an **ATmega328P** using **pure AVR assembly** compiled on **macOS** with **Homebrew AVR-GCC**.

It sends the string:

```
Hello from ATmega!
```

over **UART (TX)**, suitable for:
- Raspberry Pi UART RX
- USB‑TTL adapter
- Bluetooth modules (HC‑05 / HC‑06)

No Arduino libraries. No Arduino IDE.

---

## Hardware Setup

### Required
- ATmega328P (bare chip on breadboard)
- 5V power supply
- Programmer (USBasp or Arduino as ISP)
- UART receiver:
  - Raspberry Pi (GPIO RX)
  - USB‑TTL adapter
  - Bluetooth module

### UART Wiring
| ATmega328P | Target |
|-----------|--------|
| PD1 (TX)  | RX |
| GND       | GND |

⚠️ If connecting to Raspberry Pi, **level‑shift TX (5V → 3.3V)**.

---

## Toolchain (macOS)

### Install tools
```bash
brew tap osx-cross/avr
brew install avr-gcc avrdude
```

### Verify
```bash
avr-gcc --version
avrdude -v
```

---

## Source File

**`main.asm`**  
(Uses pure GNU AVR assembly — no preprocessor)

Key features:
- Manual register definitions
- FLASH‑resident string
- UART polling (no interrupts)
- Stack initialization
- GAS‑compatible syntax

---

## Compile

```bash
avr-gcc -mmcu=atmega328p -x assembler -o main.elf main.asm
avr-objcopy -O ihex main.elf main.hex
```

Output:
- `main.elf` – linked ELF
- `main.hex` – flashable image

---

## Flash to ATmega328P

### USBasp
```bash
avrdude -c usbasp -p m328p -B 10 -U flash:w:main.hex
```

### Arduino as ISP
```bash
avrdude -c arduino -p m328p -P /dev/tty.usbserial-XXXX -b 19200 -U flash:w:main.hex
```

---

## Expected Result

Once powered:
- ATmega immediately transmits:
```
Hello from ATmega!
```
- Repeats continuously

---

## Why `.asm` (Not `.S`)?

This project intentionally uses **`.asm`** to:
- Avoid C preprocessor
- Teach direct hardware addressing
- Show real register locations
- Deepen understanding of AVR internals

For production projects, `.S` + `<avr/io.h>` is recommended.

---

## Key Lessons Learned

- Difference between AVRASM vs GNU GAS
- FLASH vs SRAM access (`lpm`)
- UART register polling
- macOS AVR toolchain quirks
- Why Arduino hides complexity

---

## Next Steps

- Add UART RX (two‑way communication)
- Interrupt‑driven UART
- Timer‑based delays
- SPI / I²C drivers
- Servo PWM with Timer1
