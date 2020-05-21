/*
 * GccApplication1.c
 *
 * Created: 1/23/2020 10:04:20 PM
 * Author : Kirk Roerig
 */ 
//#define F_CPU 8000000
#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include "avr-usi-spi/spi_via_usi_driver.c" 

#define DDR_SPI DDRB
#define DD_MOSI 2
#define DD_SCK 1

typedef enum {
	OT_PORTA,
	OT_PORTB,
} port_t;

typedef struct {
	port_t port;
	int coil_pins[2][2];
	int coil_pin_cfg[2][2];
} stepper_t;


#define gpio_set(port, pin, state) { \
	if (state) (port) |=  (1 << (pin)); \
	else       (port) &= ~(1 << (pin)); \
} \


void stepper_dir(stepper_t* m, int dir)
{
	if (dir < 0)
	{
		m->coil_pin_cfg[0][0] = m->coil_pins[1][0];
		m->coil_pin_cfg[0][1] = m->coil_pins[1][1];
		m->coil_pin_cfg[1][0] = m->coil_pins[0][0];
		m->coil_pin_cfg[1][1] = m->coil_pins[0][1];
	}
	else
	{
		m->coil_pin_cfg[0][0] = m->coil_pins[0][0];
		m->coil_pin_cfg[0][1] = m->coil_pins[0][1];
		m->coil_pin_cfg[1][0] = m->coil_pins[1][0];
		m->coil_pin_cfg[1][1] = m->coil_pins[1][1];
	}
}


void stepper_step(const stepper_t* m)
{
	int signals[4][2] = {
		{1, 0},
		{0, 1},
		{0, 1},
		{1, 0},
	};

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			switch (m->port)
			{
				case OT_PORTA:
					gpio_set(PORTA, m->coil_pin_cfg[i % 2][j], signals[i][j]);
					break;
				case OT_PORTB:
					gpio_set(PORTB, m->coil_pin_cfg[i % 2][j], signals[i][j]);
					break;
			}
		}

		_delay_ms(3);
	}
}


int main(void)
{
	stepper_t yaw = {
		.port = OT_PORTA,
		.coil_pins = {
			{ 3, 2 },
			{ 1, 0 },
		},
	};

	stepper_t pitch = {
		.port = OT_PORTB,
		.coil_pins = {
			{ 7, 6 },
			{ 1, 2 },
		},
	};
	
	stepper_t* steppers[2] = { &yaw, &pitch };
	int8_t step_deltas[2] = {};

	// Setup pin directionality
	for (int si = 0; si < sizeof(steppers) / sizeof(stepper_t*); ++si)
	{
		int ddr = 0;
		for (int ci = 2; ci--;)
		for (int pi = 2; pi--;)
		{
			ddr |= (1 << steppers[si]->coil_pins[ci][pi]);
		}

		if (OT_PORTA == steppers[si]->port) { DDRA |= ddr; }
		if (OT_PORTB == steppers[si]->port) { DDRB |= ddr; }
	}

	// initalize usi spi
	spiX_initslave(1);

	// main loop
	for (;;)
	{
		if (spiX_status.transferComplete)
		{
			// Step deltas are sent to us as a signle byte, where the high 4 bits
			// control the yaw, and the low 4 control the pitch. The most significant
			// bit for each group of 4 is a signed bit indicating direction.
			char ctrl_byte = spiX_get();
			int8_t yaw = ctrl_byte >> 4;
			int8_t pitch = ctrl_byte & 0x0F;

			step_deltas[0] += (yaw >> 3) > 0 ? (yaw & 0x7) : -(yaw & 0x7);
			step_deltas[1] += (pitch >> 3) > 0 ? (pitch & 0x7) : -(pitch & 0x7);

			spiX_status.transferComplete = 0;
		}

		for (int i = sizeof(steppers) / sizeof(stepper_t*); i--;)
		{
			stepper_dir(steppers[i], step_deltas[i]);
			stepper_step(steppers[i]);

			if (step_deltas[i] > 0) { step_deltas[i]--; }
			else if (step_deltas[i] < 0) { step_deltas[i]++; }
		}
	}

	PORTA = 0;

	return 0;
}
