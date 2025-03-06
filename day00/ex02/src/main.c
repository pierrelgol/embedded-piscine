/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollivie <plgol.perso@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 10:09:11 by pollivie          #+#    #+#             */
/*   Updated: 2025/03/03 10:09:11 by pollivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>

#define DEBOUNCE_DELAY 10

static int isLedOn() {
        const uint8_t mask = (1 << PB0);
        return (PORTB & mask);
}

static void turnOn() {
        const uint8_t mask = (1 << PB0);
        if (!isLedOn()) {
                PORTB |= mask;
        }
}

static void turnOff() {
        const uint8_t mask = ~(1 << PB0);
        if (isLedOn()) {
                PORTB &= mask;
        }
}

static int isButtonPressed() {
        return (!(PIND & (1 << PD2)));
}

static void configurePortDAsInput() {
        DDRD &= ~(1 << PD2);
        PORTD |= (1 << PD2); // pull from PD2
}

static void configurePortBAsOutput() {
        // Configure PB0 as output.
        DDRB |= (1 << PB0);
}


static void sleepMs(double t) {
        _delay_ms(t);
}


int main(void) {

        configurePortDAsInput();
        configurePortBAsOutput();

        while (1) {
                if (isButtonPressed()) {
                        sleepMs(DEBOUNCE_DELAY);
                        if (isButtonPressed()) {
                                turnOn();
                        }
                } else {
                        turnOff();
                }
        }
        return 0;
}
