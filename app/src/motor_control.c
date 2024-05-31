#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include "motor_control.h"
#include "utils.h"
#include "spi.h"

static int SPI_FD;

int motor_control_init(const char* spi_dev_path)
{
	SPI_FD = open(spi_dev_path, O_RDWR);

	if (SPI_FD >= 0)
	{
		spi_config(SPI_FD);
	}

	return SPI_FD;
}

static uint8_t control_byte(int yaw, int pitch, int* yaw_ticks, int* pitch_ticks)
{
	uint8_t ctrl_byte = 0;

	if (yaw < 0) { ctrl_byte |= 0x80; }
	*yaw_ticks = clamp(yaw, -7, 7);

	if (pitch < 0) { ctrl_byte |= 0x08; }
	*pitch_ticks = clamp(pitch, -7, 7);

	return ctrl_byte | ((abs(*yaw_ticks) << 4) | abs(*pitch_ticks));
}

void motor_control(int yaw, int pitch, int* yaw_ticks, int* pitch_ticks)
{
	spi_transfer(SPI_FD, control_byte(yaw, pitch, yaw_ticks, pitch_ticks));
}

void motor_control_off()
{
	spi_transfer(SPI_FD, 0);
}