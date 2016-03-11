#include <msp.h>
#include <stdint.h>
#include "init.h"
#include "dynamixel.h"
#include "dynamixel_apis.h"
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
				uint8_t padding0[11];
			};
			struct __attribute__((__packed__))
			{
				uint8_t checkpoint;
				uint8_t padding1[11];
			};
		};
	};
} movement;
movement queue[40];

/* communication flags and variables*/
uint8_t waiting = 1;
uint8_t len = 0;

/* interrupt vector holders */
uint8_t s, t, p;

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
    		if (queue[i].jid == 0xFE)
    		{
    			__no_operation();
    		}
    		else if (queue[i].jid == 0xFD)
    		{
    			__no_operation();
    		}
    		else
    		{
    			__no_operation();
    		}
    	}
    }
}

void spi()
{
	/* general variables */
	static uint16_t r_i = 0, num_moves = 0;
	static uint8_t id, packet_type = 0;

	/* specific data holders */
	uint8_t g_id = 0;
	static uint8_t checkpoint = 0, q_num = 0;

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
							UCA3IE &= ~UCRXIE;
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

void timer()
{
	__no_operation();
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
