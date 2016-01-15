/*
 * author: Brian Ruiz
 * date:   December 19, 2015
 * school: George Mason University
 */

#include <msp430.h>
#include "init.h"

void msp_init(void)
{
	WDTCTL = WDTPW | WDTHOLD;

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
	UCB0CTLW0 |= UCMODE_1 | UCSYNC;		// active high enable slave
	UCB0CTLW0 &= ~UCSWRST;
	UCB0IE |= UCRXIE;					// receive interrupt enable

	/* set up UART */
	UCA0CTLW0 = UCSWRST;
	UCA0CTLW0 |= UCSSEL__SMCLK;
	UCA0BRW   |= 6; 					// baud rate: 9600
	UCA0MCTLW |= 0x2081;				/* WE NEED TO CHANGE THIS TO SOMETHING FASTER */
	UCA0CTLW0 &= ~UCSWRST;
}

void dynamixel_init(void)
{
	return;
}
