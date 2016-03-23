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

    /* floating-point unit initialization */
    SCB_CPACR |= (UINT32_C(0x0F) << 20);

    /* direction and flag initialization */
    P5DIR |= BIT0;									// flag	pin
    P6DIR |= BIT0; 									// direction pin

    /* interrupt settings */
    NVIC_ISER0 |= (1 << ((INT_EUSCIA2 - 16) & 31)); // enable euscia2 (uart)
    NVIC_ISER0 |= (1 << ((INT_TA0_0 - 16) & 31)); 	// enable timer (timer)
    NVIC_ISER0 |= (1 << ((INT_EUSCIA3 - 16) & 31));	// enable euscia3 (spi)
    NVIC_ISER1 |= (1 << ((INT_PORT4 - 16) & 31));	// enable port4 (ready signal)
    PCMCTL1 &= ~LOCKLPM5;							// unlock ports

    __enable_interrupts();							// enable interrupts
}

void dynamixel_init(void)
{
//	set_return(0xFE, 0x01);
//	joint_mode(0xFE);
//	torque_enable(0xFE);
//	led_on(0xFE);
//	__delay_cycles(48000000); 						// LEDs stay on for 1 second
//	led_off(0xFE);
}
