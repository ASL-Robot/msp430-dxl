#include <stdint.h>
#include <msp430.h>
#include "dynamixel.h"

/* for sync_write */
extern uint8_t sync_ids[19] = { 0 };
extern uint16_t sync_positions[19] = { 0 };
extern uint16_t sync_speeds[19] = { 0 };

/* for sync_read, for xl-320s only! */
extern uint16_t sync_readings[19] = { 0 };

/* lookup table for xl-320 checksum */
#pragma PERSISTENT(lookup)
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


/* checksum generator */
uint16_t checksum_gen(uint64_t packet)
{
	if (GET_ID(packet) == 0xFE)
	{
		static uint8_t x = 0x01;
		if (x)
		{
			x ^= 1;
			if (GET_INST(packet) == PING || GET_INST(packet) == ACTION)
				return (~(GET_ID(packet) + 2 + GET_INST(packet)) & 0xFF);
			switch(GET_PARAM(packet))
			{
				case 2:
					return (~(GET_ID(packet) + 4 + GET_INST(packet) + GET_REG(packet) + GET_1(packet)) & 0xFF);
				case 3:
					return (~(GET_ID(packet) + 5 + GET_INST(packet) + GET_REG(packet) + GET_1(packet) + GET_2(packet))
					& 0xFF);
				case 4:
					return (~(GET_ID(packet) + 6 + GET_INST(packet) + GET_REG(packet) + GET_1(packet) + GET_2(packet) + GET_3(packet)) & 0xFF);
				case 5:
					return (~(GET_ID(packet) + 7 + GET_INST(packet) + GET_REG(packet) + GET_1(packet) + GET_2(packet) + GET_3(packet) + GET_4(packet)) & 0xFF);
			}
		}
		else
		{
			x ^= 1;
			uint8_t len = GET_PARAM(packet) + 4;		// if ping or action, this should overflow: 0xFF + 4 = 0x03.
			uint16_t crc_accum = 0, i, j;

			uint8_t tx[] = { 0xFF, 0xFF, 0xFD, 0x00, GET_ID(packet), len, 0x00, GET_INST(packet),
					GET_REG(packet), 0x00, GET_1(packet), GET_2(packet), GET_3(packet),
					GET_4(packet) };

			for(j = 0; j < len+5; j++)
			{
    				i = ((uint16_t)(crc_accum >> 8) ^ tx[j]) & 0xFF;
        			crc_accum = (crc_accum << 8) ^ lookup[i];
    		}
    		return crc_accum;
		}
	}

	else if (GET_ID(packet) < 0x07)
	{
		if (GET_INST(packet) == PING || GET_INST(packet) == ACTION)
			return (~(GET_ID(packet) + 2 + GET_INST(packet)) & 0xFF);
		switch(GET_PARAM(packet))
		{
			case 2:
				return (~(GET_ID(packet) + 4 + GET_INST(packet) + GET_REG(packet) + GET_1(packet)) & 0xFF);
			case 3:
				return (~(GET_ID(packet) + 5 + GET_INST(packet) + GET_REG(packet) + GET_1(packet) + GET_2(packet))
				& 0xFF);
			case 4:
				return (~(GET_ID(packet) + 6 + GET_INST(packet) + GET_REG(packet) + GET_1(packet) + GET_2(packet) + GET_3(packet)) & 0xFF);
			case 5:
				return (~(GET_ID(packet) + 7 + GET_INST(packet) + GET_REG(packet) + GET_1(packet) + GET_2(packet) + GET_3(packet) + GET_4(packet)) & 0xFF);
		}
	}

	else
	{
		uint8_t len = GET_PARAM(packet) + 4;		// if ping or action, this should overflow: 0xFF + 4 = 0x03.
		uint16_t crc_accum = 0, i, j;

		uint8_t tx[] = { 0xFF, 0xFF, 0xFD, 0x00, GET_ID(packet), len, 0x00, GET_INST(packet),
				GET_REG(packet), 0x00, GET_1(packet), GET_2(packet), GET_3(packet),
				GET_4(packet) };

		for(j = 0; j < len+5; j++)
		{
    			i = ((uint16_t)(crc_accum >> 8) ^ tx[j]) & 0xFF;
        		crc_accum = (crc_accum << 8) ^ lookup[i];
    	}
    	return crc_accum;
	}
	return 0;
}

void motor_write(uint64_t packet, uint8_t crc_l, uint8_t crc_h)
{
	while(UCA0STATW & UCBUSY);			// ensure not busy
	uint8_t i;
	P3OUT |= BIT2;						// claim the bus

	if (GET_ID(packet) == 0xFE)
	{
		static uint8_t x = 0x01;
		if (x)
		{
			x ^= 1;
			/* send start condition */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = 0xFF;
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = 0xFF;

			/* send id */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = GET_ID(packet);

			/* send packet length */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = GET_PARAM(packet) + 2;

			/* send instruction type */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = GET_INST(packet);

			/* send register to write to */
			if (GET_INST(packet) == PING || GET_INST(packet) == ACTION)
			{
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = crc_l;
			}
			else
			{
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = GET_REG(packet);

				/* send parameters */
				for (i = 0; i < 8*(GET_PARAM(packet)-1); i=i+8)
				{
					while(!(UCA0IFG & UCTXIFG));
					UCA0TXBUF = ((packet >> i) & 0xFF);
				}

				/* send checksum */
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = crc_l;
			}
		}

		else
		{
			x ^= 1;

			/* send start condition */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = 0xFF;
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = 0xFF;
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = 0xFD;
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = 0x00;

			/* send id */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = GET_ID(packet);

			/* send packet length */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = GET_PARAM(packet) + 4;		// if ping or action, this should overflow: 0xFF + 4 = 0x03.
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = 0x00;

			/* send instruction */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = GET_INST(packet);

			/* send register to write to */
			if (GET_INST(packet) == PING || GET_INST(packet) == ACTION)
			{
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = crc_l;
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = crc_h;
			}
			else
			{
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = GET_REG(packet);
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = 0x00;

				/* send parameters */
				for (i = 0; i < 8*(GET_PARAM(packet)-1); i=i+8)
				{
					while(!(UCA0IFG & UCTXIFG));
					UCA0TXBUF = ((packet >> i) & 0xFF);
				}

				/* send checksum */
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = crc_l;
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = crc_h;
			}
		}
	}

	else if (GET_ID(packet) < 0x07)
	{
		/* send start condition */
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = 0xFF;
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = 0xFF;

		/* send id */
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = GET_ID(packet);

		/* send packet length */
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = GET_PARAM(packet) + 2;

		/* send instruction type */
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = GET_INST(packet);

		/* send register to write to */
		if (GET_INST(packet) == PING || GET_INST(packet) == ACTION)
		{
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = crc_l;
		}
		else
		{
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = GET_REG(packet);

			/* send parameters */
			for (i = 0; i < 8*(GET_PARAM(packet)-1); i=i+8)
			{
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = ((packet >> i) & 0xFF);
			}

			/* send checksum */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = crc_l;
		}
	}

	else
	{
		/* send start condition */
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = 0xFF;
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = 0xFF;
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = 0xFD;
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = 0x00;

		/* send id */
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = GET_ID(packet);

		/* send packet length */
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = GET_PARAM(packet) + 4;		// if ping or action, this should overflow: 0xFF + 4 = 0x03.
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = 0x00;

		/* send instruction */
		while(!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = GET_INST(packet);

		/* send register to write to */
		if (GET_INST(packet) == PING || GET_INST(packet) == ACTION)
		{
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = crc_l;
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = crc_h;
		}
		else
		{
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = GET_REG(packet);
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = 0x00;

			/* send parameters */
			for (i = 0; i < 8*(GET_PARAM(packet)-1); i=i+8)
			{
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = ((packet >> i) & 0xFF);
			}

			/* send checksum */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = crc_l;
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = crc_h;
		}
	}
	while(UCA0STATW & UCBUSY);
	P3OUT &= ~BIT2;								// give the bus to the motor
}


void sync_write(uint8_t len)
{
	uint8_t i, mode, split, length;
	uint16_t checksum = 0;

	if (sync_ids[len-1] < 0x07)
		mode = 0;
	else if (sync_ids[0] >= 0x07)
		mode = 1;
	else
	{
		mode = 2;
		for (i = 0; i < len; i++)
		{
			if (sync_ids[i] >= 0x07)
			{
				split = i;
				break;
			}
		}
	}

	switch(mode)
	{
		case 0:				// communication protocol one
			length = (len*5) + 4;
			checksum += 0xFE + GOAL_POS + SYNC_WRITE + length + 4;

			/* send start condition */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = 0xFF;
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = 0xFF;
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = 0xFE;

			/* send length */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = length;

			/* send instruction */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = SYNC_WRITE;

			/* send register to write to */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = GOAL_POS; 	// should always be this

			/* send number of packets per id */
			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = 0x04;		// should always be four

			/* sync write time! */
			for (i = 0; i < len; i++)
			{
				/* send id */
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = sync_ids[i];
				checksum += sync_ids[i];

				/* send position */
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = XL_GET_1(sync_positions[i]);
				checksum += XL_GET_1(sync_positions[i]);
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = XL_GET_2(sync_positions[i]);
				checksum += XL_GET_2(sync_positions[i]);

				/* send speed */
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = XL_GET_1(sync_speeds[i]);
				checksum += XL_GET_1(sync_speeds[i]);
				while(!(UCA0IFG & UCTXIFG));
				UCA0TXBUF = XL_GET_2(sync_speeds[i]);
				checksum += XL_GET_2(sync_speeds[i]);
			}

			while(!(UCA0IFG & UCTXIFG));
			UCA0TXBUF = ~checksum;
			while(UCA0STATW & UCBUSY);
			P3OUT &= ~BIT2;								// give the bus to the motor

			break;
		case 1:				// communication protocol two
			break;
		case 2: 			// communication protocols one and two
			break;
	}
}

uint16_t motor_read(uint64_t packet, uint8_t crc_l, uint8_t crc_h)
{
	uint64_t status = GET_ID(packet) << 56; // packet to be returned
	uint8_t i = 0, temp;
	motor_write(packet, crc_l, crc_h);		// ask to read

	if (GET_ID(packet) < 7)
	{
		if (GET_1(packet) == 1)				// asked to read one byte
		{
			while(i < 7)
			{
				while(!(UCA0IFG & UCRXIFG));
				temp = UCA0RXBUF;
				i++;
				switch (i)
				{
					case 5:
						SET_ERROR(status, temp);// error return
						break;
					case 6:
						SET_1(status, temp);	// parameter return
						break;
					default:
						break;					// discard
				}
			}
		}
		else if (GET_1(packet) == 2)			// asked to read two bytes
		{
			while(i < 8)
			{
				while(!(UCA0IFG & UCRXIFG));
				temp = UCA0RXBUF;
				i++;
				switch (i)
				{
					case 5:
						SET_ERROR(status, temp);// error return
						break;
					case 6:
						SET_1(status, temp);	// first parameter return
						break;
					case 7:
						SET_2(status, temp);	// second parameter return
						break;
					default:
						break;					// discard
				}
			}
		}
		else
			trap_error(0x40); 					// this should never happen.
		if (GET_ERROR(status))					// oops...
			trap_error(GET_ERROR(status));
		while(UCA0STATW & UCBUSY);				// ensure everything was received
		return status;
	}

	else
	{
		if (GET_1(packet) == 1)					// asked to read one byte
		{
			while(i < 12)
			{
				while(!(UCA0IFG & UCRXIFG));
				temp = UCA0RXBUF;
				i++;
				switch (i)
				{
					case 9:
						SET_ERROR(status, temp);// error return
						break;
					case 10:
						SET_1(status, temp);	// parameter return
						break;
					default:
						break;					// discard
				}
			}
		}
		else if (GET_1(packet) == 2)			// asked to read two bytes
		{
			while(i < 13)
			{
				while(!(UCA0IFG & UCRXIFG));
				temp = UCA0RXBUF;
				i++;
				switch (i)
				{
					case 9:
						SET_ERROR(status, temp);// error return
						break;
					case 10:
						SET_1(status, temp);	// first parameter return
						break;
					case 11:
						SET_2(status, temp);	// second parameter return
						break;
					default:
						break;					// discard
				}
			}
		}
		else
			trap_error(0x40); 					// this should never happen.
		if (GET_ERROR(status))					// oops...
			trap_error(GET_ERROR(status));
		while(UCA0STATW & UCBUSY);				// ensure everything was received
		return (uint16_t)(status & 0xFFFF);
	}
}

/* initialization APIs */
void set_id(uint8_t old_id, uint8_t new_id)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	SET_ID(packet, old_id);
	SET_REG(packet, ID);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, new_id);
	checksum = checksum_gen(packet);
	crc_l = XL_GET_1(checksum);
	crc_h = XL_GET_2(checksum);

	motor_write(packet, crc_l, crc_h);
	if (old_id == 0xFE)
	{
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
}

void set_baud(uint8_t id, uint8_t rate)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	SET_ID(packet, id);
	SET_REG(packet, BAUD);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, rate);
	checksum = checksum_gen(packet);
	crc_l = XL_GET_1(checksum);
	crc_h = XL_GET_2(checksum);

	motor_write(packet, crc_l, crc_h);
	if (id == 0xFE)
	{
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		switch(rate)
		{
			case 1:
				packet &= ~(0xFF);
				SET_1(packet, 0x03);
				break;
			case 16:
				packet &= ~(0xFF);
				SET_1(packet, 0x02);
				break;
			case 34:
				packet &= ~(0xFF);
				SET_1(packet, 0x01);
				break;
			case 207:
				packet &= ~(0xFF);
				break;
		}
		motor_write(packet, crc_l, crc_h);
	}
}


void set_return(uint8_t id, uint8_t level)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	if (id == 0xFE)
	{
		SET_ID(packet, id);
		SET_REG(packet, RETURN);
		SET_PARAM(packet, 2);
		SET_INST(packet, WRITE);
		SET_1(packet, level);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
		packet &= 0xFF00FFFFFFFFFFFF;
		SET_REG(packet, XL_RETURN);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
	else if (id < 0x07)
	{
		SET_ID(packet, id);
		SET_REG(packet, RETURN);
		SET_PARAM(packet, 2);
		SET_INST(packet, WRITE);
		SET_1(packet, level);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
	else
	{
		SET_ID(packet, id);
		SET_REG(packet, XL_RETURN);
		SET_PARAM(packet, 2);
		SET_INST(packet, WRITE);
		SET_1(packet, level);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
}

void set_delay(uint8_t id, uint8_t delay)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	SET_ID(packet, id);
	SET_REG(packet, DELAY);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, delay);
	checksum = checksum_gen(packet);
	crc_l = XL_GET_1(checksum);
	crc_h = XL_GET_2(checksum);
	motor_write(packet, crc_l, crc_h);
	if (id == 0xFE)
	{
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
}

void normalize(uint8_t id)
{
	goal_position(id, 0x200, 0x050);
}

void torque_enable(uint8_t id)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	SET_ID(packet, id);
	SET_REG(packet, ENTORQUE);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, 0x01);
	checksum = checksum_gen(packet);
	crc_l = XL_GET_1(checksum);
	crc_h = XL_GET_2(checksum);

	motor_write(packet, crc_l, crc_h);
	if (id == 0xFE)
	{
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
}

void torque_disable(uint8_t id)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	SET_ID(packet, id);
	SET_REG(packet, ENTORQUE);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, 0x00);
	checksum = checksum_gen(packet);
	crc_l = XL_GET_1(checksum);
	crc_h = XL_GET_2(checksum);

	motor_write(packet, crc_l, crc_h);
	if (id == 0xFE)
	{
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
}

void set_torque(uint8_t id, uint16_t torque)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	if (id == 0xFE)
	{
		SET_ID(packet, id);
		SET_REG(packet, TORQUE);
		SET_PARAM(packet, 3);
		SET_INST(packet, WRITE);
		SET_1(packet, GET_1(torque));
		SET_2(packet, GET_2(torque));
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
		packet &= ~(0x00FF000000000000);
		SET_REG(packet, XL_TORQUE);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
	else if (id < 0x07)
	{
		SET_ID(packet, id);
		SET_REG(packet, TORQUE);
		SET_PARAM(packet, 3);
		SET_INST(packet, WRITE);
		SET_1(packet, GET_1(torque));
		SET_2(packet, GET_2(torque));
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
	else
	{
		SET_ID(packet, id);
		SET_REG(packet, XL_TORQUE);
		SET_PARAM(packet, 3);
		SET_INST(packet, WRITE);
		SET_1(packet, GET_1(torque));
		SET_2(packet, GET_2(torque));
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
}

void joint_mode(uint8_t id)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	if (id == 0xFE)
	{
		SET_ID(packet, id);
		SET_REG(packet, CW);
		SET_PARAM(packet, 5);
		SET_INST(packet, WRITE);
		SET_1(packet, 0x01);
		SET_3(packet, 0xFF);
		SET_4(packet, 0x03);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
		packet &= ~(0x00FFFF00FFFFFFFF);
		SET_REG(packet, CONTROL);
		SET_PARAM(packet, 2);
		SET_1(packet, 2);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
	else if (id < 0x07)
	{
		SET_ID(packet, id);
		SET_REG(packet, CW);
		SET_PARAM(packet, 5);
		SET_INST(packet, WRITE);
		SET_1(packet, 0x01);
		SET_3(packet, 0xFF);
		SET_4(packet, 0x03);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
	else
	{
		SET_ID(packet, id);
		SET_REG(packet, CONTROL);
		SET_PARAM(packet, 2);
		SET_INST(packet, WRITE);
		SET_1(packet, 2);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
}

uint8_t ping(uint8_t id)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	if (id == 0xFE)
		trap_error(0x40);
	else if (id < 0x07)
	{
		SET_ID(packet, id);
		SET_INST(packet, PING);
	}
	else
	{
		SET_ID(packet, id);
		SET_INST(packet, PING);
		SET_PARAM(packet, 0xFF);
	}
	checksum = checksum_gen(packet);
	crc_l = XL_GET_1(checksum);
	crc_h = XL_GET_2(checksum);

	if (GET_ID(motor_read(packet, crc_l, crc_h)) == id)
		return 1;
	return 0;
}

void led_on(uint8_t id)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	SET_ID(packet, id);
	SET_REG(packet, LED);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, 0x01);
	checksum = checksum_gen(packet);
	crc_l = XL_GET_1(checksum);
	crc_h = XL_GET_2(checksum);

	motor_write(packet, crc_l, crc_h);
	if (id == 0xFE)
	{
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
}

void led_off(uint8_t id)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	SET_ID(packet, id);
	SET_REG(packet, LED);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, 0x00);
	checksum = checksum_gen(packet);
	crc_l = XL_GET_1(checksum);
	crc_h = XL_GET_2(checksum);

	motor_write(packet, crc_l, crc_h);
	if (id == 0xFE)
	{
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
}

/* performance APIs */
void goal_position(uint8_t id, uint16_t position, uint16_t speed)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	SET_ID(packet, id);
	SET_REG(packet, GOAL_POS);
	SET_PARAM(packet, 5);
	SET_INST(packet, WRITE);
	SET_1(packet, GET_1(position));
	SET_2(packet, GET_2(position));
	SET_3(packet, GET_1(speed));
	SET_4(packet, GET_2(speed));
	checksum = checksum_gen(packet);
	crc_l = XL_GET_1(checksum);
	crc_h = XL_GET_2(checksum);

	motor_write(packet, crc_l, crc_h);
	if (id == 0xFE)
	{
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
}

uint16_t curr_position(uint8_t id)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	if (id == 0xFE)
		trap_error(0x40); 			// NOT SUPPOSED TO HAPPEN!
	else if (id < 0x07)
	{
		SET_ID(packet, id);
		SET_REG(packet, CURR_POS);
		SET_PARAM(packet, 2);
		SET_INST(packet, READ);
		SET_1(packet, 0x02);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);

		return motor_read(packet, crc_l, crc_h);
	}
	else
	{
		SET_ID(packet, id);
		SET_REG(packet, XL_CURR_POS);
		SET_PARAM(packet, 3);
		SET_INST(packet, READ);
		SET_1(packet, 0x02);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);

		return motor_read(packet, crc_l, crc_h);
	}
	return 0;
}

void register_goal_position(uint8_t id, uint16_t position, uint16_t speed)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	SET_ID(packet, id);
	SET_REG(packet, GOAL_POS);
	SET_PARAM(packet, 5);
	SET_INST(packet, REG_WRITE);
	SET_1(packet, GET_1(position));
	SET_2(packet, GET_2(position));
	SET_3(packet, GET_1(speed));
	SET_4(packet, GET_2(speed));
	checksum = checksum_gen(packet);
	crc_l = XL_GET_1(checksum);
	crc_h = XL_GET_2(checksum);

	motor_write(packet, crc_l, crc_h);
	if (id == 0xFE)
	{
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
}

void action(uint8_t id)
{
	uint64_t packet = 0;
	uint16_t checksum;
	uint8_t crc_l, crc_h;
	if (id == 0xFE)
	{
		SET_ID(packet, id);
		SET_INST(packet, ACTION);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);

		motor_write(packet, crc_l, crc_h);
		SET_PARAM(packet, 0xFF);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
	else if (id < 0x07)
	{
		SET_ID(packet, id);
		SET_INST(packet, ACTION);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);

	}
	else
	{
		SET_ID(packet, id);
		SET_INST(packet, PING);
		SET_PARAM(packet, 0xFF);
		checksum = checksum_gen(packet);
		crc_l = XL_GET_1(checksum);
		crc_h = XL_GET_2(checksum);
		motor_write(packet, crc_l, crc_h);
	}
}

/* error handling */
void trap_error(uint8_t error)
{
	/* AHHH WHY DID WE EVEN GET HERE?!
	 *
	 * brian, at this point you should observe the error code
	 * and find out why you got here.
	 * ideally, you'd want to send the error code back to the SBC.
	 */
	__bis_SR_register(LPM4_bits);			// no tears now, only dreams...
}
