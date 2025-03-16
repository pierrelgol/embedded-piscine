#include "libc.h"
#include <avr/io.h>
#include <util/delay.h>


#define DIGIT_0 63  // 0x3F: segments a, b, c, d, e, f
#define DIGIT_1 6   // 0x06: segments b, c
#define DIGIT_2 91  // 0x5B: segments a, b, d, e, g
#define DIGIT_3 79  // 0x4F: segments a, b, c, d, g
#define DIGIT_4 102 // 0x66: segments b, c, f, g
#define DIGIT_5 109 // 0x6D: segments a, c, d, f, g
#define DIGIT_6 125 // 0x7D: segments a, c, d, e, f, g
#define DIGIT_7 7   // 0x07: segments a, b, c
#define DIGIT_8 127 // 0x7F: segments a, b, c, d, e, f, g
#define DIGIT_9 111 // 0x6F: segments a, b, c, d, f, g


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

#define PCA9555_ADDR 0x20

#define CONFIG_PORT0 0x06
#define OUTPUT_PORT0 0x02

#define CONFIG_PORT1 0x07
#define OUTPUT_PORT1 0x03

void pca9555_write(uint8_t reg, uint8_t data) {
        TWI_start();
        TWI_write((PCA9555_ADDR << 1) | 0);
        TWI_write(reg);
        TWI_write(data);
        TWI_stop();
}

typedef union bit_writer {
        uint8_t v;
        struct {
                uint8_t _0 : 1;
                uint8_t _1 : 1;
                uint8_t _2 : 1;
                uint8_t _3 : 1;
                uint8_t _4 : 1;
                uint8_t _5 : 1;
                uint8_t _6 : 1;
                uint8_t _7 : 1;
        } b;

} bit_writer;

int main(void) {
        TWI_init();

        pca9555_write(CONFIG_PORT0, 0b01111111);
        pca9555_write(OUTPUT_PORT0, 0b01111111);

        pca9555_write(CONFIG_PORT1, 0xFF);
        pca9555_write(CONFIG_PORT1, 0x00);

        // while (1) {
        //         pca9555_write(OUTPUT_PORT1, DIGIT_0);
        //         _delay_ms(1000);
        //         pca9555_write(OUTPUT_PORT1, DIGIT_1);
        //         _delay_ms(1000);
        //         pca9555_write(OUTPUT_PORT1, DIGIT_2);
        //         _delay_ms(1000);
        //         pca9555_write(OUTPUT_PORT1, DIGIT_3);
        //         _delay_ms(1000);
        //         pca9555_write(OUTPUT_PORT1, DIGIT_4);
        //         _delay_ms(1000);
        //         pca9555_write(OUTPUT_PORT1, DIGIT_5);
        //         _delay_ms(1000);
        //         pca9555_write(OUTPUT_PORT1, DIGIT_6);
        //         _delay_ms(1000);
        //         pca9555_write(OUTPUT_PORT1, DIGIT_7);
        //         _delay_ms(1000);
        //         pca9555_write(OUTPUT_PORT1, DIGIT_8);
        //         _delay_ms(1000);
        //         pca9555_write(OUTPUT_PORT1, DIGIT_9);
        //         _delay_ms(1000);
        // }


        pca9555_write(OUTPUT_PORT1, DIGIT_2);
        while (1) {
        }

        return 0;
}
