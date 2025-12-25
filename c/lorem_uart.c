#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#define F_CPU 16000000UL
#define UBRRVAL 103  // 9600 baud @ 16 MHz

/* -------------------------
 * FLASH string
 * ------------------------- */
const char lorem_flash[] PROGMEM =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
    "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
    "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris.\n"
    "Duis aute irure dolor in reprehenderit in voluptate velit esse.\n"
    "Excepteur sint occaecat cupidatat non proident.\n"
    "Sunt in culpa qui officia deserunt mollit anim id est laborum.\n";

/* -------------------------
 * SRAM buffer
 * ------------------------- */
char lorem_buf[512];

/* -------------------------
 * UART Init
 * ------------------------- */
void uart_init(void) {
    UBRR0H = (uint8_t)(UBRRVAL >> 8);
    UBRR0L = (uint8_t)(UBRRVAL & 0xFF);

    UCSR0B = (1 << TXEN0);                         // Enable TX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);        // 8N1
}

/* -------------------------
 * UART Send Char
 * ------------------------- */
void uart_putchar(char c) {
    while (!(UCSR0A & (1 << UDRE0))) {
        ; // wait
    }
    UDR0 = c;
}

/* -------------------------
 * UART Print String (SRAM)
 * ------------------------- */
void uart_print(const char *s) {
    while (*s) {
        uart_putchar(*s++);
    }
}

/* -------------------------
 * Copy FLASH → SRAM
 * ------------------------- */
void copy_flash_to_sram(void) {
    uint16_t i = 0;
    char c;

    do {
        c = pgm_read_byte(&lorem_flash[i]);
        lorem_buf[i] = c;
        i++;
    } while (c != '\0');
}

/* -------------------------
 * Main
 * ------------------------- */
int main(void) {
    uart_init();

    copy_flash_to_sram();

    while (1) {
        uart_print(lorem_buf);
    }
}