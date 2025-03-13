#include "hal.h"
#include <avr/delay.h>
#include <avr/interrupt.h>

// (1) = https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

void uart_tx(char c) {
        // here we use a loop to wait for empty transmit buffer
        while ((!(UCSR0A & (1 << UDRE0)))) {
        }
        //
        // UDR0 is a read/write register
        //
        UDR0 = c;        // we load our data into the transmit register
        _delay_ms(1000); // and we sleep for 1s
        // to test use :
        //
        // screen /dev/ttyUSB0 115200
        //
        // to quit :
        //
        // Ctrl-a + Shift-K + Y
        //
        // note that rx/tx can be visualized with the led.
        // because after succesfull transmition a flag is set in TXC0.
}

int main(void) {

        // RX (PD0) as input
        DDRD &= ~(1 << PD0);
        PORTD &= ~(1 << PD0);

        // TX (PD1) as output
        DDRD |= (1 << PD1);
        PORTD &= ~(1 << PD1);


        // The uart UBRR0H and UBRROL are the high/low register for the UART
        // module they are meant to configure the baudrate for the transmitions
        // as described in (1) p144 Figure 19-1 USART Block Diagram.
        // we can see that UBRRn [H:L] is directly connected to the
        // Baud Rate Generator Which configures the Clock Generator.
        // For the Operating Mode which is Asynchronous normal mode.
        // The Baud = (fOSC) / (16 * (UBBRn + 1))
        //          = (16 000 000) / (16 * 115201)
        //          = 8.68
        //          = 8
        UBRR0H = (uint8_t)(8 >> 8); // clear 8 high bits >> 8 : 0000 0000
        UBRR0L = (uint8_t)8;        // set  8 in the low bits : 0000 1000


        // From the documentation (1).
        //
        // For UCSR0A:
        //
        // [RXC0 ] (RX Complete)        : Set when a new byte has arrived.
        // [TXC0 ] (TX Complete)        : Set when the transmission of a byte finishes (including all bits).
        // [UDRE0] (Data Register Empty): Set when ready to accept new data.
        // [U2X0 ] (Double Speed Mode)  : Set for higher baud rates (halves divider).
        // [FE0  ] (Frame Error)
        // [DOR0 ] (Data OverRun)
        // [UPE0 ] (Parity Error)       : Errors during reception.



        // From the documentation (1).
        //
        // For UCSR0C:
        //
        // [UCSZ01:0] (Data Bits)  : Typically set to 8 bits ((1<<UCSZ01)|(1<<UCSZ00)).
        // [UPM01:0 ] (Parity Mode): 00 = Disabled  , 10 = Even parity , 11 = Odd parity
        // [USBS0   ] (Stop Bits)  :  0 = 1 stop bit,  1 = 2 stop bits
        //
        // To configure the protocol frame transmition we refer to (1) page 162
        // Table 19-7 UCSZn Bits Settings, as shown in the table by setting
        // the UCSZ01 and the UCSZ00 to 1 we enable 8-bit character size.
        UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

        // From the documentation (1).
        //
        // "Bit 4 – RXENn: Receiver Enable n
        //  Writing this bit to one enables the USART receiver. The receiver will override normal port operation for the RxDn pin when
        //  enabled. Disabling the receiver will flush the receive buffer invalidating the FEn, DORn, and UPEn flags."
        //
        // "Bit 3 – TXENn: Transmitter Enable n
        //  Writing this bit to one enables the USART transmitter. The transmitter will override normal port operation for the TxDn pin
        //  when enabled. The disabling of the transmitter (writing TXENn to zero) will not become effective until ongoing and pending
        //  transmissions are completed, i.e., when the transmit shift register and transmit buffer register do not contain data to be
        //  transmitted. When disabled, the transmitter will no longer override the TxDn port."
        //
        // So here we simply enable Input/Output for the UART
        UCSR0B = (1 << TXEN0) | (1 << RXEN0);


        loop {
                uart_tx('z');
        }
}
