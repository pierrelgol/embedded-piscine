#include "hal.h"
#include <avr/delay.h>
#include <avr/interrupt.h>

// (1) = https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf


volatile uint8_t uart_data_ready = 0; // ISR sets this when data is ready
volatile uint8_t received_char   = 0;

void             uart_tx(char c) {
        while (!(UCSR0A & (1 << UDRE0)))
                ;
        UDR0 = c;
}

void uart_rx(void) {
        while (!uart_data_ready)
                ;               // Wait until ISR signals data is ready
        received_char   = UDR0; // Read data (clears RXC0 flag automatically)
        uart_data_ready = 0;    // Clear data-ready flag
}

ISR(USART_RX_vect) {
        uart_data_ready = 1; // indicate data is ready
}

int main(void) {
        // UART init: 115200 baud rate at 16MHz
        UBRR0H = 0;
        UBRR0L = 8;

        // Frame: 8-bit, no parity, 1 stop bit
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

        // Enable TX, RX, and RX interrupt
        UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);

        // Configure pins: RX input, TX output
        DDRD &= ~(1 << PD0);
        PORTD &= ~(1 << PD0);
        DDRD |= (1 << PD1);
        PORTD &= ~(1 << PD1);

        sei(); // enable interrupts

        while (1) {
                // read when ready
                uart_rx();
                // write when ready
                uart_tx(received_char);
        }
}
