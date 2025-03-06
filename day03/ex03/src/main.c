#include "hal.h"
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define MAX_INPUT_LENGTH 8 // "#RRGGBB" + terminating null


static Color     color = (Color){.r = 0, .g = 0, .b = 0};
static char      buffer[MAX_INPUT_LENGTH];
static uint8_t   index          = 0;
volatile uint8_t last_char      = 0;
volatile uint8_t color_received = 0;

static void      display_color(void) {
        OCR0B = color.r; // Red on PD5 (OC0B)
        OCR0A = color.g; // Green on PD6 (OC0A)
        OCR2B = color.b; // Blue on PD3 (OC2B)
}

static void init_rgb(void) {
        // PD3 = Blue, PD5 = Red, PD6 = Green
        PORTD |= ((1 << PD3) | (1 << PD5) | (1 << PD6));
        DDRD |= ((1 << PD3) | (1 << PD5) | (1 << PD6));
}

static void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
        color.r = r;
        color.g = g;
        color.b = b;
}

static void init_uart(void) {
        UBRR0H = 0;
        UBRR0L = 8; // 115200 baud
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
        UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
        sei();
}

void uart_tx(char c) {
        while (!(UCSR0A & (1 << UDRE0))) {
                // wait until ready
        }
        UDR0 = c;
}

void write(const char *ptr) {
        while (*ptr) {
                uart_tx(*ptr++);
        }
}

ISR(USART_RX_vect) {
        char c    = UDR0;
        last_char = c;

        // Handle backspace
        if (c == 0x08) {
                if (index > 0) {
                        index--;
                        // Erase the character on the terminal
                        uart_tx(0x08);
                        uart_tx(' ');
                        uart_tx(0x08);
                }
                return;
        }

        if (c == '\r' || c == '\n') {
                buffer[index]  = '\0';
                color_received = 1;
                uart_tx('\r');
                uart_tx('\n');
        } else {
                if (index < (MAX_INPUT_LENGTH - 1)) {
                        buffer[index++] = c;
                        uart_tx(c);
                }
        }
}

static void init_timer_pwm(void) {
        // Timer0 configuration for OC0A (green, PD6) and OC0B (red, PD5)
        // Fast PWM, 8-bit mode with non-inverting outputs.
        TCCR0A = (1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0B1);
        // Prescaler 64 for Timer0
        TCCR0B = (1 << CS01) | (1 << CS00);

        // Timer2 configuration for OC2B (blue, PD3)
        // Fast PWM, 8-bit mode with non-inverting output on OC2B.
        TCCR2A = (1 << WGM20) | (1 << WGM21) | (1 << COM2B1);
        // Prescaler 64 for Timer2 (CS22 = 1 gives prescaler 64)
        TCCR2B = (1 << CS22);
}

static bool is_valid_hex(void) {
        // Expecting "#RRGGBB": must be 7 characters long
        // buffer[0] must be '#' and buffer[1..6] valid hex digits.
        uint8_t len = 0;
        while (buffer[len] != '\0' && len < MAX_INPUT_LENGTH) {
                len++;
        }
        if (len != 7 || buffer[0] != '#') return false;
        for (uint8_t i = 1; i < 7; i++) {
                char c = buffer[i];
                if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))) return false;
        }
        return true;
}

static uint8_t hex_to_dec(char c) {
        if (c >= '0' && c <= '9')
                return c - '0';
        else
                return c - 'A' + 10;
}

static uint8_t parse_hex_byte(const char *hex) {
        return (hex_to_dec(hex[0]) << 4) | hex_to_dec(hex[1]);
}

static void clear_buffer(void) {
        for (uint8_t i = 0; i < MAX_INPUT_LENGTH; i++) {
                buffer[i] = 0;
        }
        index = 0;
}

int main(void) {
        init_rgb();
        init_timer_pwm();
        init_uart();

        loop {
                if (!color_received) continue;

                if (is_valid_hex()) {
                        uint8_t r = parse_hex_byte(&buffer[1]); // characters 1 and 2
                        uint8_t g = parse_hex_byte(&buffer[3]); // characters 3 and 4
                        uint8_t b = parse_hex_byte(&buffer[5]); // characters 5 and 6
                        set_rgb(r, g, b);
                        display_color();
                } else {
                        write("Invalid color code\r\n");
                }
                clear_buffer();
                color_received = 0;
        }
}
