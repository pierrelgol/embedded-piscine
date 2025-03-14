#include "hal.h"
#include <avr/io.h>
#include <avr/eeprom.h>

#define ArraySize(ptr) (sizeof(ptr) / sizeof(ptr[0]))

static const uint8_t hex_chars[] = "0123456789ABCDEF";

void                 uart_init() {
        UBRR0H = 0;
        UBRR0L = 8;
        UCSR0B = (1 << TXEN0);
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_putchar(uint8_t c) {
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = c;
}

void uart_print(const char* str) {
        while (*str) {
                uart_putchar(*str++);
        }
}

void printHex(uint8_t c) {
        uart_putchar(hex_chars[(c >> 4) & 0x0F]);
        uart_putchar(hex_chars[c & 0x0F]);
}

void printAddress(uint16_t addr, uint8_t depth) {
        for (uint8_t i = depth; i > 0; i--) {
                uart_putchar(hex_chars[(addr >> ((i - 1) * 4)) & 0x0F]);
        }
}

int main() {
        uart_init();
        uint8_t buffer[256];

        eeprom_read_block(buffer, (const void*)0, ArraySize(buffer));

        for (uint16_t i = 0; i < ArraySize(buffer); i += 16) {
                printAddress(i, 8);
                uart_putchar(':');
                uart_putchar(' ');

                for (uint8_t j = 0; j < 16; j++) {
                        printHex(buffer[i + j]);
                        uart_putchar(' ');
                }

                uart_putchar('\r');
                uart_putchar('\n');
        }

        while (1);
}
