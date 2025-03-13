#include "hal.h"
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>

#define I2C_ADDRESS_AHT20      (0x38 << 1)
#define MEASUREMENT_CMD        0xAC
#define DELAY_BETWEEN_READS_MS 1000

#define i2c_status()           (TWSR & 0xF8)

u8 *i2c_return_code_desc(u8 status_code) {
        if (status_code == TW_START)
                return (u8 *)("START acknowledge.");
        else if (status_code == TW_REP_START)
                return (u8 *)("REPEATED START acknowledge.");
        else if (status_code == TW_MT_SLA_ACK)
                return (u8 *)("Master Transmitter: Slave ACK");
        else if (status_code == TW_MT_SLA_NACK)
                return (u8 *)("Master Transmitter : Slave NACK");
        else if (status_code == TW_MT_DATA_ACK)
                return (u8 *)("Master Transmitter : Data ACK");
        else if (status_code == TW_MT_DATA_NACK)
                return (u8 *)("Master Transmitter: Data NACK");
        else if (status_code == TW_MR_SLA_ACK)
                return (u8 *)("Master Receiver : Slave ACK");
        else if (status_code == TW_MR_SLA_NACK)
                return (u8 *)("Master Receiver : Slave NACK");
        else if (status_code == TW_MR_DATA_ACK)
                return (u8 *)("Master Receiver : Data ACK");
        else if (status_code == TW_MR_DATA_NACK)
                return (u8 *)("Master Receiver : Data NACK");
        else if (status_code == TW_MT_ARB_LOST || status_code == TW_MR_ARB_LOST)
                return (u8 *)("Arbitration Lost");
        else if (status_code == TW_ST_SLA_ACK)
                return (u8 *)("Slave Transmitter : Slave ACK");
        else if (status_code == TW_ST_ARB_LOST_SLA_ACK)
                return (u8 *)("Arbitration Lost in SLA+R/W, Slave ACK");
        else if (status_code == TW_ST_DATA_ACK)
                return (u8 *)("Slave Transmitter : Data ACK");
        else if (status_code == TW_ST_DATA_NACK)
                return (u8 *)("Slave Transmitter : Data NACK");
        else if (status_code == TW_ST_LAST_DATA)
                return (u8 *)("Slave Transmitter : Last Data");
        else if (status_code == TW_SR_SLA_ACK)
                return (u8 *)("Slave Receiver : Slave ACK");
        else if (status_code == TW_SR_ARB_LOST_SLA_ACK)
                return (u8 *)("Arbitration Lost in SLA+R/W, Slave ACK");
        else if (status_code == TW_SR_GCALL_ACK)
                return (u8 *)("General Call : Slave ACK");
        else if (status_code == TW_SR_ARB_LOST_GCALL_ACK)
                return (u8 *)("Arbitration Lost in General Call, Slave ACK");
        else if (status_code == TW_SR_DATA_ACK)
                return (u8 *)("Slave Receiver : Data ACK");
        else if (status_code == TW_SR_DATA_NACK)
                return (u8 *)("Slave Receiver : Data NACK");
        else if (status_code == TW_SR_GCALL_DATA_ACK)
                return (u8 *)("General Call : Data ACK");
        else if (status_code == TW_SR_GCALL_DATA_NACK)
                return (u8 *)("General Call : Data NACK");
        else if (status_code == TW_SR_STOP)
                return (u8 *)("Slave Receiver : STOP received");
        else if (status_code == TW_NO_INFO)
                return (u8 *)("No state information available");
        else if (status_code == TW_BUS_ERROR)
                return (u8 *)("Bus Error");
        else
                return (u8 *)("Unknown Status Code");
}

// uart initialization
void uart_init() {
        UBRR0H = 0;
        UBRR0L = 8; // baud rate for 115200 with 16mhz clock
        UCSR0B = (1 << TXEN0);
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// uart putu8 function
void uart_putchar(u8 c) {
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UDR0 = c;
}


// print a string with cr+lf at the end
void uart_println(const u8 *str) {
        if (!str) return;
        while (*str) {
                uart_putchar(*str++);
        }
        uart_putchar('\r');
        uart_putchar('\n');
}

// function to print a byte in hex format via uart
void print_hex_value(u8 c) {
        u8 hex_chars[] = "0123456789ABCDEF";
        uart_putchar(hex_chars[(c >> 4) & 0x0F]); // print high nibble
        uart_putchar(hex_chars[c & 0x0F]);        // print low nibble
        uart_putchar(' ');                        // space for readability
}

void i2c_debug() {
#ifdef DEBUG
        uart_println(i2c_return_code_desc(i2c_status()));
#endif
}

// function to send data on the i2c bus
void i2c_write(u8 data) {
        TWDR = data;                       // load data into twdr register
        TWCR = (1 << TWEN) | (1 << TWINT); // start transmission
        while (!(TWCR & (1 << TWINT)));    // wait for completion
        i2c_debug();
}

// function to read a byte from the i2c bus with ack
u8 i2c_read_ack(void) {
        TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA); // enable ack
        while (!(TWCR & (1 << TWINT)));                  // wait for completion
        i2c_debug();
        return TWDR;
}

// function to read a byte from the i2c bus with nack
u8 i2c_read_nack(void) {
        TWCR = (1 << TWEN) | (1 << TWINT); // no ack after last byte
        while (!(TWCR & (1 << TWINT)));    // wait for completion
        i2c_debug();
        return TWDR;
}

// function to send start condition
void i2c_start() {
        TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
        while (!(TWCR & (1 << TWINT)));
        i2c_debug();
}

// function to send stop condition
void i2c_stop(void) {
        TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
        i2c_debug();
}

// function to read aht20 sensor data
void i2c_read(void) {
        u8 data[7]; // aht20 measurement returns 7 bytes

        i2c_start();
        i2c_write(I2C_ADDRESS_AHT20 | 0x01); // read mode
        for (u8 i = 0; i < 6; i++) {
                data[i] = i2c_read_ack(); // read first 6 bytes with ack
        }
        data[6] = i2c_read_nack(); // last byte with nack
        i2c_stop();

        // print the received data
        for (u8 i = 0; i < 7; i++) {
                print_hex_value(data[i]);
        }
}

// function to initialize i2c
void i2c_init() {
        TWSR = 0x00;                                // prescaler = 1
        TWBR = ((16000000UL / 100000UL) - 16) >> 1; // 100khz i2c clock
        TWCR = (1 << TWEN);
        i2c_debug();
}



int main() {
        uart_init();
        i2c_init();

        while (1) {
                // trigger measurement
                i2c_start();
                i2c_write(I2C_ADDRESS_AHT20);
                i2c_write(MEASUREMENT_CMD);
                i2c_write(0x33);
                i2c_write(0x00);
                i2c_stop();

                _delay_ms(DELAY_BETWEEN_READS_MS); // wait for measurement
                i2c_read();
                uart_putchar('\r');
                uart_putchar('\n');
                _delay_ms(DELAY_BETWEEN_READS_MS);
        }
}
