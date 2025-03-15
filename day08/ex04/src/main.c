#include "libc.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

typedef union {
        uint8_t bytes[3];
        struct {
                uint8_t r;
                uint8_t g;
                uint8_t b;
        };
} Color;

#define BLACK ((const Color){.r = 0, .g = 0, .b = 0})
#define WHITE ((const Color){.r = 255, .g = 255, .b = 255})

Color ledD6 = BLACK;
Color ledD7 = BLACK;
Color ledD8 = BLACK;

void  spi_init(void) {
        DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB5);
        SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void spi_cmd(uint8_t data) {
        SPDR = data;
        while (!(SPSR & (1 << SPIF)))
                ;
}

void spi_set_color(Color d6, Color d7, Color d8) {
        uint16_t t6 = d6.r + d6.g + d6.b;
        uint16_t t7 = d7.r + d7.g + d7.b;
        uint16_t t8 = d8.r + d8.g + d8.b;

        // Start frame (12 bytes of 0x00 as in your design)
        for (uint8_t i = 0; i < 12; i++) {
                spi_cmd(0x00);
        }

        // LED frame for each LED (brightness byte + colors)
        spi_cmd(t6 ? 0xFF : 0xE0);
        spi_cmd(d6.r);
        spi_cmd(d6.g);
        spi_cmd(d6.b);

        spi_cmd(t7 ? 0xFF : 0xE0);
        spi_cmd(d7.r);
        spi_cmd(d7.g);
        spi_cmd(d7.b);

        spi_cmd(t8 ? 0xFF : 0xE0);
        spi_cmd(d8.r);
        spi_cmd(d8.g);
        spi_cmd(d8.b);

        // End frame (12 bytes of 0xFF)
        for (uint8_t i = 0; i < 12; i++) {
                spi_cmd(0xFF);
        }
}

void wheel(uint8_t pos) {
        pos     = 255 - pos;
        Color c = BLACK;
        if (pos < 85) {
                c = (const Color){.r = 255 - pos * 3, .g = 0, .b = pos * 3};
        } else if (pos < 170) {
                pos -= 85;
                c = (const Color){.r = 0, .g = pos * 3, .b = 255 - pos * 3};
        } else {
                pos -= 170;
                c = (const Color){.r = pos * 3, .g = 255 - pos * 3, .b = 0};
        }
        spi_set_color(c, c, c);
}

void trim_newline(char *str) {
        size_t len = strlen(str);
        while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
                str[len - 1] = '\0';
                len--;
        }
}

int main(void) {
        spi_init();
        char line[MAX_LINE_SIZE];

        while (1) {
                int16_t len = uart_getline(line, MAX_LINE_SIZE);
                if (len <= 0) continue;

                trim_newline(line);

                // Check for rainbow command (case-insensitive compare)
                if (string_case_compare(line, "#FULLRAINBOW") == 0) {
                        for (uint16_t i = 0; i < 256; i++) {
                                wheel((uint8_t)i);
                                _delay_ms(20);
                        }
                        continue;
                }

                // Validate LED color command: expecting "#RRGGBBDX" (9 characters)
                if (strlen(line) == 9 && line[0] == '#' && line[7] == 'D' && (line[8] == '6' || line[8] == '7' || line[8] == '8')) {

                        char    buf[3] = {0};
                        int16_t r, g, b;

                        buf[0]         = line[1];
                        buf[1]         = line[2];
                        r              = parse_number(buf, 16);

                        buf[0]         = line[3];
                        buf[1]         = line[4];
                        g              = parse_number(buf, 16);

                        buf[0]         = line[5];
                        buf[1]         = line[6];
                        b              = parse_number(buf, 16);

                        Color newColor = {.r = (uint8_t)r, .g = (uint8_t)g, .b = (uint8_t)b};

                        if (line[8] == '6') {
                                ledD6 = newColor;
                        } else if (line[8] == '7') {
                                ledD7 = newColor;
                        } else if (line[8] == '8') {
                                ledD8 = newColor;
                        }

                        spi_set_color(ledD6, ledD7, ledD8);

                        uart_printf("New LED Colors: D6=#%02X%02X%02X, D7=#%02X%02X%02X, D8=#%02X%02X%02X\r\n",
                                    ledD6.r,
                                    ledD6.g,
                                    ledD6.b,
                                    ledD7.r,
                                    ledD7.g,
                                    ledD7.b,
                                    ledD8.r,
                                    ledD8.g,
                                    ledD8.b);
                } else {
                        uart_putline("Error: Invalid input. Expected format: #RRGGBBDX or #FULLRAINBOW");
                }
        }
        return 0;
}
