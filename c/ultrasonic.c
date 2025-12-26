#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// --------------------
// UART (TX only) @ 9600
// --------------------
static void uart_init(void) {
    uint16_t ubrr = (F_CPU / (16UL * 9600UL)) - 1;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr & 0xFF);

    UCSR0B = (1 << TXEN0); // TX enable
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
}

static void uart_send_char(char c) {
    while (!(UCSR0A & (1 << UDRE0))) { }
    UDR0 = (uint8_t)c;
}

static void uart_send_string(const char *s) {
    while (*s) uart_send_char(*s++);
}

static void uart_send_u16(uint16_t v) {
    // Print unsigned int as decimal (no stdio)
    char buf[6];
    uint8_t i = 0;

    if (v == 0) {
        uart_send_char('0');
        return;
    }

    while (v > 0 && i < sizeof(buf)) {
        buf[i++] = (char)('0' + (v % 10));
        v /= 10;
    }
    while (i > 0) uart_send_char(buf[--i]);
}

// --------------------
// HC-SR04 (PD2 TRIG, PD3 ECHO)
// --------------------
#define TRIG_DDR   DDRD
#define TRIG_PORT  PORTD
#define TRIG_PIN   PD2

#define ECHO_DDR   DDRD
#define ECHO_PINR  PIND
#define ECHO_PIN   PD3

static void hcsr04_init_pins(void) {
    // TRIG as output, ECHO as input
    TRIG_DDR |= (1 << TRIG_PIN);
    ECHO_DDR &= ~(1 << ECHO_PIN);
    TRIG_PORT &= ~(1 << TRIG_PIN);
}

static void hcsr04_trigger(void) {
    // TRIG high for >=10us
    TRIG_PORT |= (1 << TRIG_PIN);
    _delay_us(12);
    TRIG_PORT &= ~(1 << TRIG_PIN);
}

static uint16_t hcsr04_read_echo_ticks_timeout(void) {
    // Measure ECHO high pulse width using Timer1, prescaler=8 (0.5us per tick)
    // Returns ticks; 0 on timeout.

    // Wait for ECHO to go HIGH with timeout
    uint32_t guard = 60000UL;
    while (!(ECHO_PINR & (1 << ECHO_PIN))) {
        if (--guard == 0) return 0;
    }

    // Reset Timer1
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    // Start Timer1 prescaler 8
    TCCR1B = (1 << CS11);

    // Wait for ECHO to go LOW with timeout
    guard = 120000UL;
    while (ECHO_PINR & (1 << ECHO_PIN)) {
        if (--guard == 0) {
            TCCR1B = 0; // stop
            return 0;
        }
    }

    // Stop timer and read ticks
    TCCR1B = 0;
    return (uint16_t)TCNT1;
}

static uint16_t hcsr04_get_distance_cm(void) {
    hcsr04_trigger();
    uint16_t ticks = hcsr04_read_echo_ticks_timeout();
    if (ticks == 0) return 0;

    // Timer tick = 0.5us (16MHz / 8)
    // distance_cm = microseconds / 58
    // microseconds = ticks * 0.5
    // => distance_cm = ticks / 116
    return (uint16_t)(ticks / 116U);
}

int main(void) {
    uart_init();
    hcsr04_init_pins();

    while (1) {
        uint16_t cm = hcsr04_get_distance_cm();

        uart_send_string("Distance: ");
        uart_send_u16(cm);
        uart_send_string(" cm\r\n");

        _delay_ms(200);
    }
}