# ATmega328P UART in C (Bare‑Metal, macOS)

This project demonstrates **bare‑metal UART transmission written in C**
for the **ATmega328P**, compiled on **macOS (Apple Silicon / Intel)** using
the **AVR‑GCC cross‑toolchain**.

The firmware repeatedly sends:

```
Hello from ATmega!
```

over **UART TX (9600 baud)** every second.

No Arduino core. No Arduino IDE.  
This is **pure AVR‑GCC + avr‑libc**.

---

## What This Project Demonstrates

- Bare‑metal AVR C programming
- Direct register access (`UCSR0x`, `UBRR0x`, `UDR0`)
- UART initialization and polling
- Accurate delays using `_delay_ms()`
- Cross‑compilation on macOS
- Flashing with `avrdude`

---

## Hardware Requirements

### Required
- ATmega328P (bare chip or Arduino‑compatible board)
- 5V power supply
- Programmer:
  - USBasp **or**
  - Arduino as ISP
- UART receiver:
  - USB‑TTL adapter
  - Raspberry Pi UART RX
  - Bluetooth module (HC‑05 / HC‑06)

### UART Wiring (TX only)

| ATmega328P Pin | Connect To |
|---------------|------------|
| PD1 (TX)      | RX |
| GND           | GND |

⚠️ **Important:**  
Raspberry Pi RX is **3.3V tolerant only**.  
Use a voltage divider (e.g. 1kΩ + 2kΩ).

---

## Clock Requirement

This code assumes:

```c
#define F_CPU 16000000UL
```

You must use **one** of the following:

### Recommended
- 16 MHz crystal
- 2 × 22 pF capacitors

### Alternative
- Internal oscillator (requires correct fuse settings)
- UART baud accuracy may degrade

---

## Toolchain (macOS)

### Install AVR tools

```bash
brew tap osx-cross/avr
brew install avr-gcc avrdude
```

### Verify installation

```bash
avr-gcc --version
avrdude -v
```

---

## Source Code

**`samplefile.c`**

Key functions:
- `uart_init()` – configure UART hardware
- `uart_send_char()` – blocking TX
- `uart_send_string()` – send C strings
- `main()` – send message every second

Uses:
- `<avr/io.h>` for register definitions
- `<util/delay.h>` for `_delay_ms()`

---

## Compile

```bash
avr-gcc \
  -mmcu=atmega328p \
  -DF_CPU=16000000UL \
  -Os \
  -Wall \
  -o samplefile.elf \
  samplefile.c
```

### Why these flags?

| Flag | Purpose |
|----|----|
| `-mmcu=atmega328p` | Target MCU |
| `-DF_CPU=16000000UL` | Clock for `_delay_ms()` |
| `-Os` | Optimize for size |
| `-Wall` | Enable warnings |

---

## Convert to HEX

```bash
avr-objcopy -O ihex samplefile.elf samplefile.hex
```

`samplefile.hex` is what gets flashed to the chip.

---

## Flash to ATmega328P

### USBasp

```bash
avrdude -c usbasp -p m328p -B 10 -U flash:w:samplefile.hex
```

### Arduino as ISP

```bash
avrdude -c arduino -p m328p -P /dev/tty.usbserial-XXXX -b 19200 -U flash:w:samplefile.hex
```

(Replace `XXXX` with your serial port.)

---

## Expected Behavior

Once powered:

- UART continuously outputs:
```
Hello from ATmega!
```
- Message repeats every **1 second**
- No input required

---

## Why C Instead of Assembly?

Assembly:
- Maximum control
- Steep learning curve

C:
- Easier to write and maintain
- Still fully bare‑metal
- Direct register control
- Portable across AVR chips

This project shows the **middle ground**:
high‑level syntax with **low‑level control**.

---

## Comparison to Assembly Version

| Feature | Assembly | C |
|------|---------|---|
| Register access | Manual | Header‑defined |
| Delay | Timer loops | `_delay_ms()` |
| Readability | Low | High |
| Portability | Low | Higher |

---

## Next Steps

- Add UART RX and echo
- Interrupt‑driven UART
- Timer‑based scheduling (no busy‑wait)
- SPI / I²C drivers
- Servo PWM control

---

## License

MIT — free to use for learning and experimentation.
