#include "hal.h"

int main(void) {

        // set the OC1A and D2 as an ouput.
        DDRB |= (1 << PB1);

        // This configure timer1 in fast pwm mode (ICR1 is top in this case).
        // Clear OC1A on compare match, set on BOTTOM
        TCCR1A = (1 << COM1A1) | (1 << WGM11);
        // Fast PWM, prescaler = 256
        TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12);
        // set top value for 1hz (16Mhz) / (256 * 1) = 62499
        ICR1 = 62499;
        // 10 % of OCR1A is 6249 this mean that the duty cycle is 10 % (0.1s)
        // over 0.9s
        OCR1A = 6249;

        loop {
        }
}
