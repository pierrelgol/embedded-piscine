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

#define BLACK ((const Color){.r = 0, .g = 0, .b = 0})
#define WHITE ((const Color){.r = 255, .g = 255, .b = 255})

// Global variables for LED colors.
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

// Update the three APA102 LEDs.
// A start frame (12 x 0x00) is sent, then one frame per LED,
// then an end frame (12 x 0xFF).
void spi_set_color(Color d6, Color d7, Color d8) {
        uint16_t t6 = d6.r + d6.g + d6.b;
        uint16_t t7 = d7.r + d7.g + d7.b;
        uint16_t t8 = d8.r + d8.g + d8.b;

        for (uint8_t i = 0; i < 12; i++) {
                spi_cmd(0x00);
        }

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

        for (uint8_t i = 0; i < 12; i++) {
                spi_cmd(0xFF);
        }
}

// ADC initialization on ADC0 (RV1).
void adc_init(void) {
        ADMUX  = (1 << REFS0); // Use AVcc as reference, ADC0 selected.
        ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t read_adc(void) {
        ADCSRA |= (1 << ADSC); // Start conversion.
        while (ADCSRA & (1 << ADSC))
                ; // Wait until conversion complete.
        return ADC;
}

// Define button pins.
#define SW1_PIN PD2 // Validate primary color (switches from red → green → blue)
#define SW2_PIN PD4 // Switch to the next LED

int main(void) {
        spi_init();
        adc_init();

        // Configure SW1 and SW2 as input with pull-up.
        DDRD &= ~((1 << SW1_PIN) | (1 << SW2_PIN));
        PORTD |= ((1 << SW1_PIN) | (1 << SW2_PIN));

        // currentLED: 0 = D6, 1 = D7, 2 = D8.
        uint8_t currentLED = 0;
        // currentPrimary: 0 = red, 1 = green, 2 = blue.
        uint8_t currentPrimary = 0;

        while (1) {
                // Read potentiometer value from RV1 (ADC0), scale 10-bit to 8-bit.
                uint16_t adc_val  = read_adc();
                uint8_t  potValue = (uint8_t)(adc_val >> 2);

                // Update the active primary color on the selected LED.
                if (currentLED == 0) {
                        if (currentPrimary == 0)
                                ledD6.r = potValue;
                        else if (currentPrimary == 1)
                                ledD6.g = potValue;
                        else
                                ledD6.b = potValue;
                } else if (currentLED == 1) {
                        if (currentPrimary == 0)
                                ledD7.r = potValue;
                        else if (currentPrimary == 1)
                                ledD7.g = potValue;
                        else
                                ledD7.b = potValue;
                } else { // currentLED == 2
                        if (currentPrimary == 0)
                                ledD8.r = potValue;
                        else if (currentPrimary == 1)
                                ledD8.g = potValue;
                        else
                                ledD8.b = potValue;
                }

                // Refresh the LED display.
                spi_set_color(ledD6, ledD7, ledD8);

                // Check SW1: When pressed, validate the current value and move to the next primary.
                if (!(PIND & (1 << SW1_PIN))) { // Active low.
                        _delay_ms(50);          // Debounce.
                        while (!(PIND & (1 << SW1_PIN)))
                                ; // Wait for release.
                        _delay_ms(50);
                        currentPrimary = (currentPrimary + 1) % 3;
                }

                // Check SW2: When pressed, switch to the next LED and reset primary to red.
                if (!(PIND & (1 << SW2_PIN))) { // Active low.
                        _delay_ms(50);          // Debounce.
                        while (!(PIND & (1 << SW2_PIN)))
                                ;
                        _delay_ms(50);
                        currentLED     = (currentLED + 1) % 3;
                        currentPrimary = 0;
                }
        }

        return 0;
}
