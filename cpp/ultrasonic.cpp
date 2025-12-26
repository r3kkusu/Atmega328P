#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

class UART {
public:
    void init(uint32_t baud) {
        uint16_t ubrr = (uint16_t)((F_CPU / (16UL * baud)) - 1);
        UBRR0H = (uint8_t)(ubrr >> 8);
        UBRR0L = (uint8_t)(ubrr & 0xFF);

        UCSR0B = (1 << TXEN0);
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    }

    void sendChar(char c) {
        while (!(UCSR0A & (1 << UDRE0))) { }
        UDR0 = (uint8_t)c;
    }

    void sendString(const char* s) {
        while (*s) sendChar(*s++);
    }

    void sendU16(uint16_t v) {
        if (v == 0) { sendChar('0'); return; }
        char buf[6];
        uint8_t i = 0;
        while (v > 0 && i < sizeof(buf)) {
            buf[i++] = (char)('0' + (v % 10));
            v /= 10;
        }
        while (i > 0) sendChar(buf[--i]);
    }
};

class HCSR04 {
public:
    // TRIG: PD2, ECHO: PD3
    void initPins() {
        DDRD |= (1 << PD2);     // TRIG output
        DDRD &= ~(1 << PD3);    // ECHO input
        PORTD &= ~(1 << PD2);   // TRIG low
    }

    uint16_t distanceCm() {
        trigger();

        uint16_t ticks = readEchoTicks();
        if (ticks == 0) return 0;

        // 0.5us per tick => cm = ticks / 116
        return (uint16_t)(ticks / 116U);
    }

private:
    void trigger() {
        PORTD |= (1 << PD2);
        _delay_us(12);
        PORTD &= ~(1 << PD2);
    }

    uint16_t readEchoTicks() {
        // Wait for ECHO high (timeout)
        uint32_t guard = 60000UL;
        while (!(PIND & (1 << PD3))) {
            if (--guard == 0) return 0;
        }

        // Timer1 start prescaler 8
        TCCR1A = 0;
        TCCR1B = 0;
        TCNT1 = 0;
        TCCR1B = (1 << CS11);

        // Wait for ECHO low (timeout)
        guard = 120000UL;
        while (PIND & (1 << PD3)) {
            if (--guard == 0) { TCCR1B = 0; return 0; }
        }

        TCCR1B = 0;
        return (uint16_t)TCNT1;
    }
};

int main() {
    UART uart;
    HCSR04 sensor;

    uart.init(9600);
    sensor.initPins();

    while (true) {
        uint16_t cm = sensor.distanceCm();

        uart.sendString("Distance: ");
        uart.sendU16(cm);
        uart.sendString(" cm\r\n");

        _delay_ms(200);
    }
}