#include <avr/io.h>
#include <stdint.h>

#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VAL ((F_CPU / (16UL * BAUD)) - 1)

class UART {
public:
    static void init() {
        UBRR0H = (uint8_t)(UBRR_VAL >> 8);
        UBRR0L = (uint8_t)(UBRR_VAL);
        UCSR0B = (1 << RXEN0) | (1 << TXEN0);
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    }

    static uint8_t read() {
        while (!(UCSR0A & (1 << RXC0))) {
            ;
        }
        return UDR0;
    }

    static void write(uint8_t c) {
        while (!(UCSR0A & (1 << UDRE0))) {
            ;
        }
        UDR0 = c;
    }

    static void print(const char *s) {
        while (*s) {
            write(*s++);
        }
    }
};

int main()
{
    UART::init();

    while (1) {
        uint8_t cmd = UART::read();

        switch (cmd) {
            case 1:
                UART::print("FORWARD\r\n");
                break;
            case 2:
                UART::print("BACKWARD\r\n");
                break;
            case 3:
                UART::print("LEFT\r\n");
                break;
            case 4:
                UART::print("RIGHT\r\n");
                break;
            default:
                break;
        }
    }
}