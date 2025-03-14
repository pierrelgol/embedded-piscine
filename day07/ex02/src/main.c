
#include "libc.h"
#include <avr/eeprom.h>

#define EEPROM_SIZE    1024
#define MAGIC_NUMBER   0x7F
#define MAX_KEY_SIZE   32
#define MAX_VALUE_SIZE 32
#define MAX_ENTRY_SIZE (1 + MAX_KEY_SIZE + MAX_VALUE_SIZE)


uint16_t find_key(const char* key) {
        uint16_t addr = 0;
        while (addr + MAX_ENTRY_SIZE <= EEPROM_SIZE) {
                uint8_t magic = eeprom_read_byte((uint8_t*)addr);
                if (magic == MAGIC_NUMBER) {
                        char stored_key[MAX_KEY_SIZE];
                        eeprom_read_block(stored_key, (void*)(addr + 1), MAX_KEY_SIZE);
                        if (string_compare(stored_key, key) == 0) return addr;
                }
                addr += MAX_ENTRY_SIZE;
        }
        return 0xFFFF;
}


uint16_t find_free_space() {
        uint16_t addr = 0;
        while (addr + MAX_ENTRY_SIZE <= EEPROM_SIZE) {
                if (eeprom_read_byte((uint8_t*)addr) == 0xFF) return addr;
                addr += MAX_ENTRY_SIZE;
        }
        return 0xFFFF;
}


void read_command(const char* key) {
        uint16_t addr = find_key(key);
        if (addr == 0xFFFF) {
                uart_putline("empty");
                return;
        }
        char value[MAX_VALUE_SIZE];
        eeprom_read_block(value, (void*)(addr + 1 + MAX_KEY_SIZE), MAX_VALUE_SIZE);
        uart_printf("%s\r\n", value);
}


void write_command(const char* key, const char* value) {
        if (find_key(key) != 0xFFFF) {
                uart_putline("already exists");
                return;
        }
        uint16_t addr = find_free_space();
        if (addr == 0xFFFF) {
                uart_putline("no space left");
                return;
        }
        eeprom_write_byte((uint8_t*)addr, MAGIC_NUMBER);
        eeprom_write_block(key, (void*)(addr + 1), MAX_KEY_SIZE);
        eeprom_write_block(value, (void*)(addr + 1 + MAX_KEY_SIZE), MAX_VALUE_SIZE);
        uart_printf("0x%04X\r\n", addr);
}


void forget_command(const char* key) {
        uint16_t addr = find_key(key);
        if (addr == 0xFFFF) {
                uart_putline("not found");
                return;
        }
        eeprom_write_byte((uint8_t*)addr, 0x00);
        uart_putline("deleted");
}

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

void print_hexdump() {
        for (uint16_t i = 0; i < EEPROM_SIZE; i += 16) {
                uart_printf("0x%08X: ", i);
                for (uint8_t j = 0; j < 16; j += 2) {
                        uint8_t val = eeprom_read_byte((uint8_t*)(i + j));
                        uart_printf("%02X", val);
                        val = eeprom_read_byte((uint8_t*)(i + j + 1));
                        uart_printf("%02X ", val);
                }
                print_has_string(i);
        }
}

void clear_command(void) {
        for (uint16_t i = 0; i < EEPROM_SIZE; i++) {
                eeprom_write_byte((uint8_t*)i, 0xFF);
        }
        uart_putline("EEPROM cleared");
}

int main() {
        UartOption opts = {.baud_rate = 8, .transmit = true, .receive = true};
        uart_init(&opts);
        uart_putline("EEPROM Key-Value Store");

        while (1) {
                uart_printf("> ");
                char input[72];
                readline_echo_back(input, sizeof(input));

                char* cmd = string_tokenize(input, " ");
                if (!cmd) continue;

                if (string_compare(cmd, "READ") == 0) {
                        char* key = string_tokenize(NULL, " ");
                        if (key)
                                read_command(key);
                        else
                                uart_putline("Usage: READ <key>");
                } else if (string_compare(cmd, "WRITE") == 0) {
                        char* key   = string_tokenize(NULL, " ");
                        char* value = string_tokenize(NULL, " ");
                        if (key && value)
                                write_command(key, value);
                        else
                                uart_putline("Usage: WRITE <key> <value>");
                } else if (string_compare(cmd, "FORGET") == 0) {
                        char* key = string_tokenize(NULL, " ");
                        if (key)
                                forget_command(key);
                        else
                                uart_putline("Usage: FORGET <key>");
                } else if (string_compare(cmd, "PRINT") == 0) {
                        print_hexdump();
                } else if (string_compare(cmd, "CLEAR") == 0) {
                        clear_command();
                } else {
                        uart_putline("Unknown command.");
                }
                clear_line(string_length(input));
        }

        return 0;
}
