#include <msp.h>
#include "init.h"
#include "dynamixel.h"
#include "dynamixel_apis.h"
#include "calculations.h"
#include "gestures.h"

void main(void)
{
	msp_init();
	uint8_t i;
	for (i = 0; i < 2; i++)
	{
		sync_ids[i] = i;
		sync_positions[i] = i;
		sync_speeds[i] = i;
	}
	sync_write(2);
	//dynamixel_init();
    while(1);
}
