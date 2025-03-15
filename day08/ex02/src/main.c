#include "libc.h"
#include <avr/io.h>
#include <util/delay.h>

typedef union {
        uint8_t bytes[3];
        struct {
                uint8_t r;
                uint8_t g;
                uint8_t b;
        };
} Color;

#define BLACK                                                                                                                                                  \
        (const Color) {                                                                                                                                        \
                .r = 0, .g = 0, .b = 0                                                                                                                         \
        }
#define WHITE                                                                                                                                                  \
        (const Color) {                                                                                                                                        \
                .r = 255, .g = 255, .b = 255                                                                                                                   \
        }

void spi_init(void) {
        DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB5);
        SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void spi_cmd(uint8_t data) {
        SPDR = data;
        while (!(SPSR & (1 << SPIF)))
                ;
}

void spi_set_color(Color d6, Color d7, Color d8) {
        const uint16_t t6 = d6.r + d6.g + d6.b;
        const uint16_t t7 = d7.r + d7.g + d7.b;
        const uint16_t t8 = d8.r + d8.g + d8.b;

        for (uint8_t i = 0; i < 12; i++) spi_cmd(0x00);

        spi_cmd(t6 != 0 ? 0xFF : 0xE0);
        spi_cmd(d6.r);
        spi_cmd(d6.g);
        spi_cmd(d6.b);

        spi_cmd(t7 != 0 ? 0xFF : 0xE0);
        spi_cmd(d7.r);
        spi_cmd(d7.g);
        spi_cmd(d7.b);

        spi_cmd(t8 != 0 ? 0xFF : 0xE0);
        spi_cmd(d8.r);
        spi_cmd(d8.g);
        spi_cmd(d8.b);

        for (uint8_t i = 0; i < 12; i++) spi_cmd(0xFF);
}

int main(void) {
        spi_init();

        while (1) {
                // D6 on for 250ms
                spi_set_color(WHITE, BLACK, BLACK);
                _delay_ms(250);

                // D7 on for 250ms
                spi_set_color(BLACK, WHITE, BLACK);
                _delay_ms(250);

                // D8 on for 250ms
                spi_set_color(BLACK, BLACK, WHITE);
                _delay_ms(250);

                // All off for 250ms (using 0xE0 brightness with BLACK)
                spi_set_color(BLACK, BLACK, BLACK);
                _delay_ms(250);
        }
}
