/*
 * author: Brian Ruiz
 * date:   December 19, 2015
 * school: George Mason University
 */

#include <msp430.h>
#include "dynamixel.h"
#include "init.h"

void msp_init(void)
{
	WDTCTL = WDTPW | WDTHOLD;

	CSCTL0_H = CSKEY_H;
	CSCTL1 = DCORSEL | DCOFSEL_4;
	CSCTL3 = DIVA_0 | DIVS__8 | DIVM__2;
	CSCTL0_H = 0;

	/* configure pins for SPI */
	P1SEL0 |= (BIT4 | BIT5 | BIT6 | BIT7);		// P1.6 = UCB0MOSI
	P1SEL1 &= ~(BIT4 | BIT5 | BIT6 | BIT7); 	// P1.7 = UCB0MISO
												// P1.5 = UCB0STE
												// P1.4 = UCB0CLK

	/* configure pins for UART */
	P4SEL0 |= (BIT2 | BIT3);			// P4.2 = UCA0TX
	P4SEL1 &= ~(BIT2 | BIT3);			// P4.3 = UCA0RX
	P4REN |= (BIT3);
	P4OUT |= (BIT3);
	P3DIR  |= BIT2; 					// P3.2 = DIRECTION

	P3OUT  |= BIT2;						// claim the bus

	PM5CTL0 &= ~LOCKLPM5;

	/* set up SPI */
	UCB0CTLW0 = UCSWRST;
	UCB0CTLW0 |= UCMSB | UCMODE_2 | UCSYNC;		// active low enable slave
	UCB0CTLW0 &= ~UCSWRST;

	/* set up UART */
	UCA0CTLW0 = UCSWRST;
	UCA0CTLW0 |= UCSSEL__SMCLK;			// 1 MHz baud
	UCA0BRW = 2;
	UCA0MCTLW = 0;
	UCA0CTLW0 &= ~UCSWRST;
}

void dynamixel_init(void)
{
//	set_id(0xFE, 0x12);
//	joint_mode(0x12);
	//set_return(0x12, 1);
//	torque_enable(0x16);
//	torque_enable(0x12);

	//factory_reset(0x16, 2);
	//factory_reset(0x12, 2);


//	led_on(0xFE);
//	__delay_cycles(100000);
//	led_off(0xFE);

	//set_id(0xFE, 0x12);
	//joint_mode(0x12);
	__no_operation();
}
