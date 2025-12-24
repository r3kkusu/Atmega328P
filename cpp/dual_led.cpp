#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

class LEDController {
public:
    void init() {
        DDRB |= (1 << PB0) | (1 << PB1);
    }

    void led1_on() {
        PORTB |= (1 << PB0);
        PORTB &= ~(1 << PB1);
    }

    void led2_on() {
        PORTB &= ~(1 << PB0);
        PORTB |= (1 << PB1);
    }
};

int main() {
    LEDController leds;
    leds.init();

    while (true) {
        leds.led1_on();
        _delay_ms(1000);

        leds.led2_on();
        _delay_ms(1000);
    }
}