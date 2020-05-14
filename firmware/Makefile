CC=avr-gcc
TARGET=attiny24
CFLAGS+=-O1

all:
	$(CC) $(CFLAGS) -mmcu=$(TARGET) main.c spi.c -o ot.hex

.PHONY: flash clean
flash: all
	avrdude -p$(TARGET) -cusbtiny -e -U flash:w:ot.hex

clean:
	rm ot.hex
