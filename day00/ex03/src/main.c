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
#include <avr/delay.h>
#include <stdint.h>

int main(void) {

        DDRB |= (1 << PB0);  // we start by setting PORTB pin 0 to ouput mode
        DDRD &= ~(1 << PD2); // we then set PORTD PIN2 to input mode
        PORTD |= (1 << PD2); // we then enable the internal resistor pull

        // we save the current state of the button
        uint8_t prev_state = PIND & (1 << PD2);
        while (1) {
                // we pull the current state of the button
                uint8_t current_state = PIND & (1 << PD2);
                // if the prev state was on and the current state is off
                // this mean that the button was pressed and now has been
                // released
                if ((prev_state != 0) && (current_state == 0)) {
                        PORTB ^= (1 << PB0); // we then toggle the led
                        _delay_ms(200);      // and put some debouncing delay
                }
                // we update the previous state
                prev_state = current_state;
        }
        return 0;
}
