#include <avr/io.h>
#include <stdint.h>

#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VAL ((F_CPU / (16UL * BAUD)) - 1)

/* -------------------------
 * UART low-level functions
 * ------------------------- */

static void uart_init(void)
{
    UBRR0H = (uint8_t)(UBRR_VAL >> 8);
    UBRR0L = (uint8_t)(UBRR_VAL);

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);          // Enable RX & TX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);        // 8N1
}

static uint8_t uart_getchar(void)
{
    while (!(UCSR0A & (1 << RXC0))) {
        ; // wait
    }
    return UDR0;
}

static void uart_putchar(uint8_t c)
{
    while (!(UCSR0A & (1 << UDRE0))) {
        ; // wait
    }
    UDR0 = c;
}

static void uart_print(const char *s)
{
    while (*s) {
        uart_putchar(*s++);
    }
}

/* -------------------------
 * Main
 * ------------------------- */

int main(void)
{
    uart_init();

    while (1) {
        uint8_t cmd = uart_getchar();

        switch (cmd) {
            case 1:
                uart_print("FORWARD\r\n");
                break;
            case 2:
                uart_print("BACKWARD\r\n");
                break;
            case 3:
                uart_print("LEFT\r\n");
                break;
            case 4:
                uart_print("RIGHT\r\n");
                break;
            default:
                break;
        }
    }
}