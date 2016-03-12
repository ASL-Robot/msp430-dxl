/*
 * author: Brian Ruiz
 * date:   February 18, 2016
 * school: George Mason University
 *
 * this file contains the initialization for each of the peripherals
 * needed for this project.
 */

#include "dynamixel.h"
#include "dynamixel_apis.h"
#include <msp.h>
#include <stdint.h>

void msp_init(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    /* clock initialization */
    CSKEY = 0x695A;
    CSCTL0 |= DCOEN | DCORSEL_5;									// 48 mhz oscillator
    CSCTL1 |= DIVS_1 | SELS_3 | SELM_3; 							// SMCLK = 24 mhz
    																// MCLK  = 48 mhz
    CSKEY = 0x0000; 												// lock the clock registers
    while((!(CSSTAT & SMCLK_READY)) && (!(CSSTAT & MCLK_READY)));	// wait for clocks to ready

    /* uart initialization */
    P2REN |= BIT3;
    P2OUT |= BIT3;
    P2SEL0 |= (BIT2 | BIT3); 						// P3.2 = UCA2RX;
    P2SEL1 &= ~(BIT2 | BIT3); 						// P3.3 = UCA2TX;
    UCA1CTLW0 = UCSWRST;
    UCA1CTLW0 |= UCSSEL_2;
    UCA1BRW |= 1;
    UCA1MCTLW |= 0x81;
    UCA1CTLW0 &= ~UCSWRST;

    /* spi initialization */
    P1SEL0 |= (BIT4 | BIT5 | BIT6 | BIT7);			// P1.4 = UCB0STE, P1.5 = UCB0CLK
    P1SEL1 &= ~(BIT4 | BIT5 | BIT6 | BIT7); 		// P1.6 = UCB0MOSI, P1.7 = UCB0MISO
    UCB0CTLW0 = UCSWRST;
    UCB0CTLW0 |= UCMSB | UCMODE_2 | UCSYNC;
    UCB0CTLW0 &= ~UCSWRST;

    /* port initialization */
    P4DIR &= ~BIT1;									// initialization of the port that
    P4REN |= BIT1;									// must be brought low in order
    P4OUT |= BIT1;									// to maintain synchronization between
    P4IES |= BIT1;									// both microcontrollers.
    P4IE  |= BIT1;

    /* floating-point unit initialization */
    SCB_CPACR |= (UINT32_C(0x0F) << 20);

    /* direction and flag initialization */
    P5DIR |= BIT0;									// flag	pin
    P6DIR |= BIT0; 									// direction pin

    /* interrupt settings */
    NVIC_ISER0 |= (1 << ((INT_EUSCIA1 - 16) & 31)); // enable euscia1 (uart)
    NVIC_ISER0 |= (1 << ((INT_TA0_0 - 16) & 31)); 	// enable timer (timer)
    NVIC_ISER0 |= (1 << ((INT_EUSCIB0 - 16) & 31));	// enable euscib0 (spi)
    NVIC_ISER1 |= (1 << ((INT_PORT4 - 16) & 31));	// enable port4 (ready signal)
    PCMCTL1 &= ~LOCKLPM5;							// unlock ports

    __enable_interrupts();							// enable interrupts
}

void dynamixel_init(void)
{
	set_return(0xFE, 0x01);
	joint_mode(0xFE);
	torque_enable(0xFE);
	led_on(0xFE);
	__delay_cycles(4800000); 						// LEDs stay on for 1 second
	led_off(0xFE);
}
