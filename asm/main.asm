; =========================
; Constants
; =========================
.equ F_CPU,     16000000
.equ BAUD,      9600
.equ UBRR_VAL,  (F_CPU/(16*BAUD)-1)

; =========================
; Register addresses (ATmega328P)
; =========================
.equ UCSR0A, 0xC0
.equ UCSR0B, 0xC1
.equ UCSR0C, 0xC2
.equ UBRR0L, 0xC4
.equ UBRR0H, 0xC5
.equ UDR0,   0xC6

.equ TXEN0,  3
.equ UDRE0,  5
.equ UCSZ00, 1
.equ UCSZ01, 2

.equ SPL,    0x3D
.equ SPH,    0x3E
.equ RAMEND, 0x08FF   ; ATmega328P SRAM end

; =========================
; Code section
; =========================
.section .text
.global main

.org 0x0000
    rjmp main

; =========================
; UART init
; =========================
uart_init:
    ldi r16, hi8(UBRR_VAL)
    sts UBRR0H, r16
    ldi r16, lo8(UBRR_VAL)
    sts UBRR0L, r16

    ldi r16, (1<<TXEN0)
    sts UCSR0B, r16

    ldi r16, (1<<UCSZ01)|(1<<UCSZ00)
    sts UCSR0C, r16
    ret

; =========================
; Send one character (r17)
; =========================
uart_send_char:
uart_wait:
    lds r18, UCSR0A
    sbrs r18, UDRE0
    rjmp uart_wait
    sts UDR0, r17
    ret

; =========================
; Send string from FLASH
; =========================
uart_send_string:
    lpm r17, Z+
    tst r17
    breq uart_done
    rcall uart_send_char
    rjmp uart_send_string
uart_done:
    ret

; =========================
; Main
; =========================
main:
    ; Stack init
    ldi r16, lo8(RAMEND)
    sts SPL, r16
    ldi r16, hi8(RAMEND)
    sts SPH, r16

    rcall uart_init

    ldi ZH, hi8(hello_str)
    ldi ZL, lo8(hello_str)
    rcall uart_send_string

loop:
    rjmp loop

; =========================
; String in FLASH
; =========================
.section .progmem
hello_str:
    .asciz "Hello from ATmega!"
    