/*
 * GccApplication1.c
 *
 * Created: 1/23/2020 10:04:20 PM
 * Author : Kirk Roerig
 */ 
//#define F_CPU 8000000
#define F_CPU 1000000

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#define USI_OUT_REG	PORTA	//!< USI port output register.
#define USI_IN_REG	PINA	//!< USI port input register.
#define USI_DIR_REG	DDRA	//!< USI port direction register.
#define USI_CLOCK_PIN	PA4	//!< USI clock I/O pin.
#define USI_DATAIN_PIN	PA6	//!< USI data input pin.
#define USI_DATAOUT_PIN	PA5	//!< USI data output pin.


typedef enum {
	OT_PORTA,
	OT_PORTB,
} port_t;

typedef struct {
	port_t port;
	int num;
} pin_t;

typedef struct {
	pin_t coil_pins[2][2];
	pin_t coil_pin_cfg[2][2];
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
			pin_t pin = m->coil_pin_cfg[i % 2][j];

			switch (pin.port)
			{
				case OT_PORTA:
					gpio_set(PORTA, pin.num, signals[i][j]);
					break;
				case OT_PORTB:
					gpio_set(PORTB, pin.num, signals[i][j]);
					break;
			}
		}

		_delay_ms(3);
	}
}

volatile int8_t step_deltas[2] = {};

ISR(USI_OVF_vect)
{
	// fetch the yaw and pitch parts of the USIDR which
	// contains the byte recieved via SPI (USI)
	int8_t ctrl = USIBR;
	int8_t yaw = (ctrl >> 4) & 0xF;
	int8_t pitch = ctrl & 0x0F;

	step_deltas[0] += (yaw & 0x08) > 0 ? (yaw & 0x7) : -(yaw & 0x7);
	step_deltas[1] += (pitch & 0x08) > 0 ? (pitch & 0x7) : -(pitch & 0x7);

	USISR |= _BV(USIOIF);
	// USIBR = 0;
	// sei();
}

int main(void)
{
	stepper_t yaw = {
		.coil_pins = {
			{ { OT_PORTA, 3 }, { OT_PORTA, 2 } },
			{ { OT_PORTA, 1 }, { OT_PORTA, 0 } },
		},
	};

	stepper_t pitch = {
		.coil_pins = {
			{ { OT_PORTB, 0 }, { OT_PORTB, 1 } },
			{ { OT_PORTB, 2 }, { OT_PORTA, 7 } },
		},
	};
	
	stepper_t* steppers[2] = { &yaw, &pitch };


	// Setup pin directionality
	for (int si = 0; si < sizeof(steppers) / sizeof(stepper_t*); ++si)
	{
		for (int ci = 2; ci--;)
		for (int pi = 2; pi--;)
		{
			pin_t pin = steppers[si]->coil_pins[ci][pi];
			if (OT_PORTA == pin.port) { DDRA |= (1 << pin.num); }
			if (OT_PORTB == pin.port) { DDRB |= (1 << pin.num); }
		}
	}

	// Configure port directions.
	const int spi_mode = 1;

	USI_DIR_REG |= (1<<USI_DATAOUT_PIN);                      // Outputs.
	USI_DIR_REG &= ~((1<<USI_DATAIN_PIN) | (1<<USI_CLOCK_PIN)); // Inputs.
	USI_OUT_REG |= (1<<USI_DATAIN_PIN) | (1<<USI_CLOCK_PIN);  // Pull-ups.

	USICR = (1<<USIOIE) | (1<<USIWM0) |
	        (1<<USICS1) | (spi_mode<<USICS0);

	sei();

	// main loop
	for (unsigned int t = 0; 1;)
	{
		for (int i = sizeof(steppers) / sizeof(stepper_t*); i--;)
		{
			if (step_deltas[i])
			{
				stepper_dir(steppers[i], step_deltas[i]);
				stepper_step(steppers[i]);
			}

			//cli();
			if (step_deltas[i] > 0) { step_deltas[i]--; }
			else if (step_deltas[i] < 0) { step_deltas[i]++; }
			//sei();
		}
	}

/*
	for (int i = 50; i--;)
	{
		stepper_dir(&yaw, 1);
		stepper_dir(&pitch, 1);
		stepper_step(&yaw);

		if (i < 25)
		{
			stepper_step(&pitch);
		}
	}
*/
	PORTA = 0;
	PORTB = 0;

	return 0;
}
