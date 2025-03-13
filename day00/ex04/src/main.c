/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollivie <plgol.perso@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 13:42:07 by pollivie          #+#    #+#             */
/*   Updated: 2025/03/03 13:42:07 by pollivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <avr/io.h>
#include <util/delay.h>

int main(void) {
        // We start by setting the pins of PORTB (1,2,3,4) to output
        DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);
        // We make sure to clear their values
        PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4));

        // we then configure PORTD to read from sw1 sw2
        DDRD &= ~((1 << PD2) | (1 << PD4));
        // we configure the polling of the resistor
        PORTD |= (1 << PD2) | (1 << PD4);



        // we set our original value
        uint8_t value = 0;
        // poll the state of sw1
        uint8_t prev_sw1 = PIND & (1 << PD2);
        // poll the state of sw2
        uint8_t prev_sw2 = PIND & (1 << PD4);

        while (1) {

                // poll their current state
                uint8_t curr_sw1 = PIND & (1 << PD2);
                uint8_t curr_sw2 = PIND & (1 << PD4);

                // if sw1 was pressed and now isnt
                if ((prev_sw1 != 0) && (curr_sw1 == 0)) {
                        value++; // we increment
                        _delay_ms(150);
                }

                // if sw2 was pressend and now isnt we decremetn
                if ((prev_sw2 != 0) && (curr_sw2 == 0)) {
                        value--;
                        _delay_ms(150);
                }

                // we update the states;
                prev_sw1 = curr_sw1;
                prev_sw2 = curr_sw2;

                // we update the values of the led
                uint8_t led_val = 0;
                led_val |= (value & 0x07);
                if (value & 0x08) { // overflow protection
                        led_val |= (1 << PB4);
                }
                // we then write to the leds
                PORTB = (PORTB & ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4))) | led_val;

                // and add a small delay
                _delay_ms(10);
        }

        return 0;
}
