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

/* external variables that can be accessed from main */
uint8_t sync_ids[19] = { 0 };				// load sync write ids here
uint16_t sync_positions[19] = { 0 };		// load sync write positions here
uint16_t sync_speeds[19] = { 0 };			// load sync write speeds here
uint16_t sync_readings[19] = { 0 };			// load sync read positions here

/* private variables */
uint64_t packet = 0;						// global packet for sending
uint16_t return_packet = 0; 				// global packet for receiving
uint8_t checksum_1;							// global checksum for communication protocol one
uint16_t checksum_2; 						// global checksum for communication protocol two
uint8_t split = 0; 							// global split to define split in sync write

/* private interrupt vector holders */
uint8_t u, t, p, s;

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
void checksum_gen()
{
	if (GET_COMM(packet))		// generator for protocol two
	{
		uint16_t crc_accum = 0, i, j;
		switch(GET_INST(packet))
		{
			case PING:
			case ACTION:
			{
				uint8_t tx[] = { 0xFF, 0xFF, 0xFD, 0x00, GET_ID(packet),
						0x03, 0x00, GET_INST(packet) };
				for (j  = 0; j < 8; j++)
				{
					i = ((uint16_t)(crc_accum >> 8) ^ tx[j]) & 0xFF;
					crc_accum = (crc_accum << 8) ^ lookup[i];
				}
				break;
			}
			case RESET:
			{
				uint8_t tx[] = { 0xFF, 0xFF, 0xFD, 0x00, GET_ID(packet),
						0x04, 0x00, GET_INST(packet), GET_1(packet) };
				for (j  = 0; j < 9; j++)
				{
					i = ((uint16_t)(crc_accum >> 8) ^ tx[j]) & 0xFF;
					crc_accum = (crc_accum << 8) ^ lookup[i];
				}
				break;
			}
			case SYNC_WRITE:
			{
				uint8_t tx[] = { 0xFF, 0xFF, 0xFD, 0x00, 0xFE,
							 GET_PARAM(packet), 0x00, SYNC_WRITE,
						 GOAL_POS, 0x00, 0x04, 0x00 };
				uint8_t k = 0, l = split;
				for (j = 0; j < GET_PARAM(packet)+5; j++)
				{
					if (j < 12)
						i = ((crc_accum >> 8) ^ tx[j]) & 0xFF;
					else
					{
						switch(k)
						{
							case 0:
								i = ((crc_accum >> 8) ^ sync_ids[l]) & 0xFF;
								k++;
								break;
							case 1:
								i = ((crc_accum >> 8) ^ GET_1(sync_positions[l])) & 0xFF;
								k++;
								break;
							case 2:
								i = ((crc_accum >> 8) ^ GET_2(sync_positions[l])) & 0xFF;
								k++;
								break;
							case 3:
								i = ((crc_accum >> 8) ^ GET_1(sync_speeds[l])) & 0xFF;
								k++;
								break;
							case 4:
								i = ((crc_accum >> 8) ^ GET_2(sync_speeds[l])) & 0xFF;
								k = 0;
								l++;
								break;
						}
					}
					crc_accum = (crc_accum << 8) ^ lookup[i];
				}
				l = 0;
				break;
			}
			case SYNC_READ:
			{
				uint8_t tx[] = { 0xFF, 0xFF, 0xFD, 0x00, 0xFE,
							 GET_PARAM(packet), 0x00, SYNC_READ,
						 XL_CURR_POS, 0x00, 0x02, 0x00 };
				for (j = 0; j < GET_1(packet)+12; j++)
				{
					if (j < 12)
						i = ((crc_accum >> 8) ^ tx[j]) & 0xFF;
					else
						i = ((crc_accum >> 8) ^ sync_ids[j-12]) & 0xFF;
					crc_accum = (crc_accum << 8) ^ lookup[i];
				}
				break;
			}
			default:
			{
				uint8_t tx[] = { 0xFF, 0xFF, 0xFD, 0x00, GET_ID(packet), GET_PARAM(packet) + 4, 0x00, GET_INST(packet),
						GET_REG(packet), 0x00, GET_1(packet), GET_2(packet), GET_3(packet),
						GET_4(packet) };

				for(j = 0; j < GET_PARAM(packet)+9; j++)
				{
						i = ((uint16_t)(crc_accum >> 8) ^ tx[j]) & 0xFF;
						crc_accum = (crc_accum << 8) ^ lookup[i];
				}
				break;
			}
		}
		checksum_2 = crc_accum;
	}
	else						// generator for protocol one
	{
		uint8_t i;
		switch(GET_INST(packet))
		{
			case PING:
			case ACTION:
			case RESET:
				checksum_1 = (~(GET_ID(packet) + 2 + GET_INST(packet)) & 0xFF);
				break;
			case SYNC_WRITE:
				checksum_1 = 0xFE + GOAL_POS + SYNC_WRITE + GET_PARAM(packet) + 4;
				for (i = 0; i < GET_1(packet); i++)
				{
					checksum_1 += sync_ids[i] + GET_1(sync_positions[i]) + GET_2(sync_positions[i])
							   + GET_1(sync_speeds[i]) + GET_2(sync_speeds[i]);
				}
				checksum_1 = ~checksum_1 & 0xFF;
				break;
			default:
				checksum_1 += GET_ID(packet) + GET_PARAM(packet) + 2 + GET_INST(packet) + GET_REG(packet);
				for (i = 0; i < 8*(GET_PARAM(packet)-1); i=i+8)
					checksum_1 += ((packet >> i) & 0xFF);
				checksum_1 = ~checksum_1 & 0xFF;
				break;
		}
	}
}

//////////////////// read/write primitives ////////////////////
void motor_write()
{
	while(UCA2STATW & UCBUSY);					// ensure not busy
	P6OUT |= BIT0;								// claim the bus
	//SCB_SCR_SLEEPONEXIT;				// don't wake up from isr
	UCA2IE |= UCTXIE;
	__wfi();
	while(UCA2STATW & UCBUSY);					// ensure not busy
	P6OUT &= ~BIT0;
}

void sync_write(uint8_t len)
{
	//SCB_SCR_SLEEPONEXIT;				// don't wake up from isr
	SET_INST(packet, SYNC_WRITE);

	/* the mode should be fouWnd at this point to determine sending */
	if (sync_ids[len-1] < 0x10)
	{
		SET_PARAM(packet, ((5*len) + 4));
		SET_1(packet, len);
		/* generate a protocol one checksum */
		checksum_gen();

		/* now send */
		while(UCA2STATW & UCBUSY);					// ensure not busy
		P6OUT |= BIT0;								// claim the bus
		UCA2IE |= UCTXIE;
		__wfi();
	}
	else if (sync_ids[0] >= 0x10)
	{
		SET_PARAM(packet, ((5*len) + 7));
		SET_1(packet, len);
		SET_COMM(packet, 0x01);
		/* generate a protocol two checksum */
		checksum_gen();

		/* now send */
		while(UCA2STATW & UCBUSY);					// ensure not busy
		P6OUT |= BIT0;								// claim the bus
		UCA2IE |= UCTXIE;
		__wfi();
	}
	else
	{
		uint64_t temp_packet, buffer;
		uint8_t i;
		/* finds the first id that is non-communication protocol one */
		for (i = 0; i < len; i++)
		{
			if (sync_ids[i] >= 0x10)			// i is now the first communication protocol two id
			{
				SET_PARAM(packet, ((5*i) + 4));
				SET_1(packet, i);
				split = i;
				break;
			}
		}
		/* generate a protocol one checksum */
		checksum_gen();
		temp_packet = packet;

		/* generate a protocol two checksum */
		CLEAR_1(packet);
		CLEAR_PARAM(packet);
		SET_1(packet, (len-split));
		SET_PARAM(packet, ((5*(len-split)) + 7));
		SET_COMM(packet, 0x01);
		checksum_gen();
		buffer = packet;
		packet = temp_packet;

		/* send communication protocol one */
		//SCB_SCR_SLEEPONEXIT;
		while(UCA2STATW & UCBUSY);					// ensure not busy
		P6OUT |= BIT0;								// claim the bus
		UCA2IE |= UCTXIE;
		__wfi();							// we need to sleep here in order to send communication 1, then 2.

		/* now send communication protocol two */
		while(UCA2STATW & UCBUSY);					// ensure not busy
		packet = buffer;
		//SCB_SCR_SLEEPONEXIT;
		UCA2IE |= UCTXIE;
		__wfi();
	}
	while(UCA2STATW & UCBUSY);					// ensure not busy
	P6OUT &= ~BIT0;
}

uint16_t motor_read()
{
	 motor_write();							// ask to read
	 //SCB_SCR_SLEEPONEXIT;		// don't wake up from isr
	 UCA2IE |= UCRXIE;
	 __wfi();
	 UCA2IE &= ~UCRXIE;
	 return (return_packet & 0xFFFF);
}

void sync_read(uint8_t len)
{
	uint8_t i = 0;

	/* at this point, the global packet should be made. */
	SET_ID(packet, 0xFE);
	SET_COMM(packet, 0x01);
	SET_PARAM(packet, len+7);
	SET_REG(packet, XL_CURR_POS);
	SET_INST(packet, SYNC_READ);
	SET_1(packet, len);

	/* then a checksum must be generated */
	checksum_gen();

	/* first, send a request to read */
	motor_write();

	/* now, begin reading */
	UCA2IE |= UCRXIE;
	do
	{
		//SCB_SCR_SLEEPONEXIT;		// don't wake up from isr
		__wfi();
		i++;
	}while(i < len);
	UCA2IE &= ~UCRXIE;
}

//////////////////// interrupt service routines ////////////////////
void uart()
{
	static uint8_t i = 0, k = 0, l = 0, error = 0;
	uint8_t temp;
	u = UCA2IV;
	switch(u)
	{
		case 0x02:
			if (GET_COMM(packet))		// comm. protocol two
			{
				if (GET_INST(packet) == SYNC_READ)
				{
					temp = UCA2RXBUF;
					i++;
					if (GET_1(packet) == 2)
					{
						switch(i)
						{
							case  9: error = temp; 		  break;
							case 10: SET_1(sync_readings[k], temp); break;
							case 11: SET_2(sync_readings[k], temp); break;
							default:					  break;
						}
						if (error)
							while(1);
						if (i >= 13)
						{
							UCA2IE &= ~UCTXIE;
							k++;
							if (k == (GET_1(packet)))
								k = 0;
						}
					}
					else
					{
						switch(i)
						{
							case  9: error = temp; 		  break;
							case 10: SET_1(sync_readings[k], temp); break;
							default:					  break;
						}
						if (error)
							while(1);
						if (i >= 12)
						{
							UCA2IE &= ~UCTXIE;
							k++;
							if (k == (GET_1(packet)))
								k = 0;
						}
					}
				}
				else
				{
					temp = UCA2RXBUF;
					i++;
					if (GET_1(packet) == 2)
					{
						switch(i)
						{
							case  9: error = temp; 		  break;
							case 10: SET_1(return_packet, temp); break;
							case 11: SET_2(return_packet, temp); break;
							default:					  break;
						}
						if (error)
							while(1);
						if (i >= 13)
						{
							UCA2IE &= ~UCTXIE;
							k++;
							if (k == (GET_1(packet)))
								k = 0;
						}
					}
					else
					{
						switch(i)
						{
							case  9: error = temp; 		  break;
							case 10: SET_1(return_packet, temp); break;
							default: 					  break;
						}
						if (error)
							while(1);
						if (i >= 13)
						{
							UCA2IE &= ~UCTXIE;
							k++;
							if (k == (GET_1(packet)))
								k = 0;
						}
					}
					if (error)
						while(1);
				}
			}
			else						// comm. protocol one
			{
				temp = UCA2RXBUF;
				i++;
				if (GET_1(packet) == 2)
				{
					switch(i)
					{
						case 5: error = temp; 		  break;
						case 6: SET_1(return_packet, temp);  break;
						case 7: SET_2(return_packet, temp);  break;
						default: 					  break;
					}
					if (error)
						while(1);
					if (i >= 13)
					{
						UCA2IE &= ~UCTXIE;
						k++;
						if (k == (GET_1(packet)))
							k = 0;
					}
				}
				else
				{
					switch(i)
					{
						case 5: error = temp; 		  break;
						case 6: SET_1(return_packet, temp);  break;
						default: 					  break;
					}
					if (error)
						while(1);
					if (i >= 13)
					{
						UCA2IE &= ~UCTXIE;
						k++;
						if (k == (GET_1(packet)))
							k = 0;
					}
				}
				if (error)
					while(1);
			}
			break;
		case 0x04:
			if (GET_COMM(packet))		// comm. protocol two
			{
				/* these don't use packets. */
				if (GET_INST(packet) == SYNC_WRITE)
				{
					uint8_t tx[] = { 0xFF, 0xFF, 0xFD, 0x00, 0xFE, GET_PARAM(packet), 0x00, SYNC_WRITE, GOAL_POS, 0x00, 0x04, 0x00 };
					if (i < 12)
						UCA2TXBUF = tx[i];
					else if (i <= ((5*GET_1(packet)) + 11))
					{
						switch(k)
						{
							case 0:
								UCA2TXBUF = sync_ids[split];
								k++;
								break;
							case 1:
								UCA2TXBUF = GET_1(sync_positions[split]);
								k++;
								break;
							case 2:
								UCA2TXBUF = GET_2(sync_positions[split]);
								k++;
								break;
							case 3:
								UCA2TXBUF = GET_1(sync_speeds[split]);
								k++;
								break;
							case 4:
								UCA2TXBUF = GET_2(sync_speeds[split]);
								k = 0;
								split++;
								break;
						}
					}
					i++;
					if (i == ((5*GET_1(packet)) + 12))
						UCA2TXBUF = GET_1(checksum_2);
					if (i == ((5*GET_1(packet)) + 13))
						UCA2TXBUF = GET_2(checksum_2);
					if (i == ((5*GET_1(packet)) + 14))
					{
						i = split = 0;
						UCA2IE &= ~UCTXIE;
					}
				}
				else if (GET_INST(packet) == SYNC_READ)
				{
					uint8_t tx[] = { 0xFF, 0xFF, 0xFD, 0x00, 0xFE, GET_PARAM(packet), 0x00, SYNC_READ, XL_CURR_POS, 0x00, 0x02, 0x00 };
					if (i < 12)
						UCA2TXBUF = tx[i];
					else if (i <= (GET_1(packet)) + 11)
						UCA2TXBUF = sync_ids[i-12];
					i++;
					if (i == ((GET_1(packet)) + 12))
						UCA2TXBUF = GET_1(checksum_2);
					if (i == ((GET_1(packet)) + 13))
						UCA2TXBUF = GET_2(checksum_2);
					if (i == (GET_1(packet)+14))
					{
						i = 0;
						UCA2IE &= ~UCTXIE;
					}
				}
				else
				{
					uint8_t tx[] = { 0xFF, 0xFF, 0xFD, 0x00, GET_ID(packet), GET_PARAM(packet) + 4, 0x00, GET_INST(packet), GET_REG(packet), 0x00, GET_1(packet), GET_2(packet), GET_3(packet), GET_4(packet), GET_1(checksum_2), GET_2(checksum_2) };
					UCA2TXBUF = tx[i];
					i++;
					if (((GET_PARAM(packet) == 0) && (i == 8))   ||
						((GET_PARAM(packet) == 2) && (i == 11))  ||
						((GET_PARAM(packet) == 3) && (i == 12))  ||
						((GET_PARAM(packet) == 4) && (i == 13)))
						i = 14;
					if (i == 16)
					{
						i = 0;
						UCA2IE &= ~UCTXIE;
					}
				}
			}
			else						// comm. protocol one
			{
				if (GET_INST(packet) == SYNC_WRITE)
				{
					uint8_t tx[] = { 0xFF, 0xFF, 0XFE, GET_PARAM(packet), SYNC_WRITE, GOAL_POS, 0x04 };
					if (i < 7)
						UCA2TXBUF = tx[i];
					else if (i <= (5*GET_1(packet) + 6))
					{
						switch(k)
						{
							case 0:
								UCA2TXBUF = sync_ids[l];
								k++;
								break;
							case 1:
								UCA2TXBUF = GET_1(sync_positions[l]);
								k++;
								break;
							case 2:
								UCA2TXBUF = GET_2(sync_positions[l]);
								k++;
								break;
							case 3:
								UCA2TXBUF = GET_1(sync_speeds[l]);
								k++;
								break;
							case 4:
								UCA2TXBUF = GET_2(sync_speeds[l]);
								k = 0;
								l++;
								break;
						}
					}
					i++;
					if (i == ((5*GET_1(packet)) + 7))
					{
						UCA2TXBUF = checksum_1;
						i = 0;
						UCA2IE &= ~UCTXIE;
					}
				}
				else
				{
					uint8_t tx[] = { 0xFF, 0xFF, GET_ID(packet), GET_PARAM(packet) + 2, GET_INST(packet), GET_REG(packet), GET_1(packet), GET_2(packet), GET_3(packet), GET_4(packet), checksum_1 };

					UCA2TXBUF = tx[i];
					i++;

					if (((GET_PARAM(packet) == 0) && (i == 5))   ||
						((GET_PARAM(packet) == 2) && (i == 7))   ||
						((GET_PARAM(packet) == 3) && (i == 8))   ||
						((GET_PARAM(packet) == 4) && (i == 9)))
						i = 10;
					if (i == 11)
					{
						i = 0;
						UCA2IE &= ~UCTXIE;
					}
				}
			}
			break;
	}
}

void spi()
{
//	uint8_t temp; 	// this is temporary
//	s = UCB0IV;
//	switch(s)
//	{
//		case UCRXIFG: temp = UCB0RXBUF; break;
//		case UCTXIFG: UCB0TXBUF = 'U';  break;
//	}
	__no_operation();
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
