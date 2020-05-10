/*
 * GccApplication1.c
 *
 * Created: 1/23/2020 10:04:20 PM
 * Author : Kirk
 */ 
//#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include "avr/iom256rfr2.h"

#define DDR_SPI DDRB
#define DD_MOSI 2
#define DD_SCK 1

#define DDR_MOTORS DDRB
#define M0_PIN 4
#define M1_PIN 5
#define M2_PIN 6
#define M3_PIN 7

int main(void)
{
	int delay = 0;
	
	DDRB = (1<<4);


	return 0;
}
