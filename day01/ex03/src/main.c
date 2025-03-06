#include "hal.h"
#include <avr/delay.h>

volatile uint8_t      duty_cycle       = 1;
static const uint16_t DUTY_CYCLE_TENTH = 6249;

#define update_duty_cycle() OCR1A = (DUTY_CYCLE_TENTH * duty_cycle);

int main(void) {

        // we open the PD2 and PD3 as input (sw1 and sw2).
        DDRD &= ~((1 << PD2) | (1 << PD4));

        // we enable the pull-up resistor.
        PORTD |= (1 << PD2) | (1 << PD4);

        // set the OC1A and D2 as an ouput.
        DDRB |= (1 << PB1);

        // This configure timer1 in fast pwm mode (ICR1 is top in this case).
        // Clear OC1A on compare match, set on BOTTOM
        // it's important to note that OC1A is buffered, but it's
        // still important to set only on bottom as to keep a constant
        // duty_cycle.
        TCCR1A = (1 << COM1A1) | (1 << WGM11);
        // Fast PWM, prescaler = 256
        TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12);
        // set top value for 1hz (16Mhz) / (256 * 1) = 62499
        ICR1 = 62499;
        // 10 % of OCR1A is 6249 this mean that the duty cycle is 10 % (0.1s)
        // over 0.9s
        OCR1A = (DUTY_CYCLE_TENTH * duty_cycle);


        loop {

                // we increase the duty cycle
                if (!(PIND & (1 << PD2))) {
                        if (duty_cycle < 10) {
                                duty_cycle += 1;
                                update_duty_cycle();
                                _delay_ms(200);
                        }
                }

                // we decrease the duty cycle
                if (!(PIND & (1 << PD4))) {
                        if (duty_cycle > 1) {
                                duty_cycle -= 1;
                                update_duty_cycle();
                                _delay_ms(200);
                        }
                }
        }
}
