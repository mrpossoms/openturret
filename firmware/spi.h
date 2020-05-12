#ifndef _OT_SPI_H_
#define _OT_SPI_H_

#include <avr/interrupt.h>
#include <stdint.h>

void spi_init();
void spi_set_callbacks(void (*recv)(uint8_t), void (*req)());

#endif
