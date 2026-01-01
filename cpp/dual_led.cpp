#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define LED1_PIN (1 << PB0)
#define LED2_PIN (1 << PB1)

class LEDController {
public:
    inline void init() {
        // Set PB0 and PB1 as outputs
        DDRB |= LED1_PIN | LED2_PIN;

        // Ensure LEDs start OFF
        PORTB &= ~(LED1_PIN | LED2_PIN);
    }

    inline void led1_on() {
        PORTB |= LED1_PIN;
        PORTB &= ~LED2_PIN;
    }

    inline void led2_on() {
        PORTB &= ~LED1_PIN;
        PORTB |= LED2_PIN;
    }
};

int main(void)
{
    LEDController leds;
    leds.init();

    while (1) {
        leds.led1_on();
        _delay_ms(1000);

        leds.led2_on();
        _delay_ms(1000);
    }
}