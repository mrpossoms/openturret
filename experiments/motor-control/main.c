#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static int path_exists(const char* path)
{
	struct stat statbuf;

	switch (stat(path, &statbuf))
	{
		case 0:      return 1;
		case ENOENT: return 0;
	}

	return 0;
}


int hwd_gpio_set(int pin, int state)
{
	char buf[PATH_MAX] = {};
	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d", pin);

	if (!path_exists(buf))
	{
		snprintf(buf, sizeof(buf), "echo %d > /sys/class/gpio/export", pin);
		system(buf);

		snprintf(buf, sizeof(buf), "echo out > /sys/class/gpio/gpio%d/direction", pin);
		system(buf);
	}

	snprintf(buf, sizeof(buf), "echo %d > /sys/class/gpio/gpio%d/value", state, pin);
	system(buf);

	return 0;
}


typedef struct {
	int coil_pins[2][2];
	int coil_pin_cfg[2][2];
} stepper_t;


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
			hwd_gpio_set(m->coil_pin_cfg[i % 2][j], signals[i][j]);
		}
	}
}


int main(int argc, const char* argv[])
{
	stepper_t m = {
		.coil_pins = {
			{18, 23},
			{24, 25},
		},
	};

	stepper_dir(&m, atoi(argv[1]));

	for (int steps = atoi(argv[2]); steps--;)
	{
		stepper_step(&m);
	}

	return 0;
}
