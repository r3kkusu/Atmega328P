#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/* =========================================================
   I2C / TWI (SSD1306)
   ========================================================= */
#define OLED_ADDR 0x3C

static void i2c_init(void) {
    // ~100kHz @ 16MHz
    TWBR = 72;
    TWSR = 0x00;
}

static void i2c_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

static void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

static void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

/* =========================================================
   SSD1306 OLED
   ========================================================= */
static void oled_cmd(uint8_t cmd) {
    i2c_start();
    i2c_write(OLED_ADDR << 1);
    i2c_write(0x00);   // command
    i2c_write(cmd);
    i2c_stop();
}

static void oled_data(uint8_t data) {
    i2c_start();
    i2c_write(OLED_ADDR << 1);
    i2c_write(0x40);   // data
    i2c_write(data);
    i2c_stop();
}

static void oled_init(void) {
    oled_cmd(0xAE);        // display off
    oled_cmd(0xD5); oled_cmd(0x80);
    oled_cmd(0xA8); oled_cmd(0x3F);
    oled_cmd(0xD3); oled_cmd(0x00);
    oled_cmd(0x40);
    oled_cmd(0x8D); oled_cmd(0x14);
    oled_cmd(0x20); oled_cmd(0x00);
    oled_cmd(0xA1);
    oled_cmd(0xC8);
    oled_cmd(0xDA); oled_cmd(0x12);
    oled_cmd(0x81); oled_cmd(0x7F);
    oled_cmd(0xA4);
    oled_cmd(0xA6);
    oled_cmd(0xAF);        // display on

    // Page 0, column 0
    oled_cmd(0xB0);
    oled_cmd(0x00);
    oled_cmd(0x10);
}

/* =========================================================
   5x7 FONT (digits only)
   ========================================================= */
static const uint8_t font5x7_digits[10][5] = {
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}  // 9
};

static void oled_digit(char c) {
    if (c < '0' || c > '9') return;
    uint8_t idx = c - '0';
    for (uint8_t i = 0; i < 5; i++)
        oled_data(font5x7_digits[idx][i]);
    oled_data(0x00); // spacing
}

/* =========================================================
   HC-SR04 (PD2 TRIG, PD3 ECHO)
   ========================================================= */
#define TRIG_PIN PD2
#define ECHO_PIN PD3

static void hcsr04_init(void) {
    DDRD |= (1 << TRIG_PIN);
    DDRD &= ~(1 << ECHO_PIN);
    PORTD &= ~(1 << TRIG_PIN);
}

static void hcsr04_trigger(void) {
    PORTD |= (1 << TRIG_PIN);
    _delay_us(12);
    PORTD &= ~(1 << TRIG_PIN);
}

static uint16_t hcsr04_read_ticks(void) {
    // wait for ECHO high
    while (!(PIND & (1 << ECHO_PIN)));

    TCNT1 = 0;
    TCCR1A = 0;
    TCCR1B = (1 << CS11);   // prescaler 8

    // wait for ECHO low
    while (PIND & (1 << ECHO_PIN));

    TCCR1B = 0;
    return TCNT1;
}

static uint16_t hcsr04_distance_cm(void) {
    hcsr04_trigger();
    uint16_t ticks = hcsr04_read_ticks();
    return ticks / 116U;    // same approximation as ASM
}

/* =========================================================
   MAIN
   ========================================================= */
int main(void) {
    i2c_init();
    oled_init();
    hcsr04_init();

    while (1) {
        uint16_t cm = hcsr04_distance_cm();

        // reset cursor
        oled_cmd(0xB0);
        oled_cmd(0x00);
        oled_cmd(0x10);

        // print value (0–999)
        if (cm >= 100) oled_digit('0' + (cm / 100));
        if (cm >= 10)  oled_digit('0' + ((cm / 10) % 10));
        oled_digit('0' + (cm % 10));

        _delay_ms(200);
    }
}