#include "hal.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// global variables to track state:
// 'value' represents a 4-bit counter value that is modified based on switch inputs.
// it is marked as volatile because it is modified within an isr and used in the main program.
volatile uint8_t value = 0;

// 'prev_sw1' and 'prev_sw2' hold the previous states of two input switches (connected to pd2 and pd4).
// they are used to detect a falling edge (transition from high to low) in the switch signals.
volatile uint8_t prev_sw1;
volatile uint8_t prev_sw2;

// timer0 compare match a interrupt service routine (isr):
// this function is executed every time timer0 reaches the value defined in ocr0a,
// effectively creating a periodic interrupt (approximately every 10ms).
ISR(TIMER0_COMPA_vect) {
        // read the current state of switch sw1 (connected to pd2):
        // the expression 'pind & (1 << pd2)' isolates the bit corresponding to pd2.
        uint8_t curr_sw1 = PIND & (1 << PD2);

        // read the current state of switch sw2 (connected to pd4):
        // the expression 'pind & (1 << pd4)' isolates the bit corresponding to pd4.
        uint8_t curr_sw2 = PIND & (1 << PD4);

        // detect a falling edge on sw1:
        // a falling edge is detected if the previous state was high (non-zero) and the current state is low (zero).
        // on detection, increment the 'value' counter.
        if ((prev_sw1 != 0) && (curr_sw1 == 0)) {
                value++;        // increase the counter by 1.
                _delay_ms(150); // insert a crude debounce delay of 150 milliseconds to avoid multiple triggers.
        }

        // detect a falling edge on sw2:
        // a falling edge is detected if the previous state was high and the current state is low.
        // on detection, decrement the 'value' counter.
        if ((prev_sw2 != 0) && (curr_sw2 == 0)) {
                value--;        // decrease the counter by 1.
                _delay_ms(150); // insert a crude debounce delay of 150 milliseconds for switch stabilization.
        }

        // update the stored previous states for both switches with the current readings.
        // this is essential for accurate edge detection in subsequent isr calls.
        prev_sw1 = curr_sw1;
        prev_sw2 = curr_sw2;

        // prepare the value to be displayed on the leds:
        // the lower three bits of 'value' (bits 0 to 2) will be displayed on leds connected to pb0, pb1, and pb2.
        uint8_t led_val = (value & 0x07);

        // check the most significant bit (bit 3) of 'value':
        // if bit 3 is set, it indicates the value is 8 or higher.
        // in that case, light an additional led connected to pb4.
        if (value & 0x08) {
                led_val |= (1 << PB4); // set the bit corresponding to pb4 in 'led_val'.
        }

        // update the led outputs on portb:
        // first, clear the bits for pb0, pb1, pb2, and pb4 in portb to remove old led values.
        // then, set these bits to the new 'led_val' to reflect the current 'value'.
        PORTB = (PORTB & ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4))) | led_val;

        // insert an additional short delay (10ms) to provide extra stabilization (optional).
        _delay_ms(10);
}

int main(void) {
        // configure led output pins on portb:
        // set pb0, pb1, pb2, and pb4 as outputs because these pins are used to display the counter value.
        DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);
        // initialize the led pins to low (off) by clearing them.
        PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4));

        // configure switch input pins on portd:
        // clear the ddrd bits for pd2 and pd4 to configure these pins as inputs.
        DDRD &= ~((1 << PD2) | (1 << PD4));
        // enable the internal pull-up resistors on pd2 and pd4 by setting the corresponding portd bits.
        // this ensures that the switches read as high (logic 1) when not pressed.
        PORTD |= (1 << PD2) | (1 << PD4);

        // initialize previous switch states by reading the current levels on pd2 and pd4.
        // this ensures proper edge detection when the isr is first executed.
        prev_sw1 = PIND & (1 << PD2);
        prev_sw2 = PIND & (1 << PD4);

        // timer0 configuration for generating periodic interrupts:
        // configure timer0 in clear timer on compare (ctc) mode by setting wgm01 in tccr0a.
        TCCR0A = (1 << WGM01);

        // configure timer0's clock source by setting a prescaler:
        // here, cs02 and cs00 are set (with cs01 cleared) to apply a 1024 prescaler.
        // with a 16 mhz clock, this yields a timer tick frequency of approximately 15,625 hz.
        TCCR0B = (1 << CS02) | (1 << CS00);

        // set the output compare register a (ocr0a) value to 156:
        // this value determines when the timer interrupt will occur.
        // with a tick frequency of ~15,625 hz, the compare match happens roughly every 10ms (156 / 15625 ~ 0.01 sec).
        OCR0A = 156;

        // enable the timer0 compare match a interrupt by setting ocie0a in the timer/counter interrupt mask register (timsk0).
        TIMSK0 |= (1 << OCIE0A);

        // enable global interrupts so that the microcontroller can respond to the timer0 interrupt.
        __asm__ __volatile__("sei" : : : "memory");

        // main loop:
        // the loop remains empty because all the necessary operations (switch detection, debouncing, and led updates)
        // are handled within the timer0 isr.
        loop {
                // no action required here; all tasks are interrupt-driven.
        }
}
