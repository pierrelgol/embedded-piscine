#include "hal.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// define led pins
#define LED_D1    PB0
#define LED_D2    PB1
#define LED_D3    PB2
#define LED_D4    PB4

#define RGB_RED   PD5
#define RGB_GREEN PD6
#define RGB_BLUE  PD3

#define LED_MASK  ((1 << LED_D1) | (1 << LED_D2) | (1 << LED_D3) | (1 << LED_D4))

void adc_init() {
        ADMUX  = (1 << REFS0);
        ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read() {
        ADCSRA |= (1 << ADSC);
        loop_until_bit_is_clear(ADCSRA, ADSC);
        return ADC;
}

void led_init() {
        DDRB |= LED_MASK; // set only led pins as output
}

void led_gauge(uint16_t adc_value) {
        PORTB &= ~LED_MASK;

        if (adc_value >= 256) PORTB |= (1 << LED_D1);
        if (adc_value >= 512) PORTB |= (1 << LED_D2);
        if (adc_value >= 768) PORTB |= (1 << LED_D3);
        if (adc_value >= 1010) PORTB |= (1 << LED_D4);
}

void pwm_init(void) {
        // timer0 configuration for oc0a (green, pd6) and oc0b (red, pd5)
        TCCR0A = (1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0B1);
        TCCR0B = (1 << CS01) | (1 << CS00);

        // timer2 configuration for oc2b (blue, pd3)
        TCCR2A = (1 << WGM20) | (1 << WGM21) | (1 << COM2B1);
        TCCR2B = (1 << CS22);
}

// global color struct
static struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
} color;

void display_color() {
        OCR0B = color.r;
        OCR0A = color.g;
        OCR2B = color.b;
}

void rgb_init() {
        DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
        color.r = r;
        color.g = g;
        color.b = b;
}

void wheel(uint8_t pos) {
        if (pos < 85) {
                set_rgb(255 - pos * 3, 0, pos * 3);
        } else if (pos < 170) {
                pos -= 85;
                set_rgb(0, pos * 3, 255 - pos * 3);
        } else {
                pos -= 170;
                set_rgb(pos * 3, 255 - pos * 3, 0);
        }
}

int main(void) {
        adc_init();
        led_init();
        rgb_init();
        pwm_init();

        while (1) {
                uint16_t adc_value = adc_read();

                led_gauge(adc_value);
                wheel(adc_value >> 1);
                display_color();

                _delay_ms(20);
        }

        return 0;
}
