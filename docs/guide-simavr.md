# simavr Validation & Setup Guide  
**Headless AVR Simulation on macOS (Apple Silicon / Intel)**

This document explains **how to install, build, run, and validate AVR firmware using simavr** without any GUI.
It is intended to be stored directly in a project repository as a **long‑term reference guide**.

---

## 0. What simavr Is (and Is Not)

simavr is a **cycle‑accurate AVR CPU + peripheral simulator**.

✔ Accurate timing  
✔ Peripheral‑level simulation  
✔ GDB debugging  
✔ GPIO / UART / IRQ tracing  

❌ No GUI  
❌ No virtual Arduino board  
❌ No visual LEDs  

If your firmware runs silently — **that is normal**.

---

# PART I — INSTALLATION & BUILD (macOS)

## 1. System Requirements

- macOS (Apple Silicon M1/M2/M3 or Intel)
- Xcode Command Line Tools
- Homebrew
- Terminal (zsh/bash)

---

## 2. Install Xcode Command Line Tools

```bash
xcode-select --install
```

Verify:

```bash
clang --version
```

---

## 3. Install Homebrew (if not installed)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Apple Silicon default path:
```
/opt/homebrew
```

---

## 4. Install simavr Dependencies

simavr requires **libelf**, **pkg-config**, and an **AVR toolchain**.

```bash
brew install \
  pkg-config \
  libelf \
  avr-gcc \
  avr-binutils \
  avr-libc \
  gtkwave
```

### Verify installations

```bash
avr-gcc --version
pkg-config --version
```

---

## 5. Fix pkg-config Path (IMPORTANT on Apple Silicon)

Homebrew installs `libelf.pc` in:

```
/opt/homebrew/lib/pkgconfig/libelf.pc
```

Export the path:

```bash
export PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig:/opt/homebrew/share/pkgconfig"
```

Verify:

```bash
pkg-config --modversion libelf
```

Expected output:
```
0.8.x
```

### Make it permanent

```bash
nano ~/.zshrc
```

Add:

```bash
export PKG_CONFIG_PATH="/opt/homebrew/lib/pkgconfig:/opt/homebrew/share/pkgconfig"
```

Reload:

```bash
source ~/.zshrc
```

---

## 6. Clone simavr

```bash
git clone https://github.com/buserror/simavr.git
cd simavr
```

---

## 7. Build simavr

```bash
make clean
make
```

Successful build will produce:

```text
run_avr
```

Verify architecture:

```bash
file run_avr
```

Expected (Apple Silicon):

```text
Mach-O 64-bit executable arm64
```

---

## 8. Verify simavr Installation

```bash
./run_avr --help
```

If usage info appears, simavr is ready.

---

# PART II — USING simavr (VALIDATION)

## 9. Why simavr Appears “Stuck”

If you see:

```text
Loaded XXX bytes of Flash data at 0
```

And nothing else:

✔ Firmware is running  
✔ CPU is executing  
✔ Program is likely in `while(1)`  

simavr **does not exit automatically**.
You must stop it manually:

```text
Ctrl + C
```

---

## 10. GPIO / LED Validation (Logic Analyzer Method)

### Example: Trace PB5 (Arduino LED)

```bash
./run_avr \
  -m atmega328p \
  -f 16000000 \
  --add-trace led=portpin@0x05/0x20 \
  -o main.vcd \
  main.elf
```

Stop with `Ctrl+C` to flush the VCD file.

### View waveform

```bash
gtkwave main.vcd
```

This validates:
- GPIO configuration
- Toggle behavior
- Timing accuracy

---

## 11. UART / printf Validation (Primary Method)

UART output is the **standard way professionals validate firmware logic**.

Example output:

```text
BOOT
TOGGLE
TOGGLE
```

This confirms:
- Code execution
- Loop logic
- System timing

---

## 12. GDB Debugging (Step-by-Step Execution)

### Run simavr with GDB stub

```bash
./run_avr -m atmega328p -g 1234 main.elf
```

### Attach GDB

```bash
avr-gdb main.elf
(gdb) target remote :1234
(gdb) break main
(gdb) continue
```

Capabilities:
- Instruction stepping
- Register inspection
- Memory analysis

---

## 13. Assertions & Runtime Guards

```c
#define ASSERT(cond) \
  if (!(cond)) { uart_print("ASSERT FAIL\n"); while(1); }

ASSERT(counter < 100);
```

If triggered:
- Firmware halts
- UART logs failure
- Final state visible in VCD

---

## 14. Best Practices

- Prefer **ELF** over HEX (symbols & debugging)
- Match `F_CPU` with `-f`
- Use UART for logic flow
- Use VCD for timing
- Use GDB for deep debugging
- Treat simavr as a **virtual lab bench**

---

## 15. Mental Model

> simavr = CPU + peripherals  
> GTKWave = logic analyzer  
> UART = serial console  
> GDB = hardware debugger  

There is no GUI because **the firmware is the interface**.

---

## 16. Conclusion

If you can:
- Observe GPIO transitions
- Read UART output
- Step code in GDB

Then your firmware is validated **as rigorously as real hardware with tools**.

---

**Platform:** macOS (Apple Silicon / Intel)  
**Simulator:** simavr  
**Use case:** Headless, professional AVR firmware validation
