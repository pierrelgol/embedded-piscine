#include "hal.h"
#include <avr/delay.h>
#include <avr/interrupt.h>

// (1) = https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

void uart_printstr(const char *str) {
        if (!str) {
                return;
        }
        while (*str) {
                // Wait for the UART Data Register to be ready
                for (; !(UCSR0A & (1 << UDRE0)););
                UDR0 = *str++;
        }
}

volatile uint8_t interrupt_counter = 0;


ISR(TIMER1_COMPA_vect) {
        if (interrupt_counter == 1) {
                // this ensure that we only call the function
                // every 2 interrupt aka every 2s.
                uart_printstr("Hello World!\n");
                interrupt_counter = 0;
        } else {
                interrupt_counter = 1;
        }
}


int main(void) {

        // set the baud rate to 115200
        UBRR0H = 0;
        UBRR0L = 8;

        // configure the frame to be 8 bits, no parity, 1 stop bit (8N1)
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

        // enable UART Tx only since we don't need to receive anything.
        UCSR0B = (1 << TXEN0);

        // configure the pin pd1 (aka TX) as output.
        DDRD |= (1 << PD1);

        TCCR1B |= (1 << WGM12);  // CTC mode.
        OCR1A = 62499;           // we set OCR1A to 1hz.
        TIMSK1 |= (1 << OCIE1A); // enable the timer1 ctc interrupt
        TCCR1B |= (1 << CS12);   // configure the prescaler to 256

        sei(); // we enable gloabl interrups to enable the ISR macro.

        loop {
        }
}
