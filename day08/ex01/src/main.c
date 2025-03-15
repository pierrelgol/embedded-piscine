#include <avr/io.h>
#include <util/delay.h>

void spi_init(void) {
        DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB5);
        SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void spi_cmd(uint8_t data) {
        SPDR = data;
        while (!(SPSR & (1 << SPIF)))
                ;
}

int main(void) {
        spi_init();

        // APA102 LED frame format: global brightness, blue, green, red.
        // Use full brightness (0xFF = 0xE0 | 0x1F)
        const uint8_t colors[][4] = {
            {0xFF, 0x00, 0x00, 0xFF}, // red    (R=ff, G=00, B=00)
            {0xFF, 0x00, 0xFF, 0x00}, // green  (R=00, G=ff, B=00)
            {0xFF, 0xFF, 0x00, 0x00}, // blue   (R=00, G=00, B=ff)
            {0xFF, 0x00, 0xFF, 0xFF}, // yellow (R=ff, G=ff, B=00)
            {0xFF, 0xFF, 0xFF, 0x00}, // cyan   (R=00, G=ff, B=ff)
            {0xFF, 0xFF, 0x00, 0xFF}, // magenta(R=ff, G=00, B=ff)
            {0xFF, 0xFF, 0xFF, 0xFF}  // white  (R=ff, G=ff, B=ff)
        };
        uint8_t num_colors = sizeof(colors) / sizeof(colors[0]);
        uint8_t current    = 0;

        while (1) {
                // Start frame: 4 bytes of 0x00
                for (uint8_t i = 0; i < 4; i++) spi_cmd(0x00);

                // LED frame for D6: set to the current color.
                for (uint8_t j = 0; j < 4; j++) spi_cmd(colors[current][j]);

                // LED frames for D7 and D8: off (minimal brightness, colors = 0)
                for (uint8_t i = 0; i < 2; i++) {
                        spi_cmd(0xE0); // off frame (global brightness minimal)
                        spi_cmd(0x00);
                        spi_cmd(0x00);
                        spi_cmd(0x00);
                }

                // End frame: 4 bytes of 0xFF (sufficient for 3 LEDs)
                for (uint8_t i = 0; i < 4; i++) spi_cmd(0xFF);

                _delay_ms(1000); // Change color every second
                current = (current + 1) % num_colors;
        }

        return 0;
}
