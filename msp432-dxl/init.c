/*
 * author: Brian Ruiz
 * date:   February 18, 2016
 * school: George Mason University
 *
 * this file contains the initialization for each of the peripherals
 * needed for this project.
 */

#include "dynamixel.h"
#include <msp.h>
#include <stdint.h>

void msp_init(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    /* change the number of wait states */
    while(FLCTL_RDBRST_CTLSTAT & FLCTL_RDBRST_CTLSTAT_BRST_STAT1);
    FLCTL_BANK0_RDCTL |= FLCTL_BANK0_RDCTL_WAIT_2;
    FLCTL_BANK1_RDCTL |= FLCTL_BANK1_RDCTL_WAIT_2;

    /* change vcore to higher voltage */
    while(PCMCTL1 & PMR_BUSY);
    PCMCTL0 = (0x695A0000 | AMR__AM_LDO_VCORE1);
    while(PCMCTL1 & PMR_BUSY);
    PCMCTL0 = (0x695A0000 | AMR__AM_DCDC_VCORE1);
    while((PCMCTL1 & PMR_BUSY) && (!(PCMCTL0 & CPM_5)));

    /* clock initialization */
    CSKEY = 0x695A;
    CSCTL0 = 0;
    CSCTL0 |= DCORSEL_5;
    CSCTL1 |= DIVS_1 | SELS_3 | SELM_3; 							// SMCLK = 24 mhz
    																// MCLK  = 48 mhz
    CSKEY = 0x0000; 												// lock the clock registers
    while((!(CSSTAT & SMCLK_READY)) && (!(CSSTAT & MCLK_READY)));	// wait for clocks to ready

    /* uart initialization */
    P2SEL0 |= (BIT2 | BIT3); 						// P2.2 = UCA1RX;
    P2SEL1 &= ~(BIT2 | BIT3); 						// P2.3 = UCA1TX;
    UCA1CTLW0 = UCSWRST;
    UCA1CTLW0 |= UCSSEL_2;
    UCA1BRW |= 1;
    UCA1MCTLW |= 0x81;
    UCA1CTLW0 &= ~UCSWRST;

    /* spi initialization */
    P9SEL0 |= (BIT4 | BIT5 | BIT6 | BIT7);			// P9.4 = UCA3STE, P9.5 = UCA3CLK
    P9SEL1 &= ~(BIT4 | BIT5 | BIT6 | BIT7); 		// P9.6 = UCA3MISO, P9.7 = UCA3MOSI
    UCA3CTLW0 = UCSWRST;
    UCA3CTLW0 |= UCMSB | UCMODE_2 | UCSYNC;
    UCA3CTLW0 &= ~UCSWRST;

	/* scheduler initialization */
    SYSTICK_STCSR  |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;		// enable core clock
    SYSTICK_STRVR  |= 288000-1;													// interrupt fires every 6 ms.

    /* floating-point unit initialization */
    SCB_CPACR |= (UINT32_C(0x0F) << 20);

    /* communication ports initialization */
    P6DIR &= ~BIT6;		// "go" initialization
    P6REN |= BIT6;
    P6OUT |= BIT6;
    P6IES |= BIT6;
    P6IE  |= BIT6;

    P7DIR |= BIT3; 		// "ready" initialization
    P7DIR &= ~BIT3;

    P2DIR |= BIT1; 		// "direction" initialization
    P2OUT &= ~BIT1;

    P6DIR &= ~BIT7;		// "emergency" initialization
    P6REN |= BIT7;
    P6OUT |= BIT7;
    P6IES |= BIT7;
    P6IE  |= BIT7;

    P10DIR |= BIT0;		// "error" initialization
    P10OUT &= ~BIT0;

    /* interrupt settings */
    NVIC_ISER0 |= (1 << ((INT_EUSCIA1 - 16) & 31)); // enable euscia1 (uart)
    NVIC_ISER0 |= (1 << ((INT_EUSCIA3 - 16) & 31));	// enable euscia3 (spi)
    NVIC_ISER1 |= (1 << ((INT_PORT6 - 16) & 31));	// enable port6 (ready signal)
    PCMCTL1 &= ~LOCKLPM5;							// unlock ports

    __enable_interrupts();							// enable interrupts
}

void dynamixel_init(void)
{
	uint8_t j;
	read_id = 0;									// load in a joint id of 0
	event_reg = UART_READY;

	/* now send everything to home position (just to make sure we're okay!) */
	for (j = 0; j < 8; j++)
	{
		sync_ids[j] = j;
		sync_speeds[j] = 0x100;
		if (!j)
			sync_positions[j] = goal_positions[j] = 0x200;
		else
			sync_positions[j] = goal_positions[j] = 0x800;
	}
	sync_len = 8;
	g_id = open_id;
	UCA1IE |= UCTXIE;
	while(event_reg != UART_SEND_DONE);

	event_reg = UART_READ;							// tell the UART to send a read
	UCA1IE |= UCTXIE; 								// turn on sending interrupts.
	while(event_reg != UART_READ_DONE);				// sleep while reading; we can take as much time as we need.
	event_reg = UART_READY;
    UCA3IE |= UCRXIE;								// we can now turn on spi receive
}
