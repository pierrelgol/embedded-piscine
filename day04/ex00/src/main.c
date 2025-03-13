#include "hal.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// global variable declarations for hardware pin objects.
// 'd1' represents the led connected to port b, pin 0 (pb0).
// 'sw1' represents the push-button connected to port d, pin 2 (pd2).
GpioPin d1;
GpioPin sw1;

// interrupt service routine for external interrupt 0 (int0).
// this routine executes automatically when int0 is triggered.
// the interrupt is configured to detect a falling edge (high-to-low transition) on pd2.
// when triggered, it toggles the state of the led associated with 'd1'.
ISR(INT0_vect) {
        gpio_toggle(&d1); // toggle led: if it's on, turn it off; if it's off, turn it on.
}

int main(void) {
        // initialize the led hardware:
        // link the 'd1' object to port b, pin 0 (pb0).
        // configure the pin as an output to drive the led.
        gpio_init(&d1, GPIO_PORTB, PB0);
        gpio_set_output(&d1);

        // initialize the push-button hardware:
        //  link the 'sw1' object to port d, pin 2 (pd2).
        //  configure the pin as an input and enable its internal pull-up resistor.
        //   this ensures the input is high by default and goes low when the button is pressed.
        gpio_init(&sw1, GPIO_PORTD, PD2);
        gpio_set(&sw1); // enable internal pull-up resistor for reliable button state reading.

        // configure external interrupt int0 to detect a falling edge:
        //  modify the external interrupt control register a (eicra):
        //  set isc01 to 1 to indicate edge triggering.
        //  clear isc00 to 0 to specifically select falling edge detection.
        // when pd2 transitions from high to low (button press), this configuration triggers int0.
        EICRA |= (1 << ISC01);  // set isc01 bit.
        EICRA &= ~(1 << ISC00); // clear isc00 bit.

        // enable the external interrupt int0:
        //  update the external interrupt mask register (eimsk) to allow int0 events.
        EIMSK |= (1 << INT0);

        // enable global interrupts:
        //  this instruction allows the cpu to respond to enabled interrupt sources.
        __asm__ __volatile__("sei" : : : "memory");

        // enter an infinite loop:
        //  the main loop remains empty because the led toggling is handled by the isr.
        //  the microcontroller stays idle while waiting for button presses that trigger interrupts.
        loop {
        }
}
