#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define LED1 PB0
#define LED2 PB1

int main(void) {
    // Set PB0 and PB1 as outputs
    DDRB |= (1 << LED1) | (1 << LED2);

    while (1) {
        // LED1 ON, LED2 OFF
        PORTB |= (1 << LED1);
        PORTB &= ~(1 << LED2);
        _delay_ms(1000);

        // LED1 OFF, LED2 ON
        PORTB &= ~(1 << LED1);
        PORTB |= (1 << LED2);
        _delay_ms(1000);
    }
}