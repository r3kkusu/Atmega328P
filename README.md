# Bare-Metal ATmega328P UART Examples  
**Assembly · C · C++ (macOS, No Arduino)**

This repository demonstrates **bare-metal programming on the ATmega328P**
using **three different languages**:

- 🧠 **Assembly (GNU AVR GAS)**
- ⚙️ **C (avr-gcc + avr-libc)**
- 🧩 **C++ (avr-g++, embedded-safe subset)**

All examples:
- Run **without Arduino core or Arduino IDE**
- Are compiled on **macOS (Apple Silicon / Intel)**
- Use **direct register access**
- Transmit data over **UART (TX)**

---

## 🎯 What This Repository Is For

This project is intended to:

- Teach **how microcontrollers really work**
- Show the **differences between ASM, C, and C++ on bare metal**
- Demonstrate **cross-compilation** (Mac → AVR)
- Provide **clean, documented reference implementations**
- Serve as a **learning or portfolio repository**

If you’ve ever wondered *“what Arduino hides”*, this repo answers that.

---

## 📁 Repository Structure

```text
.
├── README.md          ← You are here
├── asm/               ← Pure AVR assembly version
│   ├── main.asm
│   └── README.md
├── c/                 ← Bare-metal C version
│   ├── main.c
│   └── README.md
├── cpp/               ← Bare-metal C++ version
│   ├── main.cpp
│   └── README.md
├── .gitignore