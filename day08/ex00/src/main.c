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
        // Start frame (4 bytes of 0x00)
        for (uint8_t i = 0; i < 4; i++) spi_cmd(0x00);

        // LED frame for D6: red at full brightness (0xE0 | 31 = 0xFF)
        spi_cmd(0xFF); // global brightness
        spi_cmd(0x00); // blue
        spi_cmd(0x00); // green
        spi_cmd(0xFF); // red

        // LED frame for D7: off (colors all 0)
        spi_cmd(0xE0); // minimal brightness
        spi_cmd(0x00);
        spi_cmd(0x00);
        spi_cmd(0x00);
        //
        // LED frame for D8: off (colors all 0)
        spi_cmd(0xE0); // minimal brightness
        spi_cmd(0x00);
        spi_cmd(0x00);
        spi_cmd(0x00);

        // End frame (enough bytes for 2 LEDs; 4 bytes work fine)
        for (uint8_t i = 0; i < 4; i++) spi_cmd(0xFF);

        while (1) {
        }
}
