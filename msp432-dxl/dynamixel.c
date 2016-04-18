/*
 * author: Brian Ruiz
 * date:   February 21, 2015
 * school: George Mason University
 *
 * this file contains the read/write primitives needed to make communication
 * with the dynamixels possible. these apis both work with communication protocols
 * one and two, however, all communication protocol one motors must have an id less than
 * 0x10, while the rest may be communication protocol two.
 *
 * this file also contains the interrupt service routines initialized in init.c
 */

#include <stdint.h>
#include <msp.h>
#include "dynamixel.h"
#include "gestures.h"

/* event register to command all tasks */
uint8_t event_reg = UART_READY;

/* external variables that can be accessed from main */
uint8_t sync_ids[8] = { 0 };				// load sync write ids here
uint16_t sync_positions[8] = { 0 };			// load sync write positions here
uint16_t sync_speeds[8] = { 0 };			// load sync write speeds here
uint16_t goal_positions[8] = { 0x200, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800, 0x800 }; // load theoretical goal positions here
uint8_t sync_len = 0;						// load sync write length here
uint8_t xl_len = 0; 						// load length of xl parameters here

uint8_t g_id = 0;							// load hand gesture here
uint8_t read_id = 0; 						// load desired id to read from here

/* reading variables */
uint16_t readings[8] = { 0 };				// load current positions here
float rad_readings[8] = { 0 };				// load current positions (in radians) here
uint8_t checkpoint = 0; 					// increments based on number of movements performed/read
uint16_t error = 0; 						// indicates that an error has taken place

/* private variables */
uint16_t checksum = 0; 				// global checksum for communication protocol two

/* private interrupt vector holder */
uint8_t u;

/* lookup table for xl-320 checksum */
static const uint16_t lookup[256] =
{
	0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
	0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
	0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
	0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
	0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
	0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
	0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
	0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
	0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
	0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
	0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
	0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
	0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
	0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
	0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
	0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
	0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
	0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
	0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
	0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
	0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
	0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
	0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
	0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
	0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
	0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
	0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
	0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
	0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
	0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
	0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
	0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
};

//////////////////// checksum generator ////////////////////
void checksum_gen(uint8_t byte)
{
	uint8_t i;
	i = ((uint16_t)(checksum >> 8) ^ byte) & 0xFF;
	checksum = (checksum << 8) ^ lookup[i];
}

//////////////////// interrupt service routine ////////////////////
void uart()
{
	/* receiving variables */
	uint8_t rx;
	static uint8_t j = 0;

	/* transmitting variables */
	static uint8_t i = 0;
	static uint8_t accum = 0;
	static uint8_t id = 0;
	static uint8_t header = 1;
	uint8_t tx[] = { 0xFF, 0xFF, 0xFE, (5*sync_len)+4, SYNC_WRITE, GOAL_POS, 4 };
	uint8_t read_tx[] = { 0xFF, 0xFF, read_id, 4, READ, CURR_POS, 2 };
	uint8_t head[] = { 0xFF, 0xFF, 0xFD, 0x00, 0xFE, xl_len, 0x00, SYNC_WRITE, GOAL_POS, 0x00, 0x04, 0x00 };

	__enable_interrupts(); 		/* the scheduler/spi should be able to interrupt this.
								 * if not, we could get deadlocked... */

	u = UCA1IV;
	if (u == 0x02) 				/* receiving info from motors */
	{
		event_reg = UART_READING;
		rx = UCA1RXBUF;
		switch(j)
		{
			case 0:
			case 1:
				if (rx != 0xFF)						// first two packets are 0xFF; if not...
				{
					SYSTICK_STCSR &= ~SysTick_CTRL_TICKINT_Msk;		// turn off the scheduler (for now).
					event_reg = ERROR;
					UCA1IE &= ~(UCTXIE | UCRXIE);	// turn off interrupts
					error |= BIT8;					// synchronization error
					P10OUT |= BIT0;					// throw error pin high
				}
				else
					j++;							// if no error, just increment
				break;
			case 2:
				accum += rx;						// third packet is the id
				read_id = rx;						// save that info
				j++;
				break;
			case 3:
				if (rx != 0x04)						// length should always be four; if not...
				{
					SYSTICK_STCSR &= ~SysTick_CTRL_TICKINT_Msk;		// turn off the scheduler (for now).
					event_reg = ERROR;
					UCA1IE &= ~(UCTXIE | UCRXIE);	// turn off interrupts
					error |= BIT8;					// synchronization error
					P10OUT |= BIT0;					// throw error pin high
				}
				else
				{
					accum += rx;
					j++;
				}
				break;
			case 4:
				error |= rx;
				accum += rx;
				j++;
				break;
			case 5:
				accum += rx;
				SET_1(readings[read_id], rx);
				j++;
				break;
			case 6:
				accum += rx;
				SET_2(readings[read_id], rx);
				j++;
				break;
			case 7:
				j = 0;
				if (rx == (~accum))
				{
					if (read_id != 7)
					{
						event_reg = UART_READ;
						read_id++;
						__delay_cycles(10);
						UCA1IE &= ~UCRXIE;
						UCA1IE |= UCTXIE;
					}
					else
					{
						event_reg = UART_READ_DONE;
						read_id = 0;
						UCA1IE &= ~UCRXIE;
					}
				}
				else
				{
					SYSTICK_STCSR &= ~SysTick_CTRL_TICKINT_Msk;		// turn off the scheduler (for now).
					event_reg = ERROR;
					error |= BIT4;
					UCA1IE &= ~UCRXIE;
				}
				break;
		}
	}
	else						/* transmitting info to motors */
	{
		switch(event_reg)
		{
			case UART_READY:
				i = accum = 0;
				header = 1;
				P2OUT |= BIT1;
				__delay_cycles(10);
				event_reg = UART_SENDING;
			case UART_SENDING:
				if (g_id && header)
				{
					UCA1TXBUF = head[i];
					checksum_gen(head[i]);
					i++;
					if (i == 12)
						header = i = 0; 	// sending header bytes is now done.
				}
				else if (g_id && (!header))
				{
					switch(g_id)
					{
						case 1: 		/* curl */
							if (i < curl[0])
							{
								i++;
								UCA1TXBUF = curl[i];
								checksum_gen(curl[i]);
							}
							else if (i == curl[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									/* CHANGE THIS BACK TO SEND DONE! */
									event_reg = UART_READ_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 2: 		/* open */
							if (i < open[0])
							{
								i++;
								UCA1TXBUF = open[i];
								checksum_gen(open[i]);
							}
							else if (i == open[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									/* CHANGE THIS BACK TO SEND DONE! */
									event_reg = UART_READ_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 3:			/* thumbs up */
							if (i < thumbs_up[0])
							{
								i++;
								UCA1TXBUF = thumbs_up[i];
								checksum_gen(thumbs_up[i]);
							}
							else if (i == thumbs_up[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									/* CHANGE THIS BACK TO SEND DONE! */
									event_reg = UART_READ_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 4: 		/* point */
							if (i < point[0])
							{
								i++;
								UCA1TXBUF = point[i];
								checksum_gen(point[i]);
							}
							else if (i == point[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 5:			/* okay */
							if (i < okay[0])
							{
								i++;
								UCA1TXBUF = okay[i];
								checksum_gen(okay[i]);
							}
							else if (i == okay[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 6:
							if (i < letter_a[0])
							{
								i++;
								UCA1TXBUF = letter_a[i];
								checksum_gen(letter_a[i]);
							}
							else if (i == letter_a[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 7:
							if (i < letter_b[0])
							{
								i++;
								UCA1TXBUF = letter_b[i];
								checksum_gen(letter_b[i]);
							}
							else if (i == letter_b[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 8:
							if (i < letter_c[0])
							{
								i++;
								UCA1TXBUF = letter_c[i];
								checksum_gen(letter_c[i]);
							}
							else if (i == letter_c[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 9:
							if (i < letter_d[0])
							{
								i++;
								UCA1TXBUF = letter_d[i];
								checksum_gen(letter_d[i]);
							}
							else if (i == letter_d[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 10:
							if (i < letter_e[0])
							{
								i++;
								UCA1TXBUF = letter_e[i];
								checksum_gen(letter_e[i]);
							}
							else if (i == letter_e[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 11:
							if (i < letter_f[0])
							{
								i++;
								UCA1TXBUF = letter_f[i];
								checksum_gen(letter_f[i]);
							}
							else if (i == letter_f[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 12:
							if (i < letter_g[0])
							{
								i++;
								UCA1TXBUF = letter_g[i];
								checksum_gen(letter_g[i]);
							}
							else if (i == letter_g[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 13:
							if (i < letter_h[0])
							{
								i++;
								UCA1TXBUF = letter_h[i];
								checksum_gen(letter_h[i]);
							}
							else if (i == letter_h[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 14:
							if (i < letter_i[0])
							{
								i++;
								UCA1TXBUF = letter_i[i];
								checksum_gen(letter_i[i]);
							}
							else if (i == letter_i[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 15:
							if (i < letter_k[0])
							{
								i++;
								UCA1TXBUF = letter_k[i];
								checksum_gen(letter_k[i]);
							}
							else if (i == letter_k[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 16:
							if (i < letter_l[0])
							{
								i++;
								UCA1TXBUF = letter_l[i];
								checksum_gen(letter_l[i]);
							}
							else if (i == letter_l[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 17:
							if (i < letter_n[0])
							{
								i++;
								UCA1TXBUF = letter_n[i];
								checksum_gen(letter_n[i]);
							}
							else if (i == letter_n[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
						case 18:
							if (i < letter_o[0])
							{
								i++;
								UCA1TXBUF = letter_o[i];
								checksum_gen(letter_o[i]);
							}
							else if (i == letter_o[0])
							{
								i++;
								UCA1TXBUF = GET_1(checksum);
							}
							else
							{
								UCA1TXBUF = GET_2(checksum);
								g_id = i = checksum = 0;
								if (!sync_len)
								{
									event_reg = UART_SEND_DONE;
									UCA1IE &= ~UCTXIE;
									header = 1;
									__delay_cycles(110);
									P2OUT &= ~BIT1;
								}
							}
							break;
//						case 19:
//							if (i < letter_r[0])
//							{
//								i++;
//								UCA1TXBUF = letter_r[i];
//								checksum_gen(letter_r[i]);
//							}
//							else if (i == letter_r[0])
//							{
//								i++;
//								UCA1TXBUF = GET_1(checksum);
//							}
//							else
//							{
//								UCA1TXBUF = GET_2(checksum);
//								g_id = i = checksum = 0;
//								if (!sync_len)
//								{
//									event_reg = UART_SEND_DONE;
//									UCA1IE &= ~UCTXIE;
//									header = 1;
//									__delay_cycles(110);
//									P2OUT &= ~BIT1;
//								}
//							}
//							break;
//						case 20:
//							if (i < letter_s[0])
//							{
//								i++;
//								UCA1TXBUF = letter_s[i];
//								checksum_gen(letter_s[i]);
//							}
//							else if (i == letter_s[0])
//							{
//								i++;
//								UCA1TXBUF = GET_1(checksum);
//							}
//							else
//							{
//								UCA1TXBUF = GET_2(checksum);
//								g_id = i = checksum = 0;
//								if (!sync_len)
//								{
//									event_reg = UART_SEND_DONE;
//									UCA1IE &= ~UCTXIE;
//									header = 1;
//									__delay_cycles(110);
//									P2OUT &= ~BIT1;
//								}
//							}
//							break;
//						case 21:
//							if (i < letter_t[0])
//							{
//								i++;
//								UCA1TXBUF = letter_t[i];
//								checksum_gen(letter_t[i]);
//							}
//							else if (i == letter_t[0])
//							{
//								i++;
//								UCA1TXBUF = GET_1(checksum);
//							}
//							else
//							{
//								UCA1TXBUF = GET_2(checksum);
//								g_id = i = checksum = 0;
//								if (!sync_len)
//								{
//									event_reg = UART_SEND_DONE;
//									UCA1IE &= ~UCTXIE;
//									header = 1;
//									__delay_cycles(110);
//									P2OUT &= ~BIT1;
//								}
//							}
//							break;
//						case 22:
//							if (i < letter_u[0])
//							{
//								i++;
//								UCA1TXBUF = letter_u[i];
//								checksum_gen(letter_u[i]);
//							}
//							else if (i == letter_u[0])
//							{
//								i++;
//								UCA1TXBUF = GET_1(checksum);
//							}
//							else
//							{
//								UCA1TXBUF = GET_2(checksum);
//								g_id = i = checksum = 0;
//								if (!sync_len)
//								{
//									event_reg = UART_SEND_DONE;
//									UCA1IE &= ~UCTXIE;
//									header = 1;
//									__delay_cycles(110);
//									P2OUT &= ~BIT1;
//								}
//							}
//							break;
//						case 23:
//							if (i < letter_v[0])
//							{
//								i++;
//								UCA1TXBUF = letter_v[i];
//								checksum_gen(letter_v[i]);
//							}
//							else if (i == letter_v[0])
//							{
//								i++;
//								UCA1TXBUF = GET_1(checksum);
//							}
//							else
//							{
//								UCA1TXBUF = GET_2(checksum);
//								g_id = i = checksum = 0;
//								if (!sync_len)
//								{
//									event_reg = UART_SEND_DONE;
//									UCA1IE &= ~UCTXIE;
//									header = 1;
//									__delay_cycles(110);
//									P2OUT &= ~BIT1;
//								}
//							}
//							break;
//						case 24:
//							if (i < letter_x[0])
//							{
//								i++;
//								UCA1TXBUF = letter_x[i];
//								checksum_gen(letter_x[i]);
//							}
//							else if (i == letter_x[0])
//							{
//								i++;
//								UCA1TXBUF = GET_1(checksum);
//							}
//							else
//							{
//								UCA1TXBUF = GET_2(checksum);
//								g_id = i = checksum = 0;
//								if (!sync_len)
//								{
//									event_reg = UART_SEND_DONE;
//									UCA1IE &= ~UCTXIE;
//									header = 1;
//									__delay_cycles(110);
//									P2OUT &= ~BIT1;
//								}
//							}
//							break;
//						case 25:
//							if (i < letter_y[0])
//							{
//								i++;
//								UCA1TXBUF = letter_y[i];
//								checksum_gen(letter_y[i]);
//							}
//							else if (i == letter_y[0])
//							{
//								i++;
//								UCA1TXBUF = GET_1(checksum);
//							}
//							else
//							{
//								UCA1TXBUF = GET_2(checksum);
//								g_id = i = checksum = 0;
//								if (!sync_len)
//								{
//									event_reg = UART_SEND_DONE;
//									UCA1IE &= ~UCTXIE;
//									header = 1;
//									__delay_cycles(110);
//									P2OUT &= ~BIT1;
//								}
//							}
//							break;
						default:
							SYSTICK_STCSR &= ~SysTick_CTRL_TICKINT_Msk;		// turn off the scheduler (for now).
							event_reg = ERROR;
							UCA1IE &= ~(UCTXIE | UCRXIE);	// turn off interrupts
							error |= BIT8;					// synchronization error
							P10OUT |= BIT0;					// throw error pin high
							break;
					}
				}
				else if ((sync_len) && (id != sync_len))
				{
					if (i < 2)
					{
						UCA1TXBUF = tx[i];
						i++;
					}
					else if ((i >= 2) && (i < 7))
					{
						UCA1TXBUF = tx[i];
						accum += UCA1TXBUF;
						i++;
					}
					else
					{
						if (((i-7)%5) == 0)
						{
							UCA1TXBUF = sync_ids[id];
							accum += UCA1TXBUF;
						}
						else if (((i-7)%5) == 1)
						{
							UCA1TXBUF = GET_1(sync_positions[id]);
							accum += UCA1TXBUF;
						}
						else if (((i-7)%5) == 2)
						{
							UCA1TXBUF = GET_2(sync_positions[id]);
							accum += UCA1TXBUF;
						}
						else if (((i-7)%5) == 3)
						{
							UCA1TXBUF = GET_1(sync_speeds[id]);
							accum += UCA1TXBUF;
						}
						else
						{
							UCA1TXBUF = GET_2(sync_speeds[id]);
							accum += UCA1TXBUF;
							id++;
						}
						i++;
					}
				}
				else
				{
					UCA1TXBUF = ~accum;
					UCA1IE &= ~UCTXIE;
					event_reg = UART_SEND_DONE;
					i = id = accum = sync_len = 0;
					header = 1;
					__delay_cycles(110);
					P2OUT &= ~BIT1;
				}
				break;
			case UART_READ:
				P2OUT |= BIT1;
				__delay_cycles(10);
				if (i < 2)
				{
					UCA1TXBUF = read_tx[i];
					i++;
				}
				else if ((i >= 2) && (i < 7))
				{
					UCA1TXBUF = read_tx[i];
					accum += UCA1TXBUF;
					i++;
				}
				else
				{
					UCA1TXBUF = ~accum;
					i = accum = 0;
					UCA1IE &= ~UCTXIE;
					__delay_cycles(110);
					P2OUT &= ~BIT1;
					UCA1IE |= UCRXIE;
				}
				break;
		}
	}
}
