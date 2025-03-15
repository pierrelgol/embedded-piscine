/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libc.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollivie <pollivie.student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 22:05:30 by pollivie          #+#    #+#             */
/*   Updated: 2025/03/13 22:05:30 by pollivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libc.h"
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

static uint8_t uart_initialized = false;

void           uart_init(UartOption *opts) {
        if (uart_initialized) return;
        uart_initialized = true;

        if (!opts) {
                UBRR0H = 0;
                UBRR0L = 8;
                UCSR0B = (1 << TXEN0);
                UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
        } else {
                UBRR0H = (opts->baud_rate >> 8);
                UBRR0L = opts->baud_rate;

                if (opts->receive) {
                        UCSR0B |= (1 << RXEN0);
                }
                if (opts->transmit) {
                        UCSR0B |= (1 << TXEN0);
                }
                UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
        }
}

static int16_t uart_fmt_byte(char c) {
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = c;
        return 1;
}

static int16_t uart_fmt_string(char *s, uint16_t len) {
        if (!s) {
                return -1;
        }
        uint16_t i = 0;
        while (i < len) {
                loop_until_bit_is_set(UCSR0A, UDRE0);
                UDR0 = s[i++];
        }
        return i;
}

static int16_t uart_fmt_number(uint16_t number, uint8_t base) {
        if (base < 2 || base > 36) {
                return -1;
        }
        int16_t printed = 0;
        char    buf[17];
        int32_t pos = 0;

        if (number == 0) {
                return uart_fmt_byte('0');
        }


        while (number) {
                uint8_t digit = number % base;
                number /= base;
                buf[pos++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        }


        for (int32_t i = pos - 1; i >= 0; i--) {
                printed += uart_fmt_byte(buf[i]);
        }
        return printed;
}

static int16_t uart_fmt_address(uintptr_t address, uint8_t base) {
        if (base < 2 || base > 36) {
                return -1;
        }
        int16_t printed = 0;

        if (base == 16) {
                printed += uart_fmt_string("0x", 2);
        }
        char    buf[32];
        int32_t pos = 0;

        if (address == 0) {
                return printed + uart_fmt_byte('0');
        }

        while (address) {
                uint8_t digit = address % base;
                address /= base;
                buf[pos++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        }

        for (int32_t i = pos - 1; i >= 0; i--) {
                printed += uart_fmt_byte(buf[i]);
        }
        return printed;
}

static int16_t uart_fmt_number_padded(uint32_t number, uint8_t base, int pad_width, char padChar) {
        int16_t printed = 0;
        char    buf[33];
        int     pos = 0;

        if (number == 0) {
                buf[pos++] = '0';
        } else {
                while (number) {
                        uint8_t digit = number % base;
                        number /= base;
                        buf[pos++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
                }
        }
        int pad_count = pad_width - pos;
        while (pad_count-- > 0) {
                printed += uart_fmt_byte(padChar);
        }
        for (int i = pos - 1; i >= 0; i--) {
                printed += uart_fmt_byte(buf[i]);
        }
        return printed;
}

int16_t uart_printf(const char *fmt, ...) {
        if (!uart_initialized) {
                uart_init((void *)0);
        }
        va_list args;
        va_start(args, fmt);
        int16_t printed = 0;

        while (*fmt) {
                if (*fmt == '%') {
                        fmt++;
                        if (!*fmt) break;

                        int  pad_width = 0;
                        char pad_char  = ' ';

                        if (*fmt == '0') {
                                pad_char = '0';
                                fmt++;
                        }
                        while (*fmt >= '0' && *fmt <= '9') {
                                pad_width = pad_width * 10 + (*fmt - '0');
                                fmt++;
                        }

                        switch (*fmt) {
                                case 'c' :
                                        {
                                                char ch = (char)va_arg(args, uint16_t);
                                                printed += uart_fmt_byte(ch);
                                                break;
                                        }
                                case 's' :
                                        {
                                                char *str = va_arg(args, char *);
                                                if (str) {
                                                        while (*str) printed += uart_fmt_byte(*str++);
                                                } else {
                                                        printed += uart_fmt_string("(null)", 6);
                                                }
                                                break;
                                        }
                                case 'd' :
                                case 'i' :
                                        {
                                                int32_t val = va_arg(args, int16_t);
                                                if (val < 0) {
                                                        printed += uart_fmt_byte('-');
                                                        printed += uart_fmt_number_padded((uint32_t)(-val), 10, pad_width, pad_char);
                                                } else {
                                                        printed += uart_fmt_number_padded((uint32_t)val, 10, pad_width, pad_char);
                                                }
                                                break;
                                        }
                                case 'u' :
                                        {
                                                uint32_t val = va_arg(args, uint16_t);
                                                printed += uart_fmt_number_padded(val, 10, pad_width, pad_char);
                                                break;
                                        }
                                case 'b' :
                                        {
                                                uint32_t val = va_arg(args, uint16_t);
                                                printed += uart_fmt_number_padded(val, 2, pad_width, pad_char);
                                                break;
                                        }
                                case 'x' :
                                case 'X' :
                                        {
                                                uint32_t val = va_arg(args, uint16_t);
                                                printed += uart_fmt_number_padded(val, 16, pad_width, pad_char);
                                                break;
                                        }
                                case 'p' :
                                        {
                                                void *ptr = (void *)va_arg(args, void *);
                                                printed += uart_fmt_address((uintptr_t)ptr, 16);
                                                break;
                                        }
                                case '%' :
                                        {
                                                printed += uart_fmt_byte('%');
                                                break;
                                        }
                                default :
                                        {
                                                printed += uart_fmt_byte(*fmt);
                                                break;
                                        }
                        }
                } else {
                        printed += uart_fmt_byte(*fmt);
                }
                fmt++;
        }
        va_end(args);
        return printed;
}


int16_t uart_read(char *buff, size_t bufsize) {
        if (!buff || bufsize == 0) {
                return -1;
        }
        size_t i = 0;
        while (i < bufsize) {
                loop_until_bit_is_set(UCSR0A, RXC0);
                buff[i++] = UDR0;
        }
        return i;
}

int16_t uart_read_until_delimiter(char *buff, size_t bufsize, uint8_t delimiter) {
        if (!buff || bufsize == 0) {
                return -1;
        }
        size_t i = 0;

        while (i < bufsize - 1) {
                loop_until_bit_is_set(UCSR0A, RXC0);
                char c = UDR0;
                if (c == delimiter) break;
                buff[i++] = c;
        }
        buff[i] = '\0';
        return i;
}

int16_t uart_write(const char *buff, size_t bufsize) {
        if (!buff) {
                return -1;
        }
        size_t i = 0;
        while (i < bufsize) {
                loop_until_bit_is_set(UCSR0A, UDRE0);
                UDR0 = buff[i++];
        }
        return i;
}

int16_t uart_write_until_delimiter(const char *buff, size_t bufsize, uint8_t delimiter) {
        if (!buff) {
                return -1;
        }
        size_t i = 0;
        while (i < bufsize) {
                if (buff[i] == delimiter) break;
                loop_until_bit_is_set(UCSR0A, UDRE0);
                UDR0 = buff[i++];
        }
        return i;
}

int16_t uart_getline(char *buff, size_t bufsize) {
        int16_t len = uart_read_until_delimiter(buff, bufsize, '\n');

        if (len > 0 && buff[len - 1] == '\r') {
                buff[len - 1] = '\0';
                len--;
        }
        return len;
}

int16_t uart_getline_echo(char *buff, size_t bufsize) {
        if (!buff || bufsize == 0) {
                return -1;
        }
        size_t i = 0;

        while (i < bufsize - 1) {
                loop_until_bit_is_set(UCSR0A, RXC0);
                char c = UDR0;
                if (c == '\n' && buff[i - 1] == 'r') break;
                loop_until_bit_is_set(UCSR0A, UDRE0);
                buff[i++] = c;
                UDR0      = buff[i];
        }
        buff[i] = '\0';
        return i;
}

int16_t uart_putline(const char *buff) {
        int16_t written = uart_write(buff, string_length(buff));
        written += uart_write("\r\n", 2);
        return written;
}

int16_t parse_number(const char *number, uint8_t base) {
        if (!number || base < 2 || base > 36) return 0;

        while (*number == ' ' || *number == '\t') number++;

        int32_t negative = 0;
        if (*number == '-') {
                negative = 1;
                number++;
        } else if (*number == '+') {
                number++;
        }

        int16_t result = 0;
        while (*number) {
                char    c     = *number;
                int32_t digit = -1;
                if (c >= '0' && c <= '9')
                        digit = c - '0';
                else if (c >= 'a' && c <= 'z')
                        digit = c - 'a' + 10;
                else if (c >= 'A' && c <= 'Z')
                        digit = c - 'A' + 10;
                else
                        break;

                if (digit >= base) break;

                result = result * base + digit;
                number++;
        }

        return negative ? -result : result;
}

uint16_t parse_unsigned(const char *number, uint8_t base) {
        if (!number || base < 2 || base > 36) return 0;

        while (*number == ' ' || *number == '\t') number++;

        uint16_t result = 0;
        while (*number) {
                char    c     = *number;
                int32_t digit = -1;
                if (c >= '0' && c <= '9')
                        digit = c - '0';
                else if (c >= 'a' && c <= 'z')
                        digit = c - 'a' + 10;
                else if (c >= 'A' && c <= 'Z')
                        digit = c - 'A' + 10;
                else
                        break;

                if (digit >= base) break;

                result = result * base + digit;
                number++;
        }

        return result;
}


bool is_whitespace(uint8_t c) {
        return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') || (c == '\v') || (c == '\f');
}

bool is_alphabetic(uint8_t c) {
        return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

bool is_digit(uint8_t c) {
        return (c >= '0' && c <= '9');
}

bool is_alphanumeric(uint8_t c) {
        return is_alphabetic(c) || is_digit(c);
}

bool is_xdigit(uint8_t c) {
        return is_digit(c) || ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'));
}

bool is_cntrl(uint8_t c) {
        return (c < 32) || (c == 127);
}

bool is_punctuation(uint8_t c) {
        if (c < 32 || c > 126) return false;
        if (is_alphanumeric(c) || is_whitespace(c)) return false;
        return true;
}

bool is_upper(uint8_t c) {
        return (c >= 'A' && c <= 'Z');
}

bool is_lower(uint8_t c) {
        return (c >= 'a' && c <= 'z');
}


/* Helper: check if a character is in a given set */
static int32_t is_in_set(char c, const char *set) {
        while (*set) {
                if (c == *set) return 1;
                set++;
        }
        return 0;
}

static char to_lower(char c) {
        if (c >= 'A' && c <= 'Z') return c + ('a' - 'A');
        return c;
}

int16_t string_first_index_of(const char *str, char c) {
        if (!str) return -1;
        for (int16_t i = 0; str[i]; i++) {
                if (str[i] == c) return i;
        }
        return -1;
}

int16_t string_last_index_of(const char *str, char c) {
        if (!str) return -1;
        int16_t last = -1;
        for (int16_t i = 0; str[i]; i++) {
                if (str[i] == c) last = i;
        }
        return last;
}

int16_t string_first_index_of_none(const char *str, const char *set) {
        if (!str || !set) return -1;
        for (int16_t i = 0; str[i]; i++) {
                if (!is_in_set(str[i], set)) return i;
        }
        return -1;
}

int16_t string_last_index_of_none(const char *str, const char *set) {
        if (!str || !set) return -1;
        int16_t last = -1;
        for (int16_t i = 0; str[i]; i++) {
                if (!is_in_set(str[i], set)) last = i;
        }
        return last;
}

char *string_search_substring(const char *str, const char *substr) {
        if (!str || !substr) return NULL;
        if (!*substr) return (char *)str; // empty substring is found at start
        for (int32_t i = 0; str[i]; i++) {
                int32_t j = 0;
                while (str[i + j] && substr[j] && str[i + j] == substr[j]) j++;
                if (!substr[j]) return (char *)(str + i);
        }
        return NULL;
}

char *string_search_byte(const char *str, char c) {
        if (!str) return NULL;
        while (*str) {
                if (*str == c) return (char *)str;
                str++;
        }
        return NULL;
}

int16_t string_contains(const char *str, const char *substr) {
        char *pos = string_search_substring(str, substr);
        if (pos) return (int16_t)(pos - str);
        return -1;
}

int16_t string_concat(char *dest, const char *src) {
        if (!dest || !src) return -1;
        int16_t i = 0;
        while (dest[i]) i++;
        int16_t j = 0;
        while (src[j]) {
                dest[i++] = src[j++];
        }
        dest[i] = '\0';
        return i;
}

int16_t string_copy(char *dest, const char *src) {
        if (!dest || !src) return -1;
        int16_t i = 0;
        while (src[i]) {
                dest[i] = src[i];
                i++;
        }
        dest[i] = '\0';
        return i;
}

int16_t string_reverse(char *str) {
        if (!str) return -1;
        int16_t len = 0;
        while (str[len]) len++;
        for (int16_t i = 0; i < len / 2; i++) {
                char tmp         = str[i];
                str[i]           = str[len - 1 - i];
                str[len - 1 - i] = tmp;
        }
        return len;
}

int16_t string_compare(const char *s1, const char *s2) {
        if (!s1 || !s2) return (s1 == s2) ? 0 : (s1 ? 1 : -1);
        while (*s1 && (*s1 == *s2)) {
                s1++;
                s2++;
        }
        return (int16_t)((uint8_t)*s1 - (uint8_t)*s2);
}

int16_t string_case_compare(const char *s1, const char *s2) {
        if (!s1 || !s2) return (s1 == s2) ? 0 : (s1 ? 1 : -1);
        while (*s1 && *s2) {
                char c1 = to_lower(*s1);
                char c2 = to_lower(*s2);
                if (c1 != c2) return (int16_t)(c1 - c2);
                s1++;
                s2++;
        }
        return (int16_t)(to_lower(*s1) - to_lower(*s2));
}

int16_t string_starts_with(const char *str, const char *prefix) {
        if (!str || !prefix) return 0;
        while (*prefix) {
                if (*str != *prefix) return 0;
                str++;
                prefix++;
        }
        return 1;
}

int16_t string_ends_with(const char *str, const char *suffix) {
        if (!str || !suffix) return 0;
        int16_t str_len = 0, suffix_len = 0;
        while (str[str_len]) str_len++;
        while (suffix[suffix_len]) suffix_len++;
        if (suffix_len > str_len) return 0;
        for (int16_t i = 0; i < suffix_len; i++) {
                if (str[str_len - suffix_len + i] != suffix[i]) return 0;
        }
        return 1;
}

int16_t string_count(const char *str, char ch) {
        if (!str) return 0;
        int16_t count = 0;
        for (int16_t i = 0; str[i]; i++) {
                if (str[i] == ch) count++;
        }
        return count;
}


int16_t string_spn(const char *s, const char *accept) {
        if (!s || !accept) return 0;
        int16_t count = 0;
        while (s[count] && is_in_set(s[count], accept)) count++;
        return count;
}

int16_t string_cspn(const char *s, const char *reject) {
        if (!s || !reject) return 0;
        int16_t count = 0;
        while (s[count] && !is_in_set(s[count], reject)) count++;
        return count;
}

char *string_tokenize(char *str, const char *delim) {
        static char *static_str = NULL;
        if (str) static_str = str;
        if (!static_str) return NULL;

        while (*static_str && is_in_set(*static_str, delim)) static_str++;

        if (!*static_str) {
                static_str = NULL;
                return NULL;
        }

        char *token = static_str;

        while (*static_str && !is_in_set(*static_str, delim)) static_str++;

        if (*static_str) {
                *static_str = '\0';
                static_str++;
        }
        return token;
}

int16_t string_length(const char *s) {
        if (!s) return 0;
        uint16_t i = 0;
        for (; s[i]; i++)
                ;
        return i;
}



void uart_putchar(uint8_t c) {
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = c;
}

uint8_t uart_getchar() {
        loop_until_bit_is_set(UCSR0A, RXC0);
        return UDR0;
}

int16_t readline_echo_back(char *buffer, size_t busize) {
        static char line[MAX_LINE_SIZE];
        uint16_t    pos = 0;

        while (1) {
                char c = uart_getchar();


                if (c == '\n' || c == '\r') {
                        uart_putchar('\r');
                        uart_putchar('\n');
                        break;
                }


                if (c == '\b' || c == 127) {
                        if (pos > 0) {
                                pos--;

                                uart_putchar('\b');
                                uart_putchar(' ');
                                uart_putchar('\b');
                        }
                        continue;
                }


                if (pos < (MAX_LINE_SIZE - 1) && pos < busize) {
                        line[pos++] = c;
                        uart_putchar(c);
                }
        }
        line[pos] = '\0';
        string_copy(buffer, line);
        buffer[pos] = '\0';
        return pos;
}

int16_t clear_line(uint16_t count) {

        uart_putchar('\r');
        for (uint16_t i = 0; i < count; i++) {
                uart_putchar(' ');
        }
        uart_putchar('\r');
        return 0;
}

bool is_print(uint8_t c) {
        return (c >= 32 && c <= 126);
}
