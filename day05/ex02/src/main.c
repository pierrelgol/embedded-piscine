#include "hal.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// uart initialization
void uart_init() {
        UBRR0H = 0;
        UBRR0L = 8;
        UCSR0B = (1 << TXEN0);
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// uart putchar function
void uart_putchar(u8 c) {
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = c;
}

// print a string without newline
void uart_print(const u8 *str) {
        if (!str) return;
        while (*str) {
                uart_putchar(*str++);
        }
}

// print a string with CR+LF at the end
void uart_println(const u8 *str) {
        uart_print(str);
        uart_putchar('\r');
        uart_putchar('\n');
}

// adc initialization in 10-bit mode
void adc_init() {
        ADMUX  = (1 << REFS0);                                             // avcc reference, right-adjust (ADLAR = 0 for 10-bit mode)
        ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // enable ADC, prescaler 128
}

// read 10-bit ADC value from the given channel (0-7)
uint16_t adc_read_channel(u8 channel) {
        ADMUX = (ADMUX & 0xF8) | (channel & 0x07); // select channel (preserving REFS0)
        ADCSRA |= (1 << ADSC);                     // start conversion
        loop_until_bit_is_clear(ADCSRA, ADSC);     // wait for conversion complete
        return ADC;                                // read 10-bit result
}

// convert a 10-bit integer to a decimal string
void fmt_dec(u8 *buffer, u16 value) {
        i32 i = 0;
        u8  temp[6]; // max value is 1023, so we need up to 5 chars + null terminator
        do {
                temp[i++] = (value % 10) + '0';
                value /= 10;
        } while (value > 0);

        // reverse the string
        for (u8 j = 0; j < i; j++) {
                buffer[j] = temp[i - j - 1];
        }
        buffer[i] = '\0'; // null-terminate
}

int main(void) {
        uart_init();
        adc_init();

        u8 buf_rv1[6];
        u8 buf_ldr[6];
        u8 buf_ntc[6];

        while (1) {
                uint16_t val_rv1 = adc_read_channel(0);
                uint16_t val_ldr = adc_read_channel(1);
                uint16_t val_ntc = adc_read_channel(2);

                fmt_dec(buf_rv1, val_rv1);
                fmt_dec(buf_ldr, val_ldr);
                fmt_dec(buf_ntc, val_ntc);

                uart_print(buf_rv1);
                uart_putchar(' ');
                uart_print(buf_ldr);
                uart_putchar(' ');
                uart_println(buf_ntc);

                _delay_ms(20);
        }

        return 0;
}
