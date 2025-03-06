/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hal.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pollivie <pollivie.student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 21:00:56 by pollivie          #+#    #+#             */
/*   Updated: 2025/03/03 21:00:56 by pollivie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HAL_H
#define HAL_H

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

#define loop for (;;)

typedef uint_least8_t  u8;
typedef int_least8_t   i8;
typedef uint_least16_t u16;
typedef int_least16_t  i16;
typedef uint_least32_t u32;
typedef int_least32_t  i32;
typedef uint_least64_t u64;
typedef int_least64_t  i64;
typedef uint_least16_t usize;
typedef int_least16_t  isize;

typedef volatile u8   *ptr8;
typedef volatile u16  *ptr16;
typedef volatile void *opaque;

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

typedef enum {
        LOW  = 0,
        HIGH = 1,
        ON   = 0,
        OFF  = 1
} State;

typedef enum {
        GPIO_PORTB = (uint16_t)&PORTB,
        GPIO_PORTC = (uint16_t)&PORTC,
        GPIO_PORTD = (uint16_t)&PORTD
} GpioPort;

typedef struct {
        volatile uint8_t *port;
        volatile uint8_t *ddr;
        volatile uint8_t *pin;
        u8                bit;
} GpioPin;


#define hal_mask8(bit)                   (1U << (bit))
#define hal_mask16(bit)                  (1UL << (bit))

#define hal_set_bit8(reg, bit)           ((reg) |= hal_mask8(bit))
#define hal_clear_bit8(reg, bit)         ((reg) &= ~hal_mask8(bit))
#define hal_toggle_bit8(reg, bit)        ((reg) ^= hal_mask8(bit))
#define hal_read_bit8(reg, bit)          (((reg) & hal_mask8(bit)) ? HIGH : LOW)
#define hal_write_bit8(reg, bit, state)  ((state) == HIGH ? hal_set_bit8(reg, bit) : hal_clear_bit8(reg, bit))
#define hal_is_bit8_set(reg, bit)        (((reg) & hal_mask8(bit)) != 0)
#define hal_is_bit8_unset(reg, bit)      (((reg) & hal_mask8(bit)) == 0)

#define hal_set_bit16(reg, bit)          ((reg) |= hal_mask16(bit))
#define hal_clear_bit16(reg, bit)        ((reg) &= ~hal_mask16(bit))
#define hal_toggle_bit16(reg, bit)       ((reg) ^= hal_mask16(bit))
#define hal_read_bit16(reg, bit)         (((reg) & hal_mask16(bit)) ? HIGH : LOW)
#define hal_write_bit16(reg, bit, state) ((state) == HIGH ? hal_set_bit16(reg, bit) : hal_clear_bit16(reg, bit))
#define hal_is_bit16_set(reg, bit)       (((reg) & hal_mask16(bit)) != 0)
#define hal_is_bit16_unset(reg, bit)     (((reg) & hal_mask16(bit)) == 0)

#define hal_or_reg8(reg, mask)           ((reg) |= (mask))
#define hal_and_reg8(reg, mask)          ((reg) &= (mask))
#define hal_xor_reg8(reg, mask)          ((reg) ^= (mask))
#define hal_not_reg8(reg)                ((reg) = ~(reg))

#define hal_or_reg16(reg, mask)          ((reg) |= (mask))
#define hal_and_reg16(reg, mask)         ((reg) &= (mask))
#define hal_xor_reg16(reg, mask)         ((reg) ^= (mask))
#define hal_not_reg16(reg)               ((reg) = ~(reg))

#define gpio_init(gpio, port_val, bit_val)                                                                                                                     \
        do {                                                                                                                                                   \
                (gpio)->port = (volatile uint8_t *)(port_val);                                                                                                 \
                (gpio)->ddr  = (gpio)->port - 1;                                                                                                               \
                (gpio)->pin  = (gpio)->port - 2;                                                                                                               \
                (gpio)->bit  = (bit_val);                                                                                                                      \
        } while (0)

#define gpio_set_output(gpio)   hal_set_bit8(*(gpio)->ddr, (gpio)->bit)
#define gpio_set_input(gpio)    hal_clear_bit8(*(gpio)->ddr, (gpio)->bit)
#define gpio_set(gpio)          hal_set_bit8(*(gpio)->port, (gpio)->bit)
#define gpio_clear(gpio)        hal_clear_bit8(*(gpio)->port, (gpio)->bit)
#define gpio_toggle(gpio)       hal_toggle_bit8(*(gpio)->port, (gpio)->bit)
#define gpio_read(gpio)         hal_read_bit8(*(gpio)->pin, (gpio)->bit)
#define gpio_write(gpio, state) hal_write_bit8(*(gpio)->port, (gpio)->bit, state)


typedef struct {
        volatile uint8_t *TCCRA;
        volatile uint8_t *TCCRB;
        volatile uint8_t *TCNT;
        volatile uint8_t *OCR_A;
        volatile uint8_t *OCR_B;
} Timer8Bit;

typedef struct {
        volatile uint8_t  *TCCRA;
        volatile uint8_t  *TCCRB;
        volatile uint8_t  *TCCRC;
        volatile uint16_t *TCNT;
        volatile uint16_t *ICR;
        volatile uint16_t *OCR_A;
        volatile uint16_t *OCR_B;
} Timer16Bit;

#define TIMER0 ((Timer8Bit){&TCCR0A, &TCCR0B, &TCNT0, &OCR0A, &OCR0B})
#define TIMER1 ((Timer16Bit){&TCCR1A, &TCCR1B, &TCCR1C, &TCNT1, &ICR1, &OCR1A, &OCR1B})
#define TIMER2 ((Timer8Bit){&TCCR2A, &TCCR2B, &TCNT2, &OCR2A, &OCR2B})

#define timer8_init(timer, mode, prescaler)                                                                                                                    \
        do {                                                                                                                                                   \
                *(timer).TCCRA = (mode);                                                                                                                       \
                *(timer).TCCRB = (prescaler);                                                                                                                  \
        } while (0)

#define timer16_init(timer, mode, prescaler)                                                                                                                   \
        do {                                                                                                                                                   \
                *(timer).TCCRA = (mode);                                                                                                                       \
                *(timer).TCCRB = (prescaler);                                                                                                                  \
                *(timer).TCCRC = 0;                                                                                                                            \
        } while (0)

#define timer_set_counter(timer, value)   (*(timer).TCNT = (value))
#define timer_set_compare_A(timer, value) (*(timer).OCR_A = (value))
#define timer_set_compare_B(timer, value) (*(timer).OCR_B = (value))
#define timer_start(timer, prescaler)     (*(timer).TCCRB |= (prescaler))
#define timer_stop(timer)                 (*(timer).TCCRB &= ~0x07)


#endif // HAL_H
