/*
 * author: Brian Ruiz
 * date:   February 18, 2016
 * school: George Mason University
 */

#include <stdint.h>
#include "calculations.h"

uint16_t rad_to_tick(float rad)
{
	uint16_t ticks = ((rad*(180/PI))/0.29) + 512;
	if (ticks > 1023)
		return 0x3FF;
	return ticks;
}

float tick_to_rad(uint16_t tick)
{
	 return ((tick*0.29)*(PI/180));
}

uint16_t sec_to_tick(float sec)
{
	return ((2.271f)/1000000)*sec;
}
