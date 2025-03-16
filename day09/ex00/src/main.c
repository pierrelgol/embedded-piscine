#include "libc.h"
#include <avr/io.h>
#include <util/delay.h>

#define PCA9555_ADDR 0x20 // I²C address (ensure this is correct for your setup)
#define CONFIG_PORT0 0x06 // Port 0 configuration register
#define OUTPUT_PORT0 0x02 // Port 0 output register

void TWI_init(void) {
        TWSR = 0x00;
        TWBR = 72;
        TWCR = (1 << TWEN);
}

void TWI_start(void) {
        TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
        while (!(TWCR & (1 << TWINT)))
                ;
}

void TWI_stop(void) {
        TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void TWI_write(uint8_t data) {
        TWDR = data;
        TWCR = (1 << TWINT) | (1 << TWEN);
        while (!(TWCR & (1 << TWINT)))
                ;
}

void pca9555_write(uint8_t reg, uint8_t data) {
        TWI_start();
        TWI_write((PCA9555_ADDR << 1) | 0); // Write mode (LSB = 0)
        TWI_write(reg);
        TWI_write(data);
        TWI_stop();
}

int main(void) {
        TWI_init();

        // First, set all Port 0 pins as inputs.
        pca9555_write(CONFIG_PORT0, 0xFF);
        // Then, configure only the LED pin (bit 3) as output.
        pca9555_write(CONFIG_PORT0, ~(1 << 3)); // ~(1<<3) is 0xF7: bit 3 is 0 (output), all others 1 (input)

        while (1) {
                pca9555_write(OUTPUT_PORT0, (1 << 3)); // Turn LED on (set bit 3 high)
                _delay_ms(500);
                pca9555_write(OUTPUT_PORT0, 0); // Turn LED off
                _delay_ms(500);
        }

        return 0;
}
