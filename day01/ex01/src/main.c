#include "hal.h"

int main(void) {

        Timer16Bit timer = TIMER1;
        DDRB |= (1 << PB1);

        (*timer.TCCRA) = ((0 << COM1A1) | (1 << COM1A0));
        (*timer.TCCRB) = ((0 << WGM13) | (1 << WGM12) | (1 << CS12) | (0 << CS11) | (1 << CS10));
        (*timer.OCR_A) = F_CPU / 1024;


        loop {
        }
}
