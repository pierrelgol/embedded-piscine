#include "libc.h"
#include <avr/io.h>
#include <avr/eeprom.h>

#define EEPROM_SIZE    1024
#define ArraySize(ptr) (sizeof(ptr) / sizeof(ptr[0]))

void print_has_string(uint16_t address) {
        uart_printf(" | ");
        for (uint16_t i = address; i < address + 16; i++) {
                uint8_t val = eeprom_read_byte((uint8_t*)(i));
                if (is_print(val)) {
                        uart_putchar(val);
                } else {
                        uart_putchar('.');
                }
        }
        uart_printf("\r\n");
}

void eeprom_hexdump(uint16_t highlight_addr) {
        for (uint16_t i = 0; i < EEPROM_SIZE; i += 16) {
                uart_printf("%08X: ", i);

                for (uint8_t j = 0; j < 16; j += 2) {
                        uint16_t addr = i + j;
                        if (addr >= EEPROM_SIZE) break;

                        uint8_t val = eeprom_read_byte((uint8_t*)addr);
                        if (addr == highlight_addr) {
                                uart_printf("\033[31m"); // Red color
                        }
                        uart_printf("%02X", val);
                        if (addr == highlight_addr) {
                                uart_printf("\033[0m"); // Reset color
                        }

                        val = eeprom_read_byte((uint8_t*)addr + 1);
                        if (addr + 1 == highlight_addr) {
                                uart_printf("\033[31m"); // Red color
                        }
                        uart_printf("%02X ", val);
                        if (addr + 1 == highlight_addr) {
                                uart_printf("\033[0m"); // Reset color
                        }
                }
                print_has_string(i);
        }
}



int main() {
        UartOption opts = {.baud_rate = 8, .transmit = true, .receive = true};
        uart_init(&opts);

        uart_printf("EEPROM Write & Dump Tool\r\n");

        while (1) {
                uart_printf("Enter address & value (hex): ");

                char input[16];
                readline_echo_back(input, sizeof(input));

                char* space = string_search_byte(input, ' ');
                if (!space) {
                        uart_printf("Invalid format. Use: ADDR VALUE\r\n");
                        continue;
                }

                *space        = '\0'; // Split the string
                int16_t addr  = parse_number(input, 16);
                int16_t value = parse_number(space + 1, 16);

                if (addr == -1 || value == -1) {
                        uart_printf("invalid address");
                }

                if (addr >= EEPROM_SIZE || value > 0xFF) {
                        uart_printf("Address out of range or invalid value.\r\n");
                        continue;
                }

                uint8_t current_value = eeprom_read_byte((uint8_t*)addr);
                if (current_value != value) {
                        eeprom_write_byte((uint8_t*)addr, (uint8_t)value);
                        uart_printf("Value written.\r\n");
                } else {
                        uart_printf("Value unchanged.\r\n");
                }
                eeprom_hexdump(addr);
        }
}
