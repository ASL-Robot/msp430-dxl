/*
 * author: Brian Ruiz
 * date:   December 19, 2015
 * school: George Mason University
 */

#include <stdint.h>
#include "calculations.h"

uint16_t rad_to_tick(float rad)
{
	uint16_t ticks = (rad*(180/PI))/0.29;
	if (ticks > 1024)
		return 0x3FF;
	return ticks;
}

float tick_to_rad(uint16_t tick)
{
	 return ((tick*0.29)*(PI/180));
}
