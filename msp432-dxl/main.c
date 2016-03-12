#include <msp.h>
#include "init.h"
#include "dynamixel.h"
#include "dynamixel_apis.h"
#include "calculations.h"
#include "gestures.h"

void main(void)
{
	P3DIR |= BIT0 | BIT2 | BIT5 | BIT6;
	P3OUT &= ~BIT0 & ~BIT2 & ~BIT5 & ~BIT6;
	msp_init();
	//dynamixel_init();
	uint8_t i;

    while(1)
    {
    	for (i = 0; i < 3; i++)
    	{
    		sync_ids[i] = i+0x13;
    		sync_positions[i] = 400;
    		sync_speeds[i] = 200;
    	}
    	sync_write(3);
    	P3OUT |= BIT0;
    	__delay_cycles(4800000);

    	for (i = 0; i < 3; i++)
    	{
    		sync_ids[i] = i+0x13;
    		sync_positions[i] = 600;
    		sync_speeds[i] = 200;
    	}
    	sync_write(3);
    	P3OUT &= ~BIT0;
    	__delay_cycles(4800000);

    }
}
