# ATmega328P UART in C++ (Bare-Metal, macOS)

This project demonstrates **bare-metal UART transmission written in C++**
for the **ATmega328P**, compiled on **macOS (Apple Silicon / Intel)** using the
**AVR-GCC C++ cross-toolchain (`avr-g++`)**.

The firmware repeatedly sends:

```
Hello from ATmega!
```

over **UART TX (9600 baud)** every second.

No Arduino core. No Arduino IDE.  
This is **pure AVR C++ with direct register access**.

---

## What This Project Demonstrates

- Bare-metal AVR programming in **C++**
- Using **classes** without Arduino
- Direct register manipulation (`UCSR0x`, `UBRR0x`, `UDR0`)
- UART polling (no interrupts)
- Safe embedded C++ practices (no heap, no exceptions)
- Cross-compilation on macOS
- Flashing with `avrdude`

---

## Hardware Requirements

### Required
- ATmega328P (bare chip or Arduino-compatible board)
- 5V power supply
- Programmer:
  - USBasp **or**
  - Arduino as ISP
- UART receiver:
  - USB-TTL adapter
  - Raspberry Pi UART RX
  - Bluetooth module (HC-05 / HC-06)

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
avr-g++ --version
avrdude -v
```

---

## Source Code

**`main.cpp`**

Key components:
- `class UART`
- `UART::init()` – configure UART hardware
- `UART::send_char()` – blocking TX
- `UART::send_string()` – send C strings
- `main()` – send message every second

Uses:
- `<avr/io.h>` for register definitions
- `<util/delay.h>` for `_delay_ms()`

---

## Compile (C++)

```bash
avr-g++ \
  -mmcu=atmega328p \
  -DF_CPU=16000000UL \
  -Os \
  -Wall \
  -fno-exceptions \
  -fno-rtti \
  -o main.elf \
  main.cpp
```

### Why these flags?

| Flag | Purpose |
|----|----|
| `avr-g++` | C++ frontend |
| `-mmcu=atmega328p` | Target MCU |
| `-DF_CPU=16000000UL` | Clock for `_delay_ms()` |
| `-Os` | Optimize for size |
| `-fno-exceptions` | Disable exceptions |
| `-fno-rtti` | Disable RTTI |

---

## Convert to HEX

```bash
avr-objcopy -O ihex main.elf main.hex
```

`main.hex` is what gets flashed to the chip.

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

## Why Use C++ on AVR?

C++ is useful on microcontrollers when used carefully:

### ✔ Safe to use
- Classes
- Member functions
- Stack-allocated objects
- `constexpr`
- `inline`

### ❌ Avoid
- Exceptions
- RTTI
- `new` / `delete`
- STL containers
- Virtual functions (unless fully understood)

This project shows **idiomatic embedded C++**, not Arduino-style abstractions.

---

## Comparison (Assembly vs C vs C++)

| Aspect | Assembly | C | C++ |
|------|----------|---|-----|
| Control | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ |
| Readability | ⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Code size | Smallest | Medium | Slightly larger |
| Abstraction | None | Low | Medium |

---

## Next Steps

- Add UART RX using a C++ method
- Interrupt-driven UART class
- Timer-based scheduler class
- SPI / I²C drivers in C++
- Compare flash usage vs C and ASM

---

## License

MIT — free to use for learning and experimentation.
