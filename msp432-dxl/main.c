#include <msp.h>
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
		float goal_pos;
		uint32_t start;
		uint32_t end;
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
    {
    	__sleep();
    }
}

void spi()
{
	/* general variables */
	static uint16_t r_i = 0, num_moves = 0, num_bytes;
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
				switch(packet_type)
				{
					case 1: 	// movement
						if (r_i == 0)
							id = UCB0RXBUF;
						else if (r_i == 1)
						{
							if (id == 0xFE)
							{
								g_id = UCB0RXBUF;
								gesture(g_id);
								packet_type = id = r_i = g_id = 0;
							}
							else if (id == 0xFD)
							{
								checkpoint = UCB0RXBUF;
								packet_type = id = r_i = 0;
							}
							else
								num_moves = UCB0RXBUF;
						}
						else if (r_i == 2)
						{
							num_moves = (UINT16_C(UCB0RXBUF) << 8) | num_moves;
							num_bytes = (13*num_moves);
						}
						else
						{
							if ((r_i - 3) < num_bytes)
							{
								if (r_i == ((13*(q_num+1))+3))
									q_num++;
								queue[q_num].raw[r_i - (13*q_num)+3] = UCB0RXBUF;
							}
							else
								packet_type = q_num = num_moves = num_bytes = r_i = id = 0;
						}
						break;
					case 2:
						break;
					case 3:
						break;
				}
				r_i++;
			}
			break;
		case UCTXIFG: UCB0TXBUF = 'U'; break;
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
