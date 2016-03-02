/*
 * author: Brian Ruiz
 * date:   December 19, 2015
 * school: George Mason University
 */

#include <stdint.h>
#include "calculations.h"

uint16_t rad_to_tick(float rad)
{
	uint16_t ticks = ((rad*(180/PI))/0.29) + 512;
	if (ticks > 1023)
		return 0x3FF;
	return (uint16_t)ticks;
}

float tick_to_rad(uint16_t tick)
{
	 return ((tick*0.29)*(PI/180));
}

uint16_t sec_to_tick(float sec)
{
	return (2.271/1000000)*sec;
}
