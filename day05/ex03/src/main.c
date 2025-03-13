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

// adc initialization for internal temperature sensor
void adc_init() {
        ADMUX  = (1 << REFS1) | (1 << REFS0) | (1 << MUX3);                // select Internal Temp Sensor, 1.1V reference
        ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // enable ADC, prescaler 128
}

// read 10-bit ADC value from the internal temperature sensor
u16 adc_read_temp() {
        ADCSRA |= (1 << ADSC);                 // start conversion
        loop_until_bit_is_clear(ADCSRA, ADSC); // wait for conversion complete
        return ADC;                            // read 10-bit result
}

// convert ADC value to temperature in tenths of Celsius (e.g., 253 means 25.3°C)
i16 adc_to_celsius(u16 adc_value) {
        return ((adc_value * 100) - 3243) / 122; // integer math approximation
}

// convert integer temperature (e.g., 253) to string "25.3"
void fmt_temp(u8 *buffer, i16 temperature) {
        u8 whole   = temperature / 10;
        u8 decimal = temperature % 10;
        buffer[0]  = '0' + (whole / 10);
        buffer[1]  = '0' + (whole % 10);
        buffer[2]  = '.';
        buffer[3]  = '0' + decimal;
        buffer[4]  = '\0';
}

int main(void) {
        uart_init();
        adc_init();

        u8 buf_temp[6];

        while (1) {
                u16 adc_value   = adc_read_temp();
                i16 temperature = adc_to_celsius(adc_value);
                fmt_temp(buf_temp, temperature);

                uart_print((u8 *)"Temp: ");
                uart_println(buf_temp);
                _delay_ms(20);
        }

        return 0;
}
