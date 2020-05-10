CC=avr-g++
TARGET=atmega256rfr2

all:
	$(CC) -mmcu=$(TARGET) pwm-demo.c -o pwm-demo.hex

.PHONY: flash
flash: all
	avrdude -patmega256rfr2 -cxplainedpro -e -U flash:w:pwm-demo.hex
