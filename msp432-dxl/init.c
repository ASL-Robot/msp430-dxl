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
    UCA3IE |= UCRXIE;
    UCA3CTLW0 &= ~UCSWRST;

    /* port initialization */
    P4DIR &= ~BIT1;									// initialization of the port that
    P4REN |= BIT1;									// must be brought low in order
    P4OUT |= BIT1;									// to maintain synchronization between
    P4IES |= BIT1;									// both microcontrollers.
    P4IE  |= BIT1;

	/* scheduler initialization */
    SYSTICK_STCSR  |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;		// enable core clock
    SYSTICK_STRVR  |= 960000-1;													// interrupt fires every 30 ms.

    /* floating-point unit initialization */
    SCB_CPACR |= (UINT32_C(0x0F) << 20);

    /* direction and flag initialization */
    P5DIR |= BIT0;									// flag	pin
    P6DIR |= BIT0; 									// direction pin

    /* interrupt settings */
    NVIC_ISER0 |= (1 << ((INT_EUSCIA1 - 16) & 31)); // enable euscia1 (uart)
    NVIC_ISER0 |= (1 << ((INT_EUSCIA3 - 16) & 31));	// enable euscia3 (spi)
    NVIC_ISER1 |= (1 << ((INT_PORT4 - 16) & 31));	// enable port4 (ready signal)
    PCMCTL1 &= ~LOCKLPM5;							// unlock ports

    __enable_interrupts();							// enable interrupts
}

void dynamixel_init(void)
{
	read_id = 0;									// load in a joint id of 0
	event_reg = UART_READ;							// tell the UART to send a read
	UCA1IE |= UCTXIE; 								// turn on sending interrupts.
	while(event_reg != UART_READ_DONE)				// sleep while reading; we can take as much time as we need.
		__sleep();
}
