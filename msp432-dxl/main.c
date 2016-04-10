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
raw_data queue[40];
processed buffer[40];

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
    	while(waiting)
    		__wfi();
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
    			if (readings[queue[i].jid] >= (queue[i].goal_pos))
    				rad_delta = T2R(readings[queue[i].jid]) - queue[i].goal_pos;
    			else
    				rad_delta = queue[i].goal_pos - T2R(readings[queue[i].jid]);
    			rate = (9549296/0.111)*(rad_delta/time_delta);		// now in ticks.
    			if (rate > 1023)
    				rate = 1023;
    			if (R2T(queue[i].goal_pos) > 1023)
    				ticks = 1023;
    			else
    				ticks = R2T(rad_delta);

    			/* now store in data queue */
    			buffer[i].jid = queue[i].jid;
    			buffer[i].position = ticks;
    			buffer[i].speed = rate;
    		}
    	}
    	buffer[len].jid = 0xFF; 			// stopping call for scheduler
    	P7OUT |= BIT3; 						// we're ready!
    	__sleep();
		while(event_reg != DONE)
			__sleep();
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
		case UCRXIFG:
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
							len = (r_i-2)/13;
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
		case UCTXIFG:
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
			SYSTICK_STCSR |= SysTick_CTRL_TICKINT_Msk;		// turn on the scheduler.
			break;
		case 0x10: 	// emergency!
			event_reg = EMERGENCY;
			UCA1IE |= UCTXIE;
			break;
		default: break;
	}
}

void scheduler()
{
	/* private variables for scheduler use */
	static uint16_t time_out = 0; 			// number of times the scheduler was called
	static uint32_t curr_time = 0; 			// current time
	static uint8_t i = 0; 					// iterator through processed[] array
	uint8_t j; 								// general purpose iterator

	switch(event_reg)
	{
		case UART_READY:
			if (buffer[i].jid == 0xFF)		// last item in buffer
			{
				if (time_out == 750) 		// time out of approximately 4.5 seconds
				{
					for (j = 0; j < 8; j++)	// send all motors back to home position after three seconds of hold
					{
						sync_ids[j] = j;
						sync_positions[j] = goal_positions[j] = 512;
						sync_speeds[j] = 0x100;
					}
					g_id = open_id;
					UCA1IE |= UCTXIE;		// turn on tx interrupts
					time_out = 0;			// make time_out zero
				}
			}
			else if (buffer[i].jid == 0xFE)
			{
				if (buffer[i].gstart_time == curr_time)		// make sure that this is meant to send out now
				{
					g_id = buffer[i].gesture;
					i++;
				}
				else										// if not, just send out what we have now
				{
					UCA1IE |= UCTXIE;
					break;
				}
			}
			else if (buffer[i].jid == 0xFD)
			{
				uint8_t flag = 1;
				for (j = 0; j < 8; j++)
				{
					if (!((readings[j] <= (goal_positions[j] + GUNSTON)) &&
						(readings[j] >= (goal_positions[j] - GUNSTON))))
					{
						flag = 0;
						break;
					}
				}
				if (flag)				// every motor was within +/-GUNSTON ticks
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
				else					// if not, read every motor again and update readings[]
				{
					if (!time_out) 		// if this has not been called previously, let this one slide.
					{
						time_out++;
						event_reg = READ;
						UCA1IE |= UCTXIE;
						read_id = 0;
					}
					else 				// if this has been called previously, we have an error with one of the motors.
					{
						UCA1IE &= ~(UCTXIE | UCRXIE);
						error |= BIT7;
						P10OUT |= BIT0;
					}
					break;
				}
			}
			else						// non-xl motor
			{
				if (buffer[i].start_time == curr_time)		// make sure that this is meant to send out now
				{
					sync_ids[i] = buffer[i].jid;
					sync_positions[i] = goal_positions[buffer[i].jid] = buffer[i].position;
					sync_speeds[i] = buffer[i].speed;
					i++;
					sync_len++;
				}
				else										// if not, just send out what we have now
				{
					UCA1IE |= UCTXIE;
					break;
				}
			}
		case UART_SEND_DONE:
			event_reg = UART_READ;		// tell the uart to read
			UCA1IE |= UCTXIE;			// make the uart send reading instructions
			read_id = 0;				// start from id 0
			break;
		case UART_READ_DONE:
			for (j = 0; j < 8; j++)
			{
				rad_readings[j] = T2R(readings[i]); 					// convert all readings to radians
				sync_ids[j] = sync_positions[j] = sync_speeds[j] = 0; 	// clear out shared memory
			}
			g_id = 0;
			if (buffer[i].jid == 0xFF)	// we're done with everything!
				event_reg = DONE;
			else						// there's still more to go through.
			{
				event_reg = UART_READY;
				if (buffer[i].jid == 0xFE)
					curr_time = buffer[i].gstart_time;
				else if (buffer[i].jid != 0xFD)
					curr_time = buffer[i].start_time;
			}
			break;
		case UART_SENDING:
		case UART_READING:
			/* if the event_reg states that we are still sending/receiving and the scheduler has been
			 * triggered, then we took too long.
			 */
			UCA1IE &= ~(UCTXIE | UCRXIE);
			error |= BIT7;
			P10OUT |= BIT0;
			break;
	}
}
