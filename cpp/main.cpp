#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

class UART {
public:
    void init(uint16_t ubrr) {
        UBRR0H = (ubrr >> 8);
        UBRR0L = ubrr;
        UCSR0B = (1 << TXEN0);
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    }

    void send_char(char c) {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = c;
    }

    void send_string(const char* str) {
        while (*str) {
            send_char(*str++);
        }
    }
};

int main() {
    UART uart;
    uart.init(F_CPU / 16 / 9600 - 1);

    const char* message = "Hello from ATmega!";
    
    while (true) {
        uart.send_string(message);
        _delay_ms(1000);
    }
}