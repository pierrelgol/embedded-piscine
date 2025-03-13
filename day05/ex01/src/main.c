#include "hal.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>


// uart initialization
void uart_init() {
        // set baud rate
        UBRR0H = 0;
        UBRR0L = 8;
        // enable transmitter
        UCSR0B = (1 << TXEN0);
        // set frame format: 8 data bits, 1 stop bit
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// uart putchar function
void uart_putchar(char c) {
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = c;
}

// print a string without newline
void uart_print(u8 *str) {
        if (!str) return;
        while (*str) {
                uart_putchar(*str++);
        }
}

// print a string with CR+LF at the end
void uart_println(u8 *str) {
        uart_print(str);
        uart_putchar('\r');
        uart_putchar('\n');
}

// adc initialization: use AVCC as reference and left-adjust result for 8-bit mode.
void adc_init() {
        ADMUX = (1 << REFS0) | (1 << ADLAR); // avcc and left adjust
        // enable ADC and set prescaler to 128 (for 16MHz clock -> 125kHz ADC clock)
        ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

// read the ADC value from a given channel (0-7)
// this function sets the channel (preserving REFS0 and ADLAR), starts a conversion,
// waits until complete, and returns the 8-bit result from ADCH.
u8 adc_read_channel(u8 channel) {
        ADMUX = (ADMUX & 0xF8) | (channel & 0x07); // select channel (keep REFS0 and ADLAR)
        ADCSRA |= (1 << ADSC);                     // start conversion
        loop_until_bit_is_clear(ADCSRA, ADSC);     // wait for conversion complete
        return ADCH;
}

// convert a byte into two hexadecimal digits stored in a buffer.
// the buffer should have room for at least three bytes (two hex digits and a null terminator).
void fmt_hex(u8 *const buffer, u8 byte) {
        const char hex_chars[] = "0123456789ABCDEF";
        buffer[0]              = hex_chars[byte >> 4];
        buffer[1]              = hex_chars[byte & 0x0F];
        buffer[2]              = '\0';
}

int main(void) {
        uart_init();
        adc_init();

        // buffers for each sensor reading (format "0x??")
        u8 buf_rv1[5] = {'0', 'x', 0, 0, '\0'};
        u8 buf_ldr[5] = {'0', 'x', 0, 0, '\0'};
        u8 buf_ntc[5] = {'0', 'x', 0, 0, '\0'};

        while (1) {
                // read sensors: RV1 on ADC0, LDR on ADC1, NTC on ADC2
                const u8 val_rv1 = adc_read_channel(0);
                const u8 val_ldr = adc_read_channel(1);
                const u8 val_ntc = adc_read_channel(2);

                // format each reading into hex strings
                fmt_hex(&buf_rv1[2], val_rv1);
                fmt_hex(&buf_ldr[2], val_ldr);
                fmt_hex(&buf_ntc[2], val_ntc);

                uart_print(buf_rv1);
                uart_putchar(' ');
                uart_print(buf_ldr);
                uart_putchar(' ');
                uart_println(buf_ntc);
                _delay_ms(20);
        }

        return 0;
}
