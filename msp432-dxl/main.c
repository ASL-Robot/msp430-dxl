#include <msp.h>
#include <stdint.h>
#include "init.h"
#include "dynamixel.h"
#include "dynamixel_apis.h"
#include "calculations.h"
#include "gestures.h"

/* for processing information from raspberry pi */
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
uint8_t s, t, p;

void main(void)
{
	msp_init();
//	uint8_t i;
//	for (i = 0; i < 2; i++)
//	{
//		sync_ids[i] = i;
//		sync_positions[i] = i;
//		sync_speeds[i] = i;
//	}
//	sync_write(2);
//	//dynamixel_init();
    while(1)
    	__sleep();
}

void spi()
{
	/* general variables */
	static uint16_t r_i = 0, num_moves = 0;
	static uint8_t id, packet_type = 0;

	/* specific data holders */
	uint8_t g_id = 0;
	static uint8_t checkpoint = 0, q_num = 0;

	s = UCB0IV;
	switch(s)
	{
		case UCRXIFG:
			if (packet_type == 0)
			{
				packet_type = UCB0RXBUF;
				r_i = 0;
			}
			else
			{

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
