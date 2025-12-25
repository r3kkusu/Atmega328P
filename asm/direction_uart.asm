; =========================================
; File: direction_uart.asm
; ATmega328P
; UART RX command → UART TX text
; PURE ASM
; =========================================

; -------------------------
; Register definitions
; -------------------------
.equ UBRR0H,  0xC5
.equ UBRR0L,  0xC4
.equ UCSR0A,  0xC0
.equ UCSR0B,  0xC1
.equ UCSR0C,  0xC2
.equ UDR0,    0xC6

.equ RXEN0,   4
.equ TXEN0,   3
.equ RXC0,    7
.equ UDRE0,   5
.equ UCSZ00,  1
.equ UCSZ01,  2

.equ SPL,     0x3D
.equ SPH,     0x3E
.equ RAMEND,  0x08FF

.equ UBRRVAL, 103        ; 9600 baud @ 16 MHz

; -------------------------
; Reset vector
; -------------------------
.org 0x0000
rjmp main

; -------------------------
; UART Init (RX + TX)
; -------------------------
uart_init:
    ldi r16, lo8(UBRRVAL)
    sts UBRR0L, r16
    ldi r16, hi8(UBRRVAL)
    sts UBRR0H, r16

    ldi r16, (1<<RXEN0) | (1<<TXEN0)
    sts UCSR0B, r16

    ldi r16, (1<<UCSZ01) | (1<<UCSZ00) ; 8N1
    sts UCSR0C, r16
    ret

; -------------------------
; UART RX (blocking)
; returns byte in r16
; -------------------------
uart_getchar:
wait_rx:
    lds r17, UCSR0A
    sbrs r17, RXC0
    rjmp wait_rx
    lds r16, UDR0
    ret

; -------------------------
; UART TX char (r16)
; -------------------------
uart_putchar:
wait_udre:
    lds r17, UCSR0A
    sbrs r17, UDRE0
    rjmp wait_udre
    sts UDR0, r16
    ret

; -------------------------
; UART print FLASH string
; Z → FLASH
; -------------------------
uart_print_flash:
    lpm r16, Z+
    tst r16
    breq uart_done
    rcall uart_putchar
    rjmp uart_print_flash
uart_done:
    ret

; -------------------------
; Main
; -------------------------
.global main
main:
    ; Stack init
    ldi r16, lo8(RAMEND)
    out SPL, r16
    ldi r16, hi8(RAMEND)
    out SPH, r16

    rcall uart_init

main_loop:
    rcall uart_getchar   ; r16 = command

    cpi r16, 1
    breq cmd_forward

    cpi r16, 2
    breq cmd_backward

    cpi r16, 3
    breq cmd_left

    cpi r16, 4
    breq cmd_right

    rjmp main_loop

cmd_forward:
    ldi ZL, lo8(forward)
    ldi ZH, hi8(forward)
    rcall uart_print_flash
    rjmp main_loop

cmd_backward:
    ldi ZL, lo8(backward)
    ldi ZH, hi8(backward)
    rcall uart_print_flash
    rjmp main_loop

cmd_left:
    ldi ZL, lo8(left)
    ldi ZH, hi8(left)
    rcall uart_print_flash
    rjmp main_loop

cmd_right:
    ldi ZL, lo8(right)
    ldi ZH, hi8(right)
    rcall uart_print_flash
    rjmp main_loop

; -------------------------
; FLASH strings
; -------------------------
.p2align 1
forward:
    .asciz "FORWARD\r\n"
backward:
    .asciz "BACKWARD\r\n"
left:
    .asciz "LEFT\r\n"
right:
    .asciz "RIGHT\r\n"
