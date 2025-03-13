#include "hal.h"
#include <avr/io.h>
#include <util/twi.h>

#define i2c_status() (TWSR & 0xF8)

char *i2c_return_code_desc(uint8_t status_code) {
        if (status_code == TW_START)
                return ("START acknowledge.");
        else if (status_code == TW_REP_START)
                return ("REPEATED START acknowledge.");
        else if (status_code == TW_MT_SLA_ACK)
                return ("Master Transmitter: Slave ACK");
        else if (status_code == TW_MT_SLA_NACK)
                return ("Master Transmitter : Slave NACK");
        else if (status_code == TW_MT_DATA_ACK)
                return ("Master Transmitter : Data ACK");
        else if (status_code == TW_MT_DATA_NACK)
                return ("Master Transmitter: Data NACK");
        else if (status_code == TW_MR_SLA_ACK)
                return ("Master Receiver : Slave ACK");
        else if (status_code == TW_MR_SLA_NACK)
                return ("Master Receiver : Slave NACK");
        else if (status_code == TW_MR_DATA_ACK)
                return ("Master Receiver : Data ACK");
        else if (status_code == TW_MR_DATA_NACK)
                return ("Master Receiver : Data NACK");
        else if (status_code == TW_MT_ARB_LOST || status_code == TW_MR_ARB_LOST)
                return ("Arbitration Lost");
        else if (status_code == TW_ST_SLA_ACK)
                return ("Slave Transmitter : Slave ACK");
        else if (status_code == TW_ST_ARB_LOST_SLA_ACK)
                return ("Arbitration Lost in SLA+R/W, Slave ACK");
        else if (status_code == TW_ST_DATA_ACK)
                return ("Slave Transmitter : Data ACK");
        else if (status_code == TW_ST_DATA_NACK)
                return ("Slave Transmitter : Data NACK");
        else if (status_code == TW_ST_LAST_DATA)
                return ("Slave Transmitter : Last Data");
        else if (status_code == TW_SR_SLA_ACK)
                return ("Slave Receiver : Slave ACK");
        else if (status_code == TW_SR_ARB_LOST_SLA_ACK)
                return ("Arbitration Lost in SLA+R/W, Slave ACK");
        else if (status_code == TW_SR_GCALL_ACK)
                return ("General Call : Slave ACK");
        else if (status_code == TW_SR_ARB_LOST_GCALL_ACK)
                return ("Arbitration Lost in General Call, Slave ACK");
        else if (status_code == TW_SR_DATA_ACK)
                return ("Slave Receiver : Data ACK");
        else if (status_code == TW_SR_DATA_NACK)
                return ("Slave Receiver : Data NACK");
        else if (status_code == TW_SR_GCALL_DATA_ACK)
                return ("General Call : Data ACK");
        else if (status_code == TW_SR_GCALL_DATA_NACK)
                return ("General Call : Data NACK");
        else if (status_code == TW_SR_STOP)
                return ("Slave Receiver : STOP received");
        else if (status_code == TW_NO_INFO)
                return ("No state information available");
        else if (status_code == TW_BUS_ERROR)
                return ("Bus Error");
        else
                return ("Unknown Status Code");
}

// uart Initialization
void uart_init() {
        UBRR0H = 0;
        UBRR0L = 8; // baud rate for 115200 with 16MHz clock
        UCSR0B = (1 << TXEN0);
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// uart putchar function
void uart_putchar(uint8_t c) {
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = c;
}


// print a string with CR+LF at the end
void uart_println(const char *str) {
        if (!str) return;
        while (*str) {
                uart_putchar(*str++);
        }
        uart_putchar('\r');
        uart_putchar('\n');
}

// i2c Initialization
void i2c_init() {
        TWSR = 0x00;                                // prescaler = 1
        TWBR = ((16000000UL / 100000UL) - 16) >> 1; // 100khz I2C frequency
        TWCR = (1 << TWEN);
}

void i2c_debug() {
#ifdef DEBUG
        uart_println(i2c_return_code_desc(i2c_status()));
#endif
}

void i2c_start() {
        // transmit START condition
        TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
        while (!(TWCR & (1 << TWINT))); // wait for completion
        i2c_debug();

        // send device address (AHT20 default 0x38)
        TWDR = (0x38 << 1); // write mode
        TWCR = (1 << TWEN) | (1 << TWINT);
        while (!(TWCR & (1 << TWINT))); // wait for completion
        i2c_debug();
}

void i2c_stop(void) {
        // transmit STOP condition
        TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
}



int main() {
        uart_init();
        i2c_init();
        i2c_start();
        i2c_stop();

        loop {
        }
}
