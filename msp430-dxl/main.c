#include <msp430.h> 
#include <stdint.h>
#include "dynamixel.h"
#include "init.h"
#include "calculations.h"

uint8_t x;

void main(void)
{
	uint64_t goal;
	msp_init(); 					// initialize SPI and UART
	//dynamixel_init();				// initialize the motors
	set_id(0xFE, 0x01);
	set_return(0x01, 1);
	set_delay(0x01, 250);
	while(1)
	{
		goal_position(0x03, 0x100, 0x100);
		goal = curr_position(0x03);
		__delay_cycles(2000000);
		goal_position(0x03, 0x300, 0x300);
		goal = curr_position(0x03);
		__delay_cycles(2000000);
	}
}


#pragma vector = USCI_B0_VECTOR
__interrupt void spi_receive(void)
{
  x = UCA0RXBUF;                    // echo received data
}

