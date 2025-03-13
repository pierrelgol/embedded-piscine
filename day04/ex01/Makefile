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
# Target MCU & Clock Speed
MCU        := atmega328p
F_CPU      ?= 16000000UL 
# Compilation Flags
CFLAGS     := -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra -MMD -MP
LDFLAGS    := -mmcu=$(MCU) 
# Directories
SRC_DIR    := src
BUILD_DIR  := .build
# Source & Object Files
SRC        := $(wildcard $(SRC_DIR)/*.c)
OBJ        := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
# Output Binary
TARGET     := firmware
# AVRDUDE Config
PROGRAMMER := arduino
PORT       := /dev/ttyUSB0
BAUD       := 115200
# Verbose Mode
V ?= 0
ifeq ($(V),1)
Q :=
else
Q := @
endif
# Build Targets
all: hex flash
hex: $(TARGET).hex
$(TARGET).hex: $(TARGET).elf
	@echo " [HEX]  $@"
	$(Q)$(OBJCOPY) -j .text -j .data -O ihex $< $@
$(TARGET).elf: $(OBJ)
	@echo " [LINK] $@"
	$(Q)$(CC) $(LDFLAGS) $^ -o $@
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo " [CC]   $<"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@
$(BUILD_DIR):
	@echo " [MKDIR] $(BUILD_DIR)"
	$(Q)mkdir -p $(BUILD_DIR)
flash: $(TARGET).hex
	@echo " [FLASH] $(TARGET).hex -> MCU"
	$(Q)$(AVRDUDE) -v -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -D -U flash:w:$<:i
clean:
	@echo " [CLEAN]"
	$(Q)rm -f $(TARGET).hex $(TARGET).elf $(OBJ) $(OBJ:.o=.d)
fclean: clean
	@echo " [FCLEAN] Removing build directory"
	$(Q)rm -rf $(BUILD_DIR)
re: fclean all
-include $(OBJ:.o=.d)
