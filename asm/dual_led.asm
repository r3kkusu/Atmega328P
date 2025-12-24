; =========================
; Register definitions (ATmega328P)
; =========================
.equ DDRB,   0x24
.equ PORTB,  0x25
.equ SPL,    0x3D
.equ SPH,    0x3E
.equ RAMEND, 0x08FF

.equ LED1, 0
.equ LED2, 1

.org 0x0000
rjmp main

; -------------------------
; Delay ~1 second (approx)
; -------------------------
delay_1s:
    ldi r18, 255
d1:
    ldi r19, 255
d2:
    ldi r20, 255
d3:
    dec r20
    brne d3
    dec r19
    brne d2
    dec r18
    brne d1
    ret

; -------------------------
; Main
; -------------------------
.global main
main:
    ; Stack init
    ldi r16, lo8(RAMEND)
    sts SPL, r16
    ldi r16, hi8(RAMEND)
    sts SPH, r16

    ; Set PB0 and PB1 as outputs
    lds r16, DDRB
    ori r16, (1<<LED1) | (1<<LED2)
    sts DDRB, r16

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