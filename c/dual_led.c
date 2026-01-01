#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define LED1_PIN (1 << PB0)
#define LED2_PIN (1 << PB1)

int main(void)
{
    // Set PB0 and PB1 as outputs
    DDRB |= LED1_PIN | LED2_PIN;

    // Ensure LEDs start OFF
    PORTB &= ~(LED1_PIN | LED2_PIN);

    while (1)
    {
        // LED1 ON, LED2 OFF
        PORTB |= LED1_PIN;
        PORTB &= ~LED2_PIN;
        _delay_ms(1000);

        // LED1 OFF, LED2 ON
        PORTB &= ~LED1_PIN;
        PORTB |= LED2_PIN;
        _delay_ms(1000);
    }
}