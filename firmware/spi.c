#include "spi.h"

#include <avr/io.h>
#include <util/twi.h>
#include <avr/interrupt.h>


static void (*spi_recv)(uint8_t);
static void (*spi_req)();


void spi_init()
{
	cli();
	USICR = (1 << USIWM0) | (1 << USICS1);
	sei();
}


void spi_set_callbacks(void (*recv)(uint8_t), void (*req)())
{
	cli();
	spi_recv = recv;
	spi_req  = req;
	sei();
}

