#include "hal.h"
#include <avr/delay.h>
#include <avr/interrupt.h>

const Color sequence[7] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE};

static void init_timer_pwm(void) {
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



static void display_color(Color color) {
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


int main(void) {

        init_rgb();
        init_timer_pwm();

        loop {
                for (uint8_t i = 0; i < (sizeof(sequence) / sizeof(sequence[0])); i++) {
                        display_color(sequence[i]);
                        _delay_ms(1000);
                }
        }
}
