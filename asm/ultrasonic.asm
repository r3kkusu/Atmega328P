; =========================================================
; ATmega328P – HC-SR04 Distance (UART, cm output)
; Clock: 16 MHz external crystal
; =========================================================

; ---------------------------------------------------------
; Register definitions
; ---------------------------------------------------------
.equ DDRD,   0x2A
.equ PORTD,  0x2B
.equ PIND,   0x29

.equ TCCR1A, 0x80
.equ TCCR1B, 0x81
.equ TCNT1L, 0x84
.equ TCNT1H, 0x85
.equ TIFR1,  0x36

.equ UBRR0L, 0xC4
.equ UBRR0H, 0xC5
.equ UCSR0B, 0xC1
.equ UCSR0C, 0xC2
.equ UCSR0A, 0xC0
.equ UDR0,   0xC6

.equ TXEN0,  3
.equ UDRE0,  5
.equ UCSZ00, 1
.equ UCSZ01, 2

.equ CS11,   1

.equ SPL,    0x3D
.equ SPH,    0x3E
.equ RAMEND, 0x08FF

; ---------------------------------------------------------
; Pins
; ---------------------------------------------------------
.equ TRIG_BIT, 2        ; PD2
.equ ECHO_BIT, 3        ; PD3

.equ TRIG_MASK, (1<<2)
.equ ECHO_MASK, (1<<3)

; ---------------------------------------------------------
.org 0x0000
rjmp main

; ---------------------------------------------------------
; UART INIT (9600 baud @ 16 MHz)
; ---------------------------------------------------------
uart_init:
    ldi r16, 103              ; 9600 baud
    sts UBRR0L, r16
    clr r16
    sts UBRR0H, r16

    ldi r16, (1<<TXEN0)
    sts UCSR0B, r16

    ldi r16, (1<<UCSZ01)|(1<<UCSZ00)
    sts UCSR0C, r16
    ret

uart_send:
uart_wait:
    lds r18, UCSR0A
    sbrs r18, UDRE0
    rjmp uart_wait
    sts UDR0, r17
    ret

; ---------------------------------------------------------
; Send decimal number in r20 (0–255)
; ---------------------------------------------------------
uart_send_dec:
    ldi r18, 0          ; hundreds
hundreds:
    cpi r20, 100
    brlo tens
    subi r20, 100
    inc r18
    rjmp hundreds

tens:
    ldi r19, 0
tens_loop:
    cpi r20, 10
    brlo ones
    subi r20, 10
    inc r19
    rjmp tens_loop

ones:
    ; send hundreds
    cpi r18, 0
    breq skip_h
    mov r17, r18
    ori r17, '0'
    rcall uart_send
skip_h:

    ; send tens
    cpi r19, 0
    breq skip_t
    mov r17, r19
    ori r17, '0'
    rcall uart_send
skip_t:

    ; send ones
    mov r17, r20
    ori r17, '0'
    rcall uart_send

    ; newline CR LF
    ldi r17, 13
    rcall uart_send
    ldi r17, 10
    rcall uart_send
    ret

; ---------------------------------------------------------
; Trigger HC-SR04 (10 us pulse)
; FIX: no sbi/cbi on PORTD (extended I/O)
; ---------------------------------------------------------
trigger:
    ; TRIG HIGH
    lds r16, PORTD
    ori r16, TRIG_MASK
    sts PORTD, r16

    ldi r18, 160        ; ~10 us @ 16 MHz
t1:
    dec r18
    brne t1

    ; TRIG LOW
    lds r16, PORTD
    andi r16, ~TRIG_MASK
    sts PORTD, r16
    ret

; ---------------------------------------------------------
; Measure echo pulse width
; Result: r25:r24 = timer ticks (0.5 us per tick)
; FIX: no sbis/sbic on PIND (extended I/O)
; ---------------------------------------------------------
measure_echo:
wait_high:
    lds r16, PIND
    sbrs r16, ECHO_BIT
    rjmp wait_high

    ; clear timer
    clr r16
    sts TCNT1H, r16
    sts TCNT1L, r16

    ; start timer (prescaler = 8)
    ldi r16, (1<<CS11)
    sts TCCR1B, r16

wait_low:
    lds r16, PIND
    sbrc r16, ECHO_BIT
    rjmp wait_low

    ; stop timer
    clr r16
    sts TCCR1B, r16

    ; read timer
    lds r24, TCNT1L
    lds r25, TCNT1H
    ret

; ---------------------------------------------------------
; MAIN
; ---------------------------------------------------------
.global main
main:
    ; stack init
    ldi r16, lo8(RAMEND)
    sts SPL, r16
    ldi r16, hi8(RAMEND)
    sts SPH, r16

    ; PD2 = output (TRIG), PD3 = input (ECHO)
    lds r16, DDRD
    ori r16, TRIG_MASK
    andi r16, ~ECHO_MASK
    sts DDRD, r16

    rcall uart_init

loop:
    rcall trigger
    rcall measure_echo

    ; -------------------------------------
    ; Convert ticks -> cm (rough demo)
    ; ticks * 0.5 / 58 ~= ticks / 116
    ; -------------------------------------
    mov r20, r25      ; coarse distance estimate (demo)

    rcall uart_send_dec
    rjmp loop
    