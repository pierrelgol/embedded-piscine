#include "hal.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// global variables for pwm control:
// 'duty_percent' holds the current duty cycle percentage for the pwm signal.
// 'step' defines the incremental change applied to 'duty_percent' during each timer interrupt.
volatile uint8_t duty_percent = 0;
volatile int8_t  step         = 2;

// timer0 compare match a interrupt service routine:
// this isr is executed whenever timer0 reaches the value specified in ocr0a (output compare register a).
ISR(TIMER0_COMPA_vect) {

        // increment the duty cycle by the current step value.
        duty_percent += step;

        // check if the duty cycle has reached or exceeded 100%.
        // if so, set it to 100% and reverse the direction of change by setting step to -2.
        if (duty_percent >= 100) {
                duty_percent = 100;
                step         = -2;
        }

        // check if the duty cycle has reached or fallen below 0%.
        // if so, set it to 0% and reverse the direction of change by setting step to 2.
        if (duty_percent <= 0) {
                duty_percent = 0;
                step         = 2;
        }

        // update the pwm duty cycle for timer1:
        // convert the percentage value to an 8-bit scale (0-255) since the pwm resolution is 8-bit.
        // the calculation scales 'duty_percent' from 0-100 to 0-255 and stores it in ocr1a,
        // which controls the pwm output level on the associated pin.
        OCR1A = ((uint16_t)duty_percent * 255) / 100;
}

int main(void) {
        // configure pwm output pin:
        // set pb1 as an output to drive the pwm signal generated by timer1.
        DDRB |= (1 << PB1);

        // configure timer1 for pwm mode:
        // set com1a1 to enable non-inverting pwm on the output compare pin oc1a.
        // set wgm10 for fast pwm mode with 8-bit resolution.
        TCCR1A |= (1 << COM1A1) | (1 << WGM10);
        // set wgm12 for fast pwm mode with ocr1a as top.
        // set cs11 to apply a prescaler (dividing the clock by 8) for timer1.
        TCCR1B |= (1 << WGM12) | (1 << CS11);

        // configure timer0 to generate periodic interrupts:
        // set wgm01 to put timer0 in clear timer on compare match (ctc) mode.
        TCCR0A |= (1 << WGM01);
        // set the output compare value in ocr0a to determine the interrupt frequency.
        OCR0A = 155;
        // enable the timer0 compare match a interrupt by setting ocie0a in timsk0.
        TIMSK0 |= (1 << OCIE0A);
        // configure timer0's clock by setting cs02 and cs00 for an appropriate prescaler,
        //   which controls how fast timer0 counts up to ocr0a.
        TCCR0B |= (1 << CS02) | (1 << CS00);

        // enable global interrupts:
        // this allows the microcontroller to respond to the timer0 interrupt (and any others enabled).
        __asm__ __volatile__("sei" : : : "memory");

        // main loop:
        // the program enters an infinite loop; all pwm updates are handled in the timer0 isr.
        loop {
        }
}
