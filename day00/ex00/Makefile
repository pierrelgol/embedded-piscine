# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pollivie <plgol.perso@gmail.com>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/03 10:08:36 by pollivie          #+#    #+#              #
#    Updated: 2025/03/03 10:08:39 by pollivie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC         := avr-gcc
OBJCOPY    := avr-objcopy
AVRDUDE    := avrdude
MCU        := atmega328p
F_CPU      ?= 16000000UL
CFLAGS     := -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra -MMD -MP
SRC_DIR    := src
BUILD_DIR  := .build
SRC        := $(wildcard $(SRC_DIR)/*.c)
OBJ        := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))

all: hex flash

hex: main.hex

main.hex: main.bin
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

main.bin: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

flash: main.hex
	$(AVRDUDE) -v -p $(MCU) -c arduino -P /dev/ttyUSB0 -b 115200 -D -U flash:w:$<:i

clean:
	rm -f main.hex main.bin

fclean: clean
	rm -rf $(BUILD_DIR)

re: fclean all

-include $(OBJ:.o=.d)

