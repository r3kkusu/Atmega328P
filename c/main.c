#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

void uart_init(void) {
    uint16_t ubrr = F_CPU/16/9600-1;
    UBRR0H = (ubrr >> 8);
    UBRR0L = ubrr;
    UCSR0B = (1 << TXEN0); // Enable transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits
}

void uart_send_char(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_send_string(const char *str) {
    while (*str) {
        uart_send_char(*str++);
    }
}

int main(void) {
    uart_init();
    const char *message = "Hello from ATmega!";
    
    while (1) {
        uart_send_string(message);
        _delay_ms(1000);
    }
}