#include "libc.h"
#include <avr/io.h>
#include <util/delay.h>

#define PCA9555_ADDR 0x20
#define CONFIG_PORT0 0x06 // Digit selection configuration
#define OUTPUT_PORT0 0x02 // Digit selection output
#define CONFIG_PORT1 0x07 // 7-seg segments configuration
#define OUTPUT_PORT1 0x03 // 7-seg segments output

// 7-seg digit definitions (active high for lit segments)
#define DIGIT_0     0b00111111 // segments: a, b, c, d, e, f
#define DIGIT_1     0b00000110 // segments: b, c
#define DIGIT_2     0b01011011 // segments: a, b, d, e, g
#define DIGIT_3     0b01001111 // segments: a, b, c, d, g
#define DIGIT_4     0b01100110 // segments: b, c, f, g
#define DIGIT_5     0b01101101 // segments: a, c, d, f, g
#define DIGIT_6     0b01111101 // segments: a, c, d, e, f, g
#define DIGIT_7     0b00000111 // segments: a, b, c
#define DIGIT_8     0b01111111 // segments: a, b, c, d, e, f, g
#define DIGIT_9     0b01101111 // segments: a, b, c, d, f, g
#define BLANK_DIGIT 0x00

uint8_t digitCodes[10] = {DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4, DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9};

// Digit selection patterns (active low; a low bit enables that digit)
#define DIGIT_SEL_0 0b01111111 // Rightmost digit
#define DIGIT_SEL_1 0b10111111 // 2nd digit from right
#define DIGIT_SEL_2 0b11011111 // 3rd digit from right
#define DIGIT_SEL_3 0b11101111 // Leftmost digit

uint8_t digitSelect[4] = {DIGIT_SEL_0, DIGIT_SEL_1, DIGIT_SEL_2, DIGIT_SEL_3};

void    TWI_init(void) {
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
        TWI_write((PCA9555_ADDR << 1) | 0);
        TWI_write(reg);
        TWI_write(data);
        TWI_stop();
}

int main(void) {
        TWI_init();

        // Configure both ports as outputs initially.
        pca9555_write(CONFIG_PORT0, 0x00); // Port0: digit selection as outputs
        pca9555_write(CONFIG_PORT1, 0x00); // Port1: segments as outputs

        uint16_t counter = 0;

        while (1) {
                // Determine active digits based on counter value.
                uint8_t activeDigits;
                if (counter < 10)
                        activeDigits = 1;
                else if (counter < 100)
                        activeDigits = 2;
                else if (counter < 1000)
                        activeDigits = 3;
                else
                        activeDigits = 4;

                // Extract individual digits.
                // d[0] = ones, d[1] = tens, etc.
                uint8_t  d[4] = {BLANK_DIGIT, BLANK_DIGIT, BLANK_DIGIT, BLANK_DIGIT};
                uint16_t temp = counter;
                for (uint8_t i = 0; i < activeDigits; i++) {
                        d[i] = temp % 10;
                        temp /= 10;
                }

                // Refresh display for 1 second. Within this time, each digit is multiplexed rapidly.
                // Adjust the inner loop count and delay for stable, flicker-free display.
                for (uint8_t refresh = 0; refresh < 100; refresh++) { // 50 cycles * (4 * ~5ms) ≈ 1 second
                        for (uint8_t i = 0; i < 4; i++) {
                                // Activate the digit by selecting it.
                                pca9555_write(OUTPUT_PORT0, digitSelect[i]);
                                // Make sure the segments are in output mode.
                                pca9555_write(CONFIG_PORT1, 0x00);
                                // Only display a digit if it is active; otherwise blank.
                                if (i < activeDigits)
                                        pca9555_write(OUTPUT_PORT1, digitCodes[d[i]]);
                                else
                                        pca9555_write(OUTPUT_PORT1, BLANK_DIGIT);

                                _delay_ms(1); // Active period for the digit.

                                // Clear the display to avoid ghosting.
                                pca9555_write(OUTPUT_PORT0, 0xFF); // Deselect all digits.
                                pca9555_write(OUTPUT_PORT1, BLANK_DIGIT);
                        }
                }

                counter++;
                if (counter > 9999) counter = 0;
        }
        return 0;
}
