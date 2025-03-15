/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libc.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollivie <pollivie.student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 21:00:56 by pollivie          #+#    #+#             */
/*   Updated: 2025/03/03 21:00:56 by pollivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBC_H
#define LIBC_H

#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#define loop for (;;)

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef MAX_LINE_SIZE
#define MAX_LINE_SIZE 128
#endif

typedef struct {
        uint16_t baud_rate;
        bool     transmit;
        bool     receive;
} UartOption;

void    uart_init(UartOption *opts);
uint8_t uart_getchar();
void    uart_putchar(uint8_t c);
int16_t uart_printf(const char *fmt, ...);
int16_t uart_read(char *buff, size_t bufsize);
int16_t uart_read_until_delimiter(char *buff, size_t bufsize, uint8_t delimiter);
int16_t uart_write(const char *buff, size_t bufsize);
int16_t uart_write_until_delimiter(const char *buff, size_t bufsize, uint8_t delimiter);
int16_t uart_getline(char *buff, size_t bufsize);
int16_t uart_getline_echo(char *buff, size_t bufsize);
int16_t uart_putline(const char *buff);

#define uart_assert(reg, value)                                                                                                                                \
        do {                                                                                                                                                   \
                if ((reg) != (value)) {                                                                                                                        \
                        uart_printf("Assertion failed: %s != %s (got: 0x%X, expected: 0x%X)\r\n", #reg, #value, (unsigned int)(reg), (unsigned int)(value));   \
                        while (1) {                                                                                                                            \
                        } /* Halt execution on assertion failure */                                                                                            \
                }                                                                                                                                              \
        } while (0)


int16_t  parse_number(const char *number, uint8_t base);
uint16_t parse_unsigned(const char *number, uint8_t base);

bool     is_whitespace(uint8_t c);
bool     is_alphabetic(uint8_t c);
bool     is_digit(uint8_t c);
bool     is_alphanumeric(uint8_t c);
bool     is_punctuation(uint8_t c);
bool     is_xdigit(uint8_t c);
bool     is_cntrl(uint8_t c);
bool     is_upper(uint8_t c);
bool     is_lower(uint8_t c);
bool     is_print(uint8_t c);

int16_t  string_count(const char *str, char ch);
int16_t  string_ends_with(const char *str, const char *suffix);
int16_t  string_starts_with(const char *str, const char *prefix);
int16_t  string_case_compare(const char *s1, const char *s2);
int16_t  string_compare(const char *s1, const char *s2);
int16_t  string_reverse(char *str);
int16_t  string_copy(char *dest, const char *src);
int16_t  string_concat(char *dest, const char *src);
int16_t  string_contains(const char *str, const char *substr);
char    *string_search_byte(const char *str, char c);
char    *string_search_substring(const char *str, const char *substr);
int16_t  string_last_index_of_none(const char *str, const char *set);
int16_t  string_first_index_of_none(const char *str, const char *set);
int16_t  string_last_index_of(const char *str, char c);
int16_t  string_first_index_of(const char *str, char c);
int16_t  string_spn(const char *s, const char *accept);
int16_t  string_cspn(const char *s, const char *reject);
int16_t  string_length(const char *s);
char    *string_tokenize(char *str, const char *delim);


int16_t  readline_echo_back(char *buffer, size_t busize);
int16_t  clear_line(uint16_t count);


#endif // LIBC_H
