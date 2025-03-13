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
#define loop for (;;)

static void turn_led_on_d1() {
        // The led requires PORTB see : elec42_pool-1.pdf

        // see https://docs.rs-online.com/f2bc/0900766b816cbd1f.pdf#G3.1186146
        // page 85 14.2.1 Configuring pin
        // In AVR each I/O is organized in group called Port (PORTA, PORTB...)
        // Each Port has 3 registers DDRx (Data Direction Register) Those registers
        // are used to configure the behavior of the given port
        // output = 1
        // input = 0
        // So Here we select DDRB (aka the DDR of Port B)
        // We use the constant PB0 (Port B pin 0) to create a bit mask
        // We then bitshift 1 << PB0  and or the value with DDRB
        // this creates the following change
        // DDRB before = 0000 0000
        // DDRB after the or of the mask =  0000 0001
        // This tells the chip that the DDRB port pin 0 will be used as output
        DDRB |= (1 << PB0);

        // see https://docs.rs-online.com/f2bc/0900766b816cbd1f.pdf#G3.1186146
        // page 13 1.1.3 Port B (PB7:0) XTAL1/XTAL2/TOSC1/TOSC2
        // Here we configure the value that we want to send through the pin 0
        // By turning on the first bit we set the value of the voltage to high
        // this is the equivalent of turning the current on effectively turning
        // on the LED connected to that pin
        PORTB |= (1 << PB0);
}


int main() {


        turn_led_on_d1();
        // Here we loop for ever to maintain the LED in it's state.
        loop {
        }
}
