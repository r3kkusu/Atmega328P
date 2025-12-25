; =========================================
; File: lorem_uart.asm
; ATmega328P
; PURE ASM: FLASH → SRAM → UART
; No .S, no pm(), no CPP
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

.equ TXEN0,   3
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
; UART Init
; -------------------------
uart_init:
    ldi r16, lo8(UBRRVAL)
    sts UBRR0L, r16
    ldi r16, hi8(UBRRVAL)
    sts UBRR0H, r16

    ldi r16, (1 << TXEN0)
    sts UCSR0B, r16

    ldi r16, (1 << UCSZ01) | (1 << UCSZ00)
    sts UCSR0C, r16
    ret

; -------------------------
; UART Send Char
; r16 = character
; -------------------------
uart_putchar:
wait_udre:
    lds r17, UCSR0A
    sbrs r17, UDRE0
    rjmp wait_udre
    sts UDR0, r16
    ret

; -------------------------
; UART Print String (SRAM)
; X points to string
; -------------------------
uart_print:
    ld r16, X+
    tst r16
    breq uart_done
    rcall uart_putchar
    rjmp uart_print
uart_done:
    ret

; -------------------------
; Copy FLASH → SRAM
; Z = FLASH BYTE address
; X = SRAM destination
; -------------------------
copy_flash_to_sram:
copy_loop:
    lpm r16, Z+
    st  X+, r16
    tst r16
    brne copy_loop
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

    ; ---- LOAD FLASH ADDRESS (WORD → BYTE) ----
    ldi ZL, lo8(lorem)
    ldi ZH, hi8(lorem)

    ; SRAM destination
    ldi XL, lo8(lorem_buf)
    ldi XH, hi8(lorem_buf)

    rcall copy_flash_to_sram

loop:
    ldi XL, lo8(lorem_buf)
    ldi XH, hi8(lorem_buf)
    rcall uart_print
    rjmp loop

; -------------------------
; FLASH string (single NUL)
; -------------------------
.p2align 1
lorem:
    .ascii "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
    .ascii "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
    .ascii "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris.\n"
    .ascii "Duis aute irure dolor in reprehenderit in voluptate velit esse.\n"
    .ascii "Excepteur sint occaecat cupidatat non proident.\n"
    .asciz "Sunt in culpa qui officia deserunt mollit anim id est laborum.\n"

; -------------------------
; SRAM buffer
; -------------------------
.section .bss
lorem_buf:
    .skip 512
    