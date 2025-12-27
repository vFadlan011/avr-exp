MCU = atmega328p
F_CPU = 16000000UL
BAUD = 115200

TARGET = shiftreg
SRCS = shiftreg.c anim.c

CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -I.

all: bin/$(TARGET).hex

bin/$(TARGET).elf: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o bin/$(TARGET).elf

bin/$(TARGET).hex: bin/$(TARGET).elf
	$(OBJCOPY) -O ihex bin/$(TARGET).elf bin/$(TARGET).hex

clean:
	rm -f bin/$(TARGET).elf bin/$(TARGET).hex

flash: bin/$(TARGET).hex
	avrdude -c usbasp -p $(MCU) -U flash:w:bin/$(TARGET).hex:i
