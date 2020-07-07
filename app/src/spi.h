#ifndef _OT_SPI_H_
#define _OT_SPI_H_

#include <inttypes.h>

void spi_config(int fd);

void spi_transfer(int fd, uint8_t ctrl);

#endif