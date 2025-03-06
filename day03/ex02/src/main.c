#include "hal.h"
#include <avr/delay.h>
#include <avr/interrupt.h>

static Color color;

static void  init_timer_pwm(void) {
        // Timer0 configuration for OC0A (green, PD6) and OC0B (red, PD5)
        // Fast PWM, 8-bit mode and non-inverting outputs
        TCCR0A = (1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0B1);
        // Prescaler 64 for Timer0 (CS01 and CS00 set)
        TCCR0B = (1 << CS01) | (1 << CS00);

        // Timer2 configuration for OC2B (blue, PD3)
        // Fast PWM, 8-bit mode and non-inverting output on OC2B
        TCCR2A = (1 << WGM20) | (1 << WGM21) | (1 << COM2B1);
        // Prescaler 64 for Timer2 (for Timer2, CS22 = 1 gives prescaler 64)
        TCCR2B = (1 << CS22);
}



static void display_color() {
        OCR0B = color.r; // Red on PD5 (OC0B)
        OCR0A = color.g; // Green on PD6 (OC0A)
        OCR2B = color.b; // Blue on PD3 (OC2B)
}

void init_rgb() {
        // PD3 = Blue
        // PD5 = Red
        // PD6 = Green
        PORTD |= ((1 << PD3) | (1 << PD5) | (1 << PD6));
        DDRD |= ((1 << PD3) | (1 << PD5) | (1 << PD6));
}
void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
        color.r = r;
        color.g = g;
        color.b = b;
}

void wheel(uint8_t pos) {
        pos = 255 - pos;
        if (pos < 85) {
                set_rgb(255 - pos * 3, 0, pos * 3);
        } else if (pos < 170) {
                pos = pos - 85;
                set_rgb(0, pos * 3, 255 - pos * 3);
        } else {
                pos = pos - 170;
                set_rgb(pos * 3, 255 - pos * 3, 0);
        }
}


int main(void) {

        init_rgb();
        init_timer_pwm();

        loop {
                for (uint8_t i = 0; i < 255; i++) {
                        wheel(i);
                        display_color();
                        _delay_us(16333);
                }
        }
}
