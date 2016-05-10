#include <msp.h>
#include <stdint.h>
#include "init.h"
#include "dynamixel.h"
#include "calculations.h"
#include "gestures.h"

/* data structure for information from raspberry pi */
typedef union
{
	uint8_t raw[13];
	struct __attribute__((__packed__))
	{
		uint8_t jid;
		union
		{
			struct __attribute__((__packed__))
			{
				float goal_pos;
				uint32_t start_time;
				uint32_t end_time;
			};
			struct __attribute__((__packed__))
			{
				uint8_t gesture;
				uint32_t gstart_time;
				//uint8_t padding0[7];
			};
			struct __attribute__((__packed__))
			{
				uint8_t checkpoint;
				//uint8_t padding1[11];
			};
		};
	};
} raw_data;

typedef struct __attribute__((__packed__))
{
	uint8_t jid;
	union
	{
		struct __attribute__((__packed__))
		{
			uint16_t position;
			uint16_t speed;
			uint32_t start_time;
		};
		struct __attribute__((__packed__))
		{
			uint8_t gesture;
			uint32_t gstart_time;
			//uint8_t padding0[4];
		};
		struct __attribute__((__packed__))
		{
			uint8_t checkpoint;
			//uint8_t padding1[7];
		};
	};
} processed;

/* arrays */
raw_data queue[200];
processed buffer[200];

/* communication flags and variables*/
uint8_t waiting = 1;
uint8_t len = 0;

/* interrupt vector holders */
uint8_t s, p;

void main(void)
{
	uint8_t i;
	msp_init();
	//dynamixel_init();
    while(1)
    {
    	UCA3IE |= UCRXIE;
    	while(waiting);
    	for (i = 0; i < len; i++)
    	{
    		if (queue[i].jid == 0xFE)		// gesture
    		{
    			buffer[i].jid = queue[i].jid;
    			buffer[i].gesture =  queue[i].gesture;
    			buffer[i]. gstart_time = queue[i].gstart_time;
    		}
    		else if (queue[i].jid == 0xFD)	// checkpoint
    		{
    			buffer[i].jid = queue[i].jid;
    			buffer[i].checkpoint = queue[i].checkpoint;
    		}
    		else							// calculation for non-xl motor must be done.
    		{
    			float rad_delta;			// this is the distance we are supposed to travel
    			float time_delta = queue[i].end_time - queue[i].start_time;
    			float rate;
    			uint16_t ticks;
				float temp = (queue[i].goal_pos + (0.833*PI));
				float temp1 = T2R(goal_positions[queue[i].jid]);
				float temp2 = (queue[i].goal_pos + PI);
				float temp3 = MT2R(goal_positions[queue[i].jid]);
    			if ((!queue[i].jid) || (queue[i].jid == 1))
    			{
					if (goal_positions[queue[i].jid] >= R2T(queue[i].goal_pos))
						rad_delta = temp1 - temp;
					else
						rad_delta = temp - temp1;
					rate = (270000000/PI)*(rad_delta/time_delta);		// now in ticks.
					if (rate > 0x3FF)
						rate = 0x3FF;
					ticks = R2T(queue[i].goal_pos);
    			}
    			else
    			{
    				if (goal_positions[queue[i].jid] >= MR2T(queue[i].goal_pos))
    					rad_delta = temp3 - temp2;
    				else
    					rad_delta = temp2 - temp3;
    				rate = (270000000/PI)*(rad_delta/time_delta);		// now in ticks.
    				if (rate > 0x3FF)
    					rate = 0x3FF;
    				ticks = MR2T(queue[i].goal_pos);
    			}

    			/* now store in data queue */
    			buffer[i].jid = queue[i].jid;
    			buffer[i].position = goal_positions[buffer[i].jid] = ticks;
    			buffer[i].speed = rate;
    			buffer[i].start_time = queue[i].start_time;
    		}
    	}
    	/* since we changed some values in the goal_positions[] array,
    	 * we now have to restore them all to 512, so when we load data into
    	 * shared memory, we have a reference as to where the motors "should" be
    	 * (goal_positions[]) and where the motors "actually" are. (readings[]).
    	 */
    	for (i = 0; i < 8; i++)
    	{
    		if ((!i) || (i == 1))
    			goal_positions[i] = 0x200;
    		else
    			goal_positions[i] = 0x800;
    	}
    	buffer[len].jid = 0xFF; 			// stopping call for scheduler
    	/* DELETE THE LINE BELOW THIS FOR THE FINAL AND UNCOMMENT P7OUT |= BIT3 */
    	P1OUT |= BIT5;
    	//P7OUT |= BIT3; 					// we're ready!
    	while(event_reg != BEGIN);
    	event_reg = UART_READY;
		while(event_reg != DONE);
		event_reg = UART_READY;
		waiting = 1;
		i = 0;
		P6IE |= BIT6;
    }
}

void spi()
{
	/* receiving variables */
	static uint16_t r_i = 0, num_moves = 0;
	static uint8_t packet_type = 0;

	/* transmitting variables */
	static uint8_t jid = 0;

	s = UCA3IV;
	switch(s)
	{
		case 0x02:
			switch(packet_type)
			{
				case 0: packet_type = UCA3RXBUF; r_i = 0; break;
				case 1:
					if (r_i == 0)
					{
						num_moves = UCA3RXBUF;
						r_i++;
					}
					else if (r_i == 1)
					{
						num_moves = (UINT16_C(UCA3RXBUF) << 8) | num_moves;
						r_i++;
					}
					else if ((r_i-2) < 13*num_moves)
					{
						queue[(r_i-2)/13].raw[(r_i-2)%13] = UCA3RXBUF;
						if ((r_i-2) == (13*num_moves)-1)
						{
							packet_type = waiting = 0;
							len = (r_i-1)/(13);
						}
						else
							r_i++;
					}
					break;
				case 2:
				case 3:
					UCA3IE |= UCTXIE;
					break;
			}
			break;
		case 0x04:
			switch(packet_type)
			{
				case 2:
					if (r_i < 40)
					{
						if ((r_i % 5) == 0)
							UCA3TXBUF = jid;
						else if ((r_i % 5) == 1)
							UCA3TXBUF = GET_1(rad_readings[jid]);
						else if ((r_i % 5) == 2)
							UCA3TXBUF = GET_2(rad_readings[jid]);
						else if ((r_i % 5) == 3)
							UCA3TXBUF = GET_3(rad_readings[jid]);
						else
						{
							UCA3TXBUF = GET_4(rad_readings[jid]);
							jid++;
						}
					}
					else
					{
						UCA3TXBUF = checkpoint;
						packet_type = jid = 0;
						UCA3IE &= ~UCTXIE;
					}
					break;
				case 3:
					UCA3TXBUF = error;
					packet_type = 0;
					UCA3IE &= ~UCTXIE;
					break;
			}
	}
}

void sbc_comm_port()
{
	p = P6IV;
	switch(p)
	{
		case 0x0E:	// go!
			event_reg = BEGIN;
			SYSTICK_STCSR |= SysTick_CTRL_TICKINT_Msk;		// turn on the scheduler.
			P6IE &= ~BIT6;
			//P7OUT &= ~BIT3;
			P1OUT &= ~BIT5;
			P6IFG &= ~BIT6;
			__delay_cycles(10);
			break;
		case 0x10: 	// emergency!
			event_reg = EMERGENCY;
			SYSTICK_STCSR &= ~SysTick_CTRL_TICKINT_Msk;		// turn off the scheduler (for now).
			UCA1IE |= UCTXIE;								// hold all motors still.
			P6IE &= ~BIT7;
			break;
		default: break;
	}
}

void scheduler()
{
	/* private variables for scheduler use */
	static uint16_t time_out = 0; 				// number of times the scheduler was called
	static uint16_t wait_time = 0; 				// the amount of time the scheduler has to wait before loading shared memory
	static uint32_t curr_time = 0; 				// current time
	static uint16_t i = 0; 						// iterator through processed[] array
	uint8_t j; 									// general purpose iterator

	/* checkpoint variables */
	static uint16_t last_readings[8] = { 0 }; 	// holds the readings from the last scheduler call
	static uint8_t  strike_out[8] = { 0 }; 		// holds the number of times the motor hasn't made significant progress
	/* if any index in strike_out[] reaches four, then we know something is wrong. */

	if (!wait_time)
	{
		switch(event_reg)
		{
			case UART_READY:
				/* the last item in the buffer! */
				if (buffer[i].jid == 0xFF)
				{
					if (time_out == 333) 		// time out of approximately two seconds
					{
						for (j = 0; j < 8; j++)
						{
							sync_ids[j] = j;
							sync_speeds[j] = 0x050;
							if ((!j) || (j == 1))
								sync_positions[j] = goal_positions[j] = 0x200;
							else
								sync_positions[j] = goal_positions[j] = 0x800;
						}
						sync_len = 8;
						g_id = open_id;
						xl_len = open[0] + 7;
						UCA1IE |= UCTXIE;		// turn on tx interrupts
						time_out = 0;			// make time_out zero
					}
					else
						time_out++;
				}

				/* a hand gesture */
				else if (buffer[i].jid == 0xFE)
				{
					if (buffer[i].gstart_time == curr_time) // make sure that this is meant to send out now
					{
						g_id = buffer[i].gesture;	   	 // load gesture into shared memory

						/* we can't do w or m. :( */
						switch(g_id)
						{
							case 2: xl_len = open[0] + 7; break;
							case 6: xl_len = letter_a[0] + 7; break;
							case 7: xl_len = letter_b[0] + 7; break;
							case 8: xl_len = letter_c[0] + 7; break;
							case 28: 			 	 	 	 	 	    /* z is just d with wrist motion */
							case 9: xl_len = letter_d[0] + 7; break;
							case 10: xl_len = letter_e[0] + 7; break;
							case 11: xl_len = letter_f[0] + 7; break;
							case 29: 			 	 	 	 	 	 	/* q is just g with wrist motion */
							case 12: xl_len = letter_g[0] + 7; break;
							case 13: xl_len = letter_h[0] + 7; break;
							case 26: 									/* j is just i with wrist motion */
							case 14: xl_len = letter_i[0] + 7; break;
							case 27: 									/* p is just k with wrist motion */
							case 15: xl_len = letter_k[0] + 7; break;
							case 16: xl_len = letter_l[0] + 7; break;
							case 17: xl_len = letter_n[0] + 7; break;
							case 18: xl_len = letter_o[0] + 7; break;
							case 19: xl_len = letter_r[0] + 7; break;
							case 20: xl_len = letter_s[0] + 7; break;
							case 21: xl_len = letter_t[0] + 7; break;
							case 22: xl_len = letter_u[0] + 7; break;
							case 23: xl_len = letter_v[0] + 7; break;
							case 24: xl_len = letter_x[0] + 7; break;
							case 25: xl_len = letter_y[0] + 7; break;
							default:
								SYSTICK_STCSR &= ~SysTick_CTRL_TICKINT_Msk;		// turn off the scheduler (for now).
								event_reg = ERROR;
								error |= BIT7;
								P10OUT |= BIT0;
								break;
						}
						if ((buffer[i+1].jid == 0xFF) || (buffer[i+1].jid == 0xFD) ||
						   ((buffer[i+1].jid == 0xFE) && (buffer[i+1].gstart_time != curr_time)) ||
						   ((buffer[i+1].jid < 0x08) && (buffer[i+1].start_time != curr_time)))		// if next ones aren't motor moves...
							UCA1IE |= UCTXIE;
						else
							i++;
					}
					else										// if not, just send out what we have now
						UCA1IE |= UCTXIE;
				}

				/* a checkpoint */
				else if (buffer[i].jid == 0xFD)
				{
					/*
					 * we have to check for two things when it comes to checkpoints:
					 * 1) has a "lagging" motor moved within three scheduler calls?
					 * 	  (cause apparently, the slowest motor moves at 0.4 ticks in a scheduler call)
					 * 		if it has, then move on to number two.
					 * 		if it hasn't, then we know the motor is jammed/malfunctioning.
					 * 2) if it's moving, has it reached its final position within 135 scheulder calls,
					 *    or 810 ms?
					 *    	if it has, then the motor is totally fine.
					 *    	if it hasn't, then something's going on...it's moving, just not fast enough.
					 */

					uint8_t flag = 1;
					for (j = 0; j < 8; j++)
					{
						/* check to see if the motor has reached its final position w/in a certain tolerance */
						if (!((readings[j] <= (goal_positions[j] + GUNSTON)) &&
							(readings[j] >= (goal_positions[j] - GUNSTON))))
						{
							/* if it's the first time around, then just load it's current position into last_readings[] */
							if (!strike_out[j])
							{
								last_readings[j] = readings[j]; 	// save the current reading; we will use this as a reference next time around.
								strike_out[j]++; 					// increment strike_out.
							}
							else
							{
								/* we struck out! :( */
								if (strike_out[j] == 4)
								{
									SYSTICK_STCSR &= ~SysTick_CTRL_TICKINT_Msk;		// turn off the scheduler (for now).
									UCA1IE &= ~(UCTXIE | UCRXIE);	// turn off all uart interrupts
									error |= BIT7;					// set the error bit
									P10OUT |= BIT0;					// set error port high
									return; 						// skip everything else
								}
								else
								{
									/* hey, we moved a little! that's progress! */
									if ((readings[j] < last_readings[j]) ||
										(readings[j] > last_readings[j]))
										strike_out[j] = 0;
									/* still haven't moved... */
									else
									{
										last_readings[j] = readings[j]; 	// update
										strike_out[j]++;
									}
								}
							}
							flag = 0;
						}
						else
						{
							/* the motor reached its final position, we're good. */
							strike_out[j] = 0;
						}
					}
					if (flag)	// all the motors have made it; we're good.
					{
						checkpoint++;
						time_out = 0;
						if (buffer[i+1].jid == 0xFE)
						{
							curr_time = buffer[i+1].gstart_time;
							i++;
						}
						else if (buffer[i+1].jid == 0xFF)
							i++;
						else
						{
							curr_time = buffer[i+1].start_time;
							i++;
						}
					}
					else	// at least one of the motors is lagging a little.
					{
						if (time_out < 175) // if it's been less than one second, let it slide.
						{
							time_out++;
							event_reg = READ;
							UCA1IE |= UCTXIE;
							read_id = 0;
						}
						else 	// it's been more than one second and a motor still hasn't gotten to its final position; there's a problem.
						{
							SYSTICK_STCSR &= ~SysTick_CTRL_TICKINT_Msk;		// turn off the scheduler (for now).
							UCA1IE &= ~(UCTXIE | UCRXIE);
							error |= BIT7;
							P10OUT |= BIT0;
							time_out = 0;
						}
						break;
					}
				}

				/* an arm motor */
				else
				{
					if (buffer[i].start_time == curr_time) // make sure that this is meant to send out now
					{
						sync_ids[sync_len] = buffer[i].jid;
						sync_positions[sync_len] = goal_positions[buffer[i].jid] = buffer[i].position;
						sync_speeds[sync_len] = buffer[i].speed;
						sync_len++;
						if ((buffer[i+1].jid == 0xFF) || (buffer[i+1].jid == 0xFD) ||
						   ((buffer[i+1].jid == 0xFE) && (buffer[i+1].gstart_time != curr_time)) ||
						   ((buffer[i+1].jid < 0x08) && (buffer[i+1].start_time != curr_time)))		// if next ones aren't motor moves...
							UCA1IE |= UCTXIE;
						else
							i++;
					}
					else										// if not, just send out what we have now
					{
						UCA1IE |= UCTXIE;
						break;
					}
				}
				break;
			case UART_SEND_DONE:
				event_reg = UART_READ;		// tell the uart to read
				UCA1IE |= UCTXIE;			// make the uart send reading instructions
				read_id = 0;				// start from id 0
				break;
			case UART_READ_DONE:
				for (j = 0; j < 8; j++)
				{
					if ((!j) || (j == 1))
						rad_readings[j] = T2R(readings[i]); 				// convert all readings to radians
					else
						rad_readings[j] = MT2R(readings[i]);
					sync_ids[j] = sync_positions[j] = sync_speeds[j] = 0; 	// clear out shared memory
				}
				g_id = 0;
				if (buffer[i].jid == 0xFF)	// we're done with everything!
				{
					event_reg = DONE;
					SYSTICK_STCSR &= ~SysTick_CTRL_TICKINT_Msk;		// turn off the scheduler.
					waiting = 1;
					curr_time = time_out = i = 0;
				}
				else						// there's still more to go through.
				{
					event_reg = UART_READY;
					if (buffer[i+1].jid == 0xFE)
					{
						if (((buffer[i+1].gstart_time - curr_time)/1000) > 6)
							wait_time = ((buffer[i+1].gstart_time - curr_time)/6000)-1;
						else
							wait_time = 0;
						curr_time = buffer[i+1].gstart_time;
					}
					else if (buffer[i+1].jid < 0x08)
					{
						if (((buffer[i+1].start_time - curr_time)/1000) > 6)
							wait_time = ((buffer[i+1].start_time - curr_time)/6000)-1;
						else
							wait_time = 0;
						curr_time = buffer[i+1].start_time;
					}
					i++;
				}
				break;
			case UART_SENDING:
			case UART_READING:
			case UART_READ:
				/* if the event_reg states that we are still sending/receiving and the scheduler has been
				 * triggered, then we took too long.
				 */
				SYSTICK_STCSR &= ~SysTick_CTRL_TICKINT_Msk;		// turn off the scheduler (for now).
				UCA1IE &= ~(UCTXIE | UCRXIE);
				error |= BIT7;
				P10OUT |= BIT0;
				break;
			case EMERGENCY:
			case EMERGENCY_SENDING:
				break;
			case EMERGENCY_DONE:
				break;
			case DONE: 	/* congratulations, we actually might graduate... */
				SYSTICK_STCSR &= ~SysTick_CTRL_TICKINT_Msk;		// turn off the scheduler.
				waiting = 1;
				curr_time = time_out = i = 0;
				break;
		}
	}
	else
		wait_time--;
}
