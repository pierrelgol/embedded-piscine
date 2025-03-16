#include "libc.h"
#include <avr/io.h>
#include <util/delay.h>

#define PCA9555_ADDR 0x20
#define INPUT_PORT0  0x00 // Input register for Port 0
#define OUTPUT_PORT0 0x02 // Output register for Port 0
#define CONFIG_PORT0 0x06 // Configuration register for Port 0

void TWI_init(void) {
        TWSR = 0x00;
        TWBR = 72; // Set for ~100kHz SCL at F_CPU = 16MHz
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

uint8_t TWI_read_nack(void) {
        TWCR = (1 << TWINT) | (1 << TWEN);
        while (!(TWCR & (1 << TWINT)))
                ;
        return TWDR;
}

void pca9555_write(uint8_t reg, uint8_t data) {
        TWI_start();
        TWI_write((PCA9555_ADDR << 1) | 0); // Write mode
        TWI_write(reg);
        TWI_write(data);
        TWI_stop();
}

uint8_t pca9555_read(uint8_t reg) {
        uint8_t data;
        TWI_start();
        TWI_write((PCA9555_ADDR << 1) | 0);
        TWI_write(reg);
        TWI_start();
        TWI_write((PCA9555_ADDR << 1) | 1); // Read mode
        data = TWI_read_nack();
        TWI_stop();
        return data;
}

int main(void) {
        TWI_init();

        // Configure Port 0:
        // Bit 0 (SW3) as input (1), bits 1-3 as outputs (0), and bits 4-7 as inputs (1).
        // Binary: 1 1 1 1  0 0 0 1  = 0xF1.
        pca9555_write(CONFIG_PORT0, 0b11110001);

        uint8_t counter  = 7;
        uint8_t prev_sw3 = 1; // Assume SW3 is not pressed initially (active low)

        while (1) {
                // Read Port 0 input register to get SW3 state.
                uint8_t port0_in = pca9555_read(INPUT_PORT0);
                uint8_t sw3      = port0_in & 0x01; // SW3 is on bit 0

                // Detect a falling edge: if previously high and now low.
                if (prev_sw3 == 1 && sw3 == 0) {
                        if (counter == 0) {
                                counter = 7;
                        } else {
                                counter--;
                        }
                        _delay_ms(50); // Simple debounce delay
                }
                prev_sw3 = sw3;

                // Display the lower 3 bits of the counter on LEDs at bits 1-3.
                // Shift the counter (masked to 3 bits) left by 1 so it occupies bits 1,2,3.
                uint8_t led_out = (counter & 0x07) << 1;
                pca9555_write(OUTPUT_PORT0, led_out);

                _delay_ms(10); // Polling delay
        }

        return 0;
}
