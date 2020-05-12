/*
 * GccApplication1.c
 *
 * Created: 1/23/2020 10:04:20 PM
 * Author : Kirk Roerig
 */ 
//#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

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
	}
}


int main(void)
{
	stepper_t yaw = {
		.port = PORTA,
	};

	stepper_t pitch = {
		.port = PORTB,
	};
	
	stepper_t* steppers[2] = { &yaw, &pitch };

	// Setup pin directionality
	for (int si = 0; si < sizeof(steppers) / sizeof(stepper_t); ++si)
	{
		int ddr = 0;
		for (int ci = 2; ci--;)
		for (int pi = 2; pi--;)
		{
			ddr &= (1 << steppers[si]->coil_pins[ci][pi]);
		}

		if (PORTA == steppers[si]->port) { DDRA &= ddr; }
		if (PORTB == steppers[si]->port) { DDRB &= ddr; }
	}

	// main loop
	while (1)
	{

	}

	return 0;
}
