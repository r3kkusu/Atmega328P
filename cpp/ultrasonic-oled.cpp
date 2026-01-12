#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/* =========================================================
   I2C / TWI
   ========================================================= */
class I2C {
public:
    void init() {
        TWBR = 72;   // ~100kHz @ 16MHz
        TWSR = 0x00;
    }

    void start() {
        TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
        while (!(TWCR & (1 << TWINT)));
    }

    void write(uint8_t data) {
        TWDR = data;
        TWCR = (1 << TWINT) | (1 << TWEN);
        while (!(TWCR & (1 << TWINT)));
    }

    void stop() {
        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    }
};

/* =========================================================
   SSD1306 OLED
   ========================================================= */
class SSD1306 {
public:
    static constexpr uint8_t ADDR = 0x3C;

    void init(I2C& i2c) {
        sendCmd(i2c, 0xAE);
        sendCmd(i2c, 0xD5); sendCmd(i2c, 0x80);
        sendCmd(i2c, 0xA8); sendCmd(i2c, 0x3F);
        sendCmd(i2c, 0xD3); sendCmd(i2c, 0x00);
        sendCmd(i2c, 0x40);
        sendCmd(i2c, 0x8D); sendCmd(i2c, 0x14);
        sendCmd(i2c, 0x20); sendCmd(i2c, 0x00);
        sendCmd(i2c, 0xA1);
        sendCmd(i2c, 0xC8);
        sendCmd(i2c, 0xDA); sendCmd(i2c, 0x12);
        sendCmd(i2c, 0x81); sendCmd(i2c, 0x7F);
        sendCmd(i2c, 0xA4);
        sendCmd(i2c, 0xA6);
        sendCmd(i2c, 0xAF);

        setCursor(i2c, 0, 0);
    }

    void setCursor(I2C& i2c, uint8_t page, uint8_t col) {
        sendCmd(i2c, 0xB0 | page);
        sendCmd(i2c, col & 0x0F);
        sendCmd(i2c, 0x10 | (col >> 4));
    }

    void printDigit(I2C& i2c, char c) {
        if (c < '0' || c > '9') return;
        uint8_t idx = c - '0';
        for (uint8_t i = 0; i < 5; i++)
            sendData(i2c, font[idx][i]);
        sendData(i2c, 0x00);
    }

private:
    void sendCmd(I2C& i2c, uint8_t cmd) {
        i2c.start();
        i2c.write(ADDR << 1);
        i2c.write(0x00);
        i2c.write(cmd);
        i2c.stop();
    }

    void sendData(I2C& i2c, uint8_t data) {
        i2c.start();
        i2c.write(ADDR << 1);
        i2c.write(0x40);
        i2c.write(data);
        i2c.stop();
    }

    static const uint8_t font[10][5];
};

/* =========================================================
   HC-SR04
   ========================================================= */
class HCSR04 {
public:
    void initPins() {
        DDRD |= (1 << PD2);
        DDRD &= ~(1 << PD3);
        PORTD &= ~(1 << PD2);
    }

    uint16_t distanceCm() {
        trigger();
        uint16_t ticks = readEchoTicks();
        return ticks / 116U;
    }

private:
    void trigger() {
        PORTD |= (1 << PD2);
        _delay_us(12);
        PORTD &= ~(1 << PD2);
    }

    uint16_t readEchoTicks() {
        while (!(PIND & (1 << PD3)));

        TCNT1 = 0;
        TCCR1A = 0;
        TCCR1B = (1 << CS11);

        while (PIND & (1 << PD3));

        TCCR1B = 0;
        return (uint16_t)TCNT1;
    }
};

const uint8_t SSD1306::font[10][5] = {
    {0x3E,0x51,0x49,0x45,0x3E},
    {0x00,0x42,0x7F,0x40,0x00},
    {0x42,0x61,0x51,0x49,0x46},
    {0x21,0x41,0x45,0x4B,0x31},
    {0x18,0x14,0x12,0x7F,0x10},
    {0x27,0x45,0x45,0x45,0x39},
    {0x3C,0x4A,0x49,0x49,0x30},
    {0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36},
    {0x06,0x49,0x49,0x29,0x1E}
};

/* =========================================================
   MAIN
   ========================================================= */
int main() {
    I2C i2c;
    SSD1306 oled;
    HCSR04 sensor;

    i2c.init();
    oled.init(i2c);
    sensor.initPins();

    while (true) {
        uint16_t cm = sensor.distanceCm();

        oled.setCursor(i2c, 0, 0);

        if (cm >= 100) oled.printDigit(i2c, '0' + (cm / 100));
        if (cm >= 10)  oled.printDigit(i2c, '0' + ((cm / 10) % 10));
        oled.printDigit(i2c, '0' + (cm % 10));

        _delay_ms(200);
    }
}