; ---------------------------------------------------------
; ATmega328P - Dual LED Blink @ 1 second (16 MHz crystal)
; ---------------------------------------------------------

; -------------------------
; Register definitions
; -------------------------
.equ DDRB,   0x24
.equ PORTB,  0x25

.equ SPL,    0x3D
.equ SPH,    0x3E
.equ RAMEND, 0x08FF

; Timer1 registers
.equ TCCR1A, 0x80
.equ TCCR1B, 0x81
.equ TCNT1L, 0x84
.equ TCNT1H, 0x85
.equ TIFR1,  0x36

; LED bit positions
.equ LED1, 0          ; PB0
.equ LED2, 1          ; PB1

; ---------------------------------------------------------
.org 0x0000
rjmp main

; ---------------------------------------------------------
; 1 second delay using Timer1
; Clock: 16 MHz
; Prescaler: 1024
; 16,000,000 / 1024 = 15,625 ticks per second
; Preload = 65536 - 15625 = 49911
; ---------------------------------------------------------
delay_1s:
    ; Stop Timer1
    ldi r16, 0x00
    sts TCCR1B, r16

    ; Load preload value
    ldi r16, hi8(49911)
    sts TCNT1H, r16
    ldi r16, lo8(49911)
    sts TCNT1L, r16

    ; Clear overflow flag
    ldi r16, (1<<0)
    sts TIFR1, r16

    ; Start Timer1, prescaler = 1024 (CS12=1, CS10=1)
    ldi r16, (1<<2) | (1<<0)
    sts TCCR1B, r16
wait_ovf:
    lds r16, TIFR1
    sbrs r16, 0
    rjmp wait_ovf

    ret

; ---------------------------------------------------------
; Main program
; ---------------------------------------------------------
.global main
main:
    ; Initialize stack pointer
    ldi r16, lo8(RAMEND)
    sts SPL, r16
    ldi r16, hi8(RAMEND)
    sts SPH, r16

    ; Set PB0 and PB1 as outputs
    lds r16, DDRB
    ori r16, (1<<LED1) | (1<<LED2)
    sts DDRB, r16

; ---------------------------------------------------------
; Main loop
; ---------------------------------------------------------
loop:
    ; LED1 ON, LED2 OFF
    lds r16, PORTB
    ori r16, (1<<LED1)
    andi r16, ~(1<<LED2)
    sts PORTB, r16
    rcall delay_1s

    ; LED1 OFF, LED2 ON
    lds r16, PORTB
    ori r16, (1<<LED2)
    andi r16, ~(1<<LED1)
    sts PORTB, r16
    rcall delay_1s

    rjmp loop
    