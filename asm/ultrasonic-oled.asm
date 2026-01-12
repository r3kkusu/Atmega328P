; =========================================================
; ATmega328P – HC-SR04 + SSD1306 OLED (I2C)
; Clock: 16 MHz
; =========================================================

; ---------------- Registers ----------------
.equ DDRB,   0x24
.equ DDRC,   0x27
.equ DDRD,   0x2A
.equ PORTB,  0x25
.equ PORTC,  0x28
.equ PORTD,  0x2B
.equ PIND,   0x29

.equ TCCR1A, 0x80
.equ TCCR1B, 0x81
.equ TCNT1L, 0x84
.equ TCNT1H, 0x85

.equ TWBR,   0xB8
.equ TWSR,   0xB9
.equ TWDR,   0xBB
.equ TWCR,   0xBC

.equ SPL,    0x3D
.equ SPH,    0x3E
.equ RAMEND, 0x08FF

; ---------------- Pins ----------------
.equ TRIG_BIT, 2     ; PD2
.equ ECHO_BIT, 3     ; PD3
.equ TRIG_MASK, (1<<2)
.equ ECHO_MASK, (1<<3)

; ---------------- I2C ----------------
.equ OLED_ADDR, 0x3C

; =========================================================
.org 0x0000
rjmp main

; =========================================================
; I2C ROUTINES
; =========================================================
i2c_init:
    ldi r16, 72          ; ~100kHz @ 16MHz
    sts TWBR, r16
    clr r16
    sts TWSR, r16
    ret

i2c_start:
    ldi r16, (1<<7)|(1<<5)|(1<<2)
    sts TWCR, r16
wait_start:
    lds r16, TWCR
    sbrs r16, 7
    rjmp wait_start
    ret

i2c_write:
    sts TWDR, r17
    ldi r16, (1<<7)|(1<<2)
    sts TWCR, r16
wait_write:
    lds r16, TWCR
    sbrs r16, 7
    rjmp wait_write
    ret

i2c_stop:
    ldi r16, (1<<7)|(1<<4)|(1<<2)
    sts TWCR, r16
    ret

; =========================================================
; OLED COMMAND
; =========================================================
oled_cmd:
    rcall i2c_start
    ldi r17, OLED_ADDR<<1
    rcall i2c_write
    ldi r17, 0x00
    rcall i2c_write
    mov r17, r18
    rcall i2c_write
    rcall i2c_stop
    ret

oled_data:
    rcall i2c_start
    ldi r17, OLED_ADDR<<1
    rcall i2c_write
    ldi r17, 0x40
    rcall i2c_write
    mov r17, r18
    rcall i2c_write
    rcall i2c_stop
    ret

; =========================================================
; OLED INIT (SSD1306)
; =========================================================
oled_init:
    ldi r18, 0xAE        ; Display OFF
    rcall oled_cmd

    ldi r18, 0xD5        ; Set display clock
    rcall oled_cmd
    ldi r18, 0x80
    rcall oled_cmd

    ldi r18, 0xA8        ; Multiplex
    rcall oled_cmd
    ldi r18, 0x3F
    rcall oled_cmd

    ldi r18, 0xD3        ; Display offset
    rcall oled_cmd
    ldi r18, 0x00
    rcall oled_cmd

    ldi r18, 0x40        ; Start line
    rcall oled_cmd

    ldi r18, 0x8D        ; Charge pump
    rcall oled_cmd
    ldi r18, 0x14
    rcall oled_cmd

    ldi r18, 0x20        ; Memory mode
    rcall oled_cmd
    ldi r18, 0x00        ; Horizontal
    rcall oled_cmd

    ldi r18, 0xA1        ; Segment remap
    rcall oled_cmd

    ldi r18, 0xC8        ; COM scan
    rcall oled_cmd

    ldi r18, 0xDA        ; COM pins
    rcall oled_cmd
    ldi r18, 0x12
    rcall oled_cmd

    ldi r18, 0x81        ; Contrast
    rcall oled_cmd
    ldi r18, 0x7F
    rcall oled_cmd

    ldi r18, 0xA4        ; Resume RAM
    rcall oled_cmd

    ldi r18, 0xA6        ; Normal display
    rcall oled_cmd

    ldi r18, 0xAF        ; Display ON
    rcall oled_cmd

    ; Set cursor to page 0, column 0
    ldi r18, 0xB0
    rcall oled_cmd
    ldi r18, 0x00
    rcall oled_cmd
    ldi r18, 0x10
    rcall oled_cmd

    ret

; =========================================================
; PRINT CHAR (5x7 FONT, NUMBERS ONLY)
; =========================================================
oled_digit:
    subi r18, '0'
    lsl r18
    lsl r18
    lsl r18

    ldi ZH, hi8(font)
    ldi ZL, lo8(font)
    add ZL, r18
    adc ZH, r1

    ldi r19, 5
char_loop:
    lpm r18, Z+
    rcall oled_data
    dec r19
    brne char_loop

    ldi r18, 0x00
    rcall oled_data
    ret

; =========================================================
; FONT TABLE (0–9)
; =========================================================
font:
.byte 0x3E,0x51,0x49,0x45,0x3E   ; 0
.byte 0x00,0x42,0x7F,0x40,0x00   ; 1
.byte 0x42,0x61,0x51,0x49,0x46   ; 2
.byte 0x21,0x41,0x45,0x4B,0x31   ; 3
.byte 0x18,0x14,0x12,0x7F,0x10   ; 4
.byte 0x27,0x45,0x45,0x45,0x39   ; 5
.byte 0x3C,0x4A,0x49,0x49,0x30   ; 6
.byte 0x01,0x71,0x09,0x05,0x03   ; 7
.byte 0x36,0x49,0x49,0x49,0x36   ; 8
.byte 0x06,0x49,0x49,0x29,0x1E   ; 9

; =========================================================
; HC-SR04
; =========================================================
trigger:
    lds r16, PORTD
    ori r16, TRIG_MASK
    sts PORTD, r16
    ldi r18, 160
t1: dec r18
    brne t1
    andi r16, ~TRIG_MASK
    sts PORTD, r16
    ret

measure_echo:
wait_high:
    lds r16, PIND
    sbrs r16, ECHO_BIT
    rjmp wait_high
    clr r16
    sts TCNT1H, r16
    sts TCNT1L, r16
    ldi r16, (1<<1)
    sts TCCR1B, r16
wait_low:
    lds r16, PIND
    sbrc r16, ECHO_BIT
    rjmp wait_low
    clr r16
    sts TCCR1B, r16
    lds r24, TCNT1L
    lds r25, TCNT1H
    ret

; =========================================================
; MAIN
; =========================================================
.global main
main:
    ldi r16, lo8(RAMEND)
    sts SPL, r16
    ldi r16, hi8(RAMEND)
    sts SPH, r16

    ; TRIG out, ECHO in
    lds r16, DDRD
    ori r16, TRIG_MASK
    andi r16, ~ECHO_MASK
    sts DDRD, r16

    rcall i2c_init
    rcall oled_init

loop:
    rcall trigger
    rcall measure_echo

    ; cm ≈ ticks / 116
    mov r20, r25

    ; display
    mov r18, r20
    ori r18, '0'
    rcall oled_digit

    rjmp loop
