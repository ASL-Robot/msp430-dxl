#include <msp430.h> 
#include <stdint.h>
#include "dynamixel.h"
#include "init.h"
#include "calculations.h"
#include "gestures.h"

void curl();

typedef union {
	uint8_t raw[13];
	struct __attribute__((__packed__)) {
		uint8_t jid;
		float goal_pos;
		uint32_t start;
		uint32_t end;
	};
} movement;

uint8_t x;
uint16_t num_moves = 0;
movement queue[20];

void main(void)
{
	//uint16_t goal;
	uint8_t i;
	msp_init(); 					// initialize SPI and UART
	msp_init();
	sync_ids[0] = 0x10;
	sync_ids[1] = 0x11;
	sync_ids[2] = 0x12;
	sync_ids[3] = 0x13;

	sync_positions[0] = 0x010;
	sync_positions[1] = 0x220;
	sync_positions[2] = 0x030;
	sync_positions[3] = 0x220;

	sync_speeds[0] = 0x150;
	sync_speeds[1] = 0x360;
	sync_speeds[2] = 0x170;
	sync_speeds[3] = 0x380;

	sync_write(4);
    while(1);
	//dynamixel_init();				// initialize the motors
//	while(1)
//	{
//		while(!(UCB0IFG & UCRXIFG));
//		num_moves = UCB0RXBUF;
//		while(!(UCB0IFG & UCRXIFG));
//		num_moves = (UINT16_C(UCB0RXBUF) << 8) | num_moves;
//
//		for (uint16_t i = 0; i < num_moves; i++)
//		{
//			for (uint16_t j = 0; j < 13; j++)
//			{
//				while(!(UCB0IFG & UCRXIFG));
//				queue[i].raw[j] = UCB0RXBUF;
//			}
//		}
//
//		/* calculation time! */
//		for (uint16_t i = 0; i < num_moves; i++)
//		{
//			sync_ids[i] = queue[i].jid;
//			sync_positions[i] = rad_to_tick(queue[i].goal_pos);
//			sync_speeds[i] = sec_to_tick(queue[i].end - queue[i].start);
//		}
//
//		sync_write(num_moves);
//		curl();
//		__delay_cycles(12000000);
//		open();
//		__delay_cycles(12000000);
//		thumbs_up();
//		__delay_cycles(12000000);
//		point();
//		__delay_cycles(12000000);
//		okay();
//		__delay_cycles(12000000);
//		open();
//		__delay_cycles(12000000);
//	}
}
