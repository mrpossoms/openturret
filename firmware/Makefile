CC=avr-gcc
TARGET=attiny24

all:
	$(CC) -mmcu=$(TARGET) main.c -o ot.hex

.PHONY: flash
flash: all
	avrdude -pattiny24 -cxplainedpro -e -U flash:w:ot.hex
