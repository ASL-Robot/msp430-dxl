#include <msp.h>
#include <stdint.h>
#include "init.h"
#include "dynamixel.h"
#include "calculations.h"
#include "gestures.h"

/* data structure for information from raspberry pi */
typedef union
{
	uint8_t raw[13];
	struct __attribute__((__packed__))
	{
		uint8_t jid;
		union
		{
			struct __attribute__((__packed__))
			{
				float goal_pos;
				uint32_t start_time;
				uint32_t end_time;
			};
			struct __attribute__((__packed__))
			{
				uint8_t gesture;
				uint32_t gstart_time;
				//uint8_t padding0[7];
			};
			struct __attribute__((__packed__))
			{
				uint8_t checkpoint;
				//uint8_t padding1[11];
			};
		};
	};
} raw_data;

typedef struct __attribute__((__packed__))
{
	uint8_t jid;
	union
	{
		struct __attribute__((__packed__))
		{
			uint16_t position;
			uint16_t speed;
			uint32_t start_time;
		};
		struct __attribute__((__packed__))
		{
			uint8_t gesture;
			uint32_t gstart_time;
			//uint8_t padding0[4];
		};
		struct __attribute__((__packed__))
		{
			uint8_t checkpoint;
			//uint8_t padding1[7];
		};
	};
} processed;

/* arrays */
raw_data queue[40];
processed buffer[40];

/* communication flags and variables*/
uint8_t waiting = 1;
uint8_t len = 0;

/* interrupt vector holders */
uint8_t s, p;

void main(void)
{
	uint8_t i;
	msp_init();
	//dynamixel_init();
    while(1)
    {
    	while(waiting)
    		__wfi();
    	for (i = 0; i < len; i++)
    	{
    		if (queue[i].jid == 0xFE)		// gesture
    		{
    			buffer[i].jid = queue[i].jid;
    			buffer[i].gesture =  queue[i].gesture;
    			buffer[i]. gstart_time = queue[i].gstart_time;
    		}
    		else if (queue[i].jid == 0xFD)	// checkpoint
    		{
    			buffer[i].jid = queue[i].jid;
    			buffer[i].checkpoint = queue[i].checkpoint;
    		}
    		else							// calculation for non-xl motor must be done.
    		{
    			float rad_delta;			// this is the distance we are supposed to travel
    			float time_delta = queue[i].end_time - queue[i].start_time;
    			float rate;
    			uint16_t ticks;
    			if (readings[queue[i].jid] >= (queue[i].goal_pos))
    				rad_delta = T2R(readings[queue[i].jid]) - queue[i].goal_pos;
    			else
    				rad_delta = queue[i].goal_pos - T2R(readings[queue[i].jid]);
    			rate = (9549296/0.111)*(rad_delta/time_delta);		// now in ticks.
    			if (rate > 1023)
    				rate = 1023;
    			if (R2T(queue[i].goal_pos) > 1023)
    				ticks = 1023;
    			else
    				ticks = R2T(rad_delta);

    			/* now store in data queue */
    			buffer[i].jid = queue[i].jid;
    			buffer[i].position = ticks;
    			buffer[i].speed = rate;
    		}
    		//while(event_reg != DONE)
    			//__sleep();
    	}
    }
}

void spi()
{
	/* general variables */
	static uint16_t r_i = 0, num_moves = 0;
	static uint8_t packet_type = 0;

	s = UCA3IV;
	switch(s)
	{
		case UCRXIFG:
			switch(packet_type)
			{
				case 0: packet_type = UCA3RXBUF; r_i = 0; break;
				case 1:
					if (r_i == 0)
					{
						num_moves = UCA3RXBUF;
						r_i++;
					}
					else if (r_i == 1)
					{
						num_moves = (UINT16_C(UCA3RXBUF) << 8) | num_moves;
						r_i++;
					}
					else if ((r_i-2) < 13*num_moves)
					{
						queue[(r_i-2)/13].raw[(r_i-2)%13] = UCA3RXBUF;
						if ((r_i-2) == (13*num_moves)-1)
						{
							packet_type = waiting = 0;
							len = (r_i-2)/13;
							//UCA3IE &= ~UCRXIE;
						}
						else
							r_i++;
					}
					break;
				case 2:
					break;
				case 3:
					break;
			}
			break;
	}
}

void port()
{
	p = P4IV;
	switch(p)
	{
		case 0x04: break;
		default: break;
	}
}

void systick()
{
	__no_operation();
}
