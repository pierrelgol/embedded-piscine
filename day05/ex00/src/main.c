#include "hal.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// uart initialization
void uart_init(void) {
        // set baud rate
        UBRR0H = 0;
        UBRR0L = 8;
        // enable transmitter
        UCSR0B = (1 << TXEN0);
        // set frame format: 8 data bits, 1 stop bit
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// uart putchar function (if needed)
void uart_putchar(char c) {
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = c;
}

// print string and append CRLF
void uart_println(char *str) {
        if (!str) {
                return;
        }
        while (*str) {
                loop_until_bit_is_set(UCSR0A, UDRE0);
                UDR0 = *str++;
        }
        // append carriage return and line feed
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = '\r';
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = '\n';
}

// adc initialization: use AVCC as reference, left-adjust for 8-bit resolution, select ADC0 (PC0)
void adc_init() {
        ADMUX = (1 << REFS0) | (1 << ADLAR);
        // enable ADC and set prescaler to 128 for a 16MHz clock (16MHz/128 = 125kHz)
        ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

// read 8-bit ADC result from ADCH
u8 adc_read() {
        ADCSRA |= (1 << ADSC);                 // start conversion
        loop_until_bit_is_clear(ADCSRA, ADSC); // wait until conversion is complete
        return ADCH;                           // return left-adjusted 8-bit result
}

// format a byte as two hex characters (null-terminated).
// the caller provides a pointer to a buffer that must have room for 3 bytes.
void fmt_hex(u8 *const buffer, u8 byte) {
        const char hex_chars[] = "0123456789ABCDEF";
        buffer[0]              = hex_chars[byte >> 4];
        buffer[1]              = hex_chars[byte & 0x0F];
        buffer[2]              = '\0';
}

int main(void) {
        uart_init();
        adc_init();

        // create a buffer for the string "0xXX" plus the null terminator.
        u8 buffer[5] = {'0', 'x', 0, 0, '\0'};

        loop {
                u8 adc_value = adc_read();
                // format ADC value into two hex digits stored at buffer[2] and buffer[3]
                fmt_hex(&buffer[2], adc_value);
                uart_println((char *)buffer);
                _delay_ms(20);
        }
}
