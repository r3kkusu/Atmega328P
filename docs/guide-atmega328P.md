
# ATmega328P Bare-Metal Wiring Guide (ISP + Power + LEDs)

This document contains the **complete, known-good wiring** for a bare **ATmega328P (DIP-28)**,
including **ISP programming (USBasp)**, **power options**, **clock recovery**, and **LED test wiring**.

---

## 📄 Official Datasheet (IMPORTANT)

Use this datasheet as the **authoritative reference** for pin functions, electrical limits,
clock configuration, fuse bits, and programming behavior.

**ATmega328P Automotive Datasheet (Microchip / Atmel-7810)**  
https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

---

## 1. ATmega328P DIP-28 Pin Map (Top View)

```
                 ┌───────── notch ─────────┐
 RESET      (1)  │●                         │ (28) PC5
 RXD        (2)  │                          │ (27) PC4
 TXD        (3)  │                          │ (26) PC3
            (4)  │                          │ (25) PC2
            (5)  │                          │ (24) PC1
            (6)  │                          │ (23) PC0
 VCC        (7)  │                          │ (22) GND
 GND        (8)  │                          │ (21) AREF
 XTAL1      (9)  │                          │ (20) AVCC
 XTAL2     (10)  │                          │ (19) PB5  ← SCK
            (11) │                          │ (18) PB4  ← MISO
            (12) │                          │ (17) PB3  ← MOSI
            (13) │                          │ (16) PB2
 LED1 PB0  (14)  │                          │ (15) PB1  LED2
                 └──────────────────────────┘
```

---

## 2. Power Wiring (MANDATORY)

```
Pin 7  (VCC)   → +5V
Pin 20 (AVCC)  → +5V
Pin 8  (GND)   → GND
Pin 22 (GND)   → GND
```

### Required Decoupling Capacitor
```
0.1µF ceramic capacitor
Pin 7 (VCC) ──||── Pin 8 (GND)
```

---

## 3. Power Supply Options

### Option A — USBasp Power
- Enable USBasp VCC jumper
- Supplies +5V via ISP cable

### Option B — External 5V Power (SAFE)
```
External +5V → Pin 7 and Pin 20
External GND → Pin 8 and Pin 22
```

⚠️ Remove USBasp VCC jumper  
⚠️ Grounds must be common

---

## 4. ISP Wiring (USBasp → ATmega328P)

```
USBasp MISO  → Pin 18 (PB4)
USBasp MOSI  → Pin 17 (PB3)
USBasp SCK   → Pin 19 (PB5)
USBasp RESET → Pin 1
USBasp VCC   → +5V (if used)
USBasp GND   → GND
```

---

## 5. Reset Pull-up (Recommended)

```
Pin 1 (RESET) ── 10kΩ ── +5V
```

---

## 6. Clock Wiring (Only if Fused for External Clock)

```
Pin 9  (XTAL1) ─┐
                ├── 16 MHz crystal
Pin 10 (XTAL2) ─┘

Pin 9  ── 22pF ── GND
Pin 10 ── 22pF ── GND
```

---

## 7. LED Wiring

```
Pin 14 (PB0) ── 220Ω ──►|── GND
Pin 15 (PB1) ── 220Ω ──►|── GND
```

---

## 8. Programming Commands

```
avrdude -c usbasp -p m328p
avrdude -c usbasp -p m328p -U flash:w:dual_led.hex
```

---

## 9. Summary

This wiring is equivalent to the **core of an Arduino Uno** without USB or bootloader.
