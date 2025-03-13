
#include "hal.h"
#include <avr/delay.h>
#include <avr/interrupt.h>

#define MAX_INPUT_LENGTH 32
#define ON_PRESS_ENTER   0x0D
#define ON_PRESS_BACKSPC 127

const char       user[]        = "pollivie";
const char       pass[]        = "admin1234";

volatile uint8_t received_byte = 0;
volatile uint8_t data_ready    = 0;

char             username[MAX_INPUT_LENGTH];
char             password[MAX_INPUT_LENGTH];
uint8_t          input_index = 0;
uint8_t          stage       = 0;
volatile uint8_t last_char;

ISR(USART_RX_vect) {
        last_char     = UDR0;
        received_byte = 1;
}


void putch(char c) {
        while (!(UCSR0A & (1 << UDRE0)))
                ;
        UDR0 = c;
}

void write(const char *str) {
        while (*str) putch(*str++);
}

void clear(char *ptr, uint8_t size) {
        while (size--) {
                *ptr++ = 0;
        }
}

uint8_t compare(const char *p1, const char *p2, uint8_t size) {
        for (uint8_t i = 0; i < size; i++) {
                if (p1[i] != p2[i]) {
                        return (p1[i] - p2[i]);
                }
        }
        return 0;
}

void blink_led(void) {
        DDRB |= (1 << PB5);
        for (int i = 0; i < 5; ++i) {
                PORTB ^= (1 << PB5);
                _delay_ms(100);
        }
        PORTB &= ~(1 << PB5);
}

void blink_led_more(void) {
        uint8_t maskB = (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);
        uint8_t maskD = (1 << PD3) | (1 << PD5) | (1 << PD6);

        DDRB |= maskB;
        DDRD |= maskD;

        for (int i = 0; i < 5; ++i) {
                PORTB |= maskB;
                PORTD |= maskD;
                _delay_ms(100);
                PORTB &= ~maskB;
                PORTD &= ~maskD;
                _delay_ms(100);
        }
}


void init_uart() {
        UBRR0H = 0;
        UBRR0L = 8; // 115200 baud
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
        UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
        sei();
}

uint8_t has_input() {
        return received_byte;
}

uint8_t buffer_push(char *buffer) {
        if (input_index < (MAX_INPUT_LENGTH - 1)) {
                buffer[input_index++] = last_char;
                return 1;
        }
        return 0;
}

uint8_t buffer_pop(char *buffer) {
        if (input_index > 0) {
                input_index--;
                buffer[input_index] = 0;
                return 1;
        }
        return 0;
}

void char_echo(uint8_t ch) {
        putch(ch);
}

void char_erase(uint8_t count) {
        for (uint8_t i = 0; i < count; i++) {
                write("\b \b");
        }
}

uint8_t length(const char *s1) {
        uint8_t index = 0;
        while (*s1++) {
                ++index;
        }
        return index;
}

int main(void) {
        init_uart();
        write("Enter Username: ");
        while (1) {
                if (!has_input()) continue;
                const uint8_t ch = last_char;
                received_byte    = 0;

                if (stage == 0) {
                        if (ch == ON_PRESS_ENTER) {
                                username[input_index] = '\0';
                                if ((compare(user, username, length(user)) == 0) && (length(username) == length(user))) {
                                        blink_led();
                                        write("\r\nCorrect Username\r\nEnter Password: ");
                                        clear(username, MAX_INPUT_LENGTH);
                                        input_index = 0;
                                        stage       = 1;
                                } else {
                                        blink_led();
                                        write("\r\nInvalid Username\r\nEnter Username: ");
                                        clear(username, MAX_INPUT_LENGTH);
                                        input_index = 0;
                                }
                                continue;
                        } else if (ch == ON_PRESS_BACKSPC) {
                                if (buffer_pop(username)) {
                                        char_erase(1);
                                }
                        } else {
                                if (buffer_push(username)) {
                                        char_echo(ch);
                                } else {
                                        blink_led();
                                        write("\r\nUsername too long. Try again.\r\nEnter Username: ");
                                        clear(username, MAX_INPUT_LENGTH);
                                        input_index = 0;
                                }
                        }
                } else if (stage == 1) {
                        if (ch == ON_PRESS_ENTER) {
                                password[input_index] = '\0';
                                if ((compare(pass, password, length(pass)) == 0) && (length(password) == length(pass))) {
                                        blink_led();
                                        write("\r\nCorrect Password\r\n");
                                        stage = 2;
                                        continue;
                                } else {
                                        blink_led();
                                        write("\r\nInvalid Password\r\nEnter Password: ");
                                        clear(password, MAX_INPUT_LENGTH);
                                        input_index = 0;
                                        continue;
                                }
                        } else if (ch == ON_PRESS_BACKSPC) {
                                if (buffer_pop(password)) {
                                        char_erase(1);
                                }
                        } else {
                                if (buffer_push(password)) {
                                        char_echo('*');
                                } else {
                                        blink_led();
                                        write("\r\nPassword too long. Try again.\r\nEnter Password: ");
                                        clear(password, MAX_INPUT_LENGTH);
                                        input_index = 0;
                                }
                        }
                }

                if (stage == 2) {
                        stage = 0;
                        blink_led_more();
                        write("Good work\r\n");
                        blink_led_more();
                        write("Enter Username: ");
                        clear(password, MAX_INPUT_LENGTH);
                        clear(username, MAX_INPUT_LENGTH);
                        input_index = 0;
                        continue;
                }
        }
}
