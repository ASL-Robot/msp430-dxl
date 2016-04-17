/*
 * author: Brian Ruiz
 * date:   February 21, 2015
 * school: George Mason University
 */

#ifndef MSP430_DXL_DYNAMIXEL_H_
#define MSP430_DXL_DYNAMIXEL_H_

#include <stdint.h>
#include <msp.h>

/* for writes */
extern uint8_t sync_ids[8];				// holds the ids that need to be written to/read from
extern uint16_t sync_positions[8];		// holds positions to move to
extern uint16_t sync_speeds[8];			// holds speeds to move to above positions to
extern uint16_t goal_positions[8]; 		// holds theoretical value of goal position for each motor
extern uint8_t sync_len; 				// holds how long the non-xl sync write will be

extern uint8_t g_id; 					// holds the gesture the motors must perform
extern uint8_t xl_len; 					// holds the length of the sync_write packet for a gesture
extern uint8_t read_id; 				// holds the id that needs to be read from
extern uint8_t event_reg; 				// the MOST important "data structure". commands tasks to run

/* for readings */
extern uint16_t readings[8];			// holds current positions of motors from sync_read()
extern float	rad_readings[8];		// holds current positions in radians
extern uint8_t  checkpoint; 			// holds current checkpoint
extern uint16_t  error; 				// holds error

/* should not be accessed w/in any other file */
extern uint16_t checksum; 				// global checksum for communication protocol two


/* register addresses (that i think are relevant) */
#define ID				3
#define BAUD			4
#define DELAY			5
#define CW				6	// use a four parameter write for joint mode
#define TORQUE			14	// use a two parameter write
#define RETURN			16
#define ENTORQUE 		24
#define LED				25
#define GOAL_POS		30	// use a four parameter write
#define CURR_POS		36	// use a two parameter read

/* register addresses for xl-320 (that i think are relevant) */
#define	CONTROL			11
#define XL_TORQUE		15	// two parameter write
#define XL_RETURN		17
#define XL_CURR_POS		37	// two paramter read

/* instruction types */
#define PING			0x01
#define READ			0x02
#define WRITE			0x03
#define REG_WRITE		0x04
#define ACTION			0x05
#define RESET			0x06
#define INST			0x55
#define SYNC_READ		0x82
#define SYNC_WRITE 		0x83

/* instruction types for event register */
#define UART_READY			'G'
#define UART_SENDING		'E'
#define UART_READING		'O'
#define UART_SEND_DONE		'r'
#define UART_READ			'g'
#define DONE				'e'

#define UART_READ_DONE		'M'
#define ERROR				'a'
#define EMERGENCY 			's'
#define EMERGENCY_SENDING	'o'
#define EMERGENCY_DONE		'n'

#define BEGIN				'U'

/* motor ids */
#define WRIST			0x01
#define THUMB_BASE 		0x10
#define THUMB_KNUCKLE 	0x11
#define THUMB_POINT		0x12
#define PINKY_BASE		0x13
#define PINKY_KNUCKLE	0x14
#define PINKY_POINT		0x15
#define INDEX_BASE		0x16
#define INDEX_KNUCKLE	0x17
#define INDEX_POINT		0x18
#define PINKY_SPADE		0x19
#define INDEX_SPADE		0x1A

/* gesture ids */
#define curl_id			1
#define open_id			2
#define thumbs_up_id	3
#define point_id		4
#define okay_id			5

/* other important constants */
#define GUNSTON 		15 		// fudge factor; turns out that readings can be off by approximately three degrees...

/* macro functions/variables that may be helpful */
/* the send packet layout for the write primitive is as follows:
 *		[63:56] - id of motor to read/write to
 *		[55]	- communication protocol
 *		[54:48] - register to read/write to
 *		[47:40] - number of parameters
 *		[39:32] - instruction type
 *		[31:24] - fourth parameter
 *		[23:16] - third parameter
 *		[15:8]  - second parameter
 *		[7:0]   - first parameter
 *
 *	the return packet layout for the read primitive is as follows:
 *		[63:56] - id of motor to read/write to
 *		[55:48] - error codes
 *		[47:40] - reserved
 *		[39:32] - reserved
 *		[31:24] - reserved
 *		[23:16] - reserved
 *		[15:8]  - second return
 *		[7:0]   - first return
 */

/* getters */
#define GET_ID(x)		 ((UINT64_C(x) & 0xFF00000000000000) >> 56)
#define GET_COMM(x) 	 ((UINT64_C(x) & 0x0080000000000000) >> 55)
#define GET_REG(x)		 ((UINT64_C(x) & 0x007F000000000000) >> 48)
#define GET_ERROR(x) 	 ((UINT64_C(x) & 0x00FF000000000000) >> 48)
#define GET_PARAM(x)	 ((UINT64_C(x) & 0x0000FF0000000000) >> 40)
#define GET_INST(x) 	 ((UINT64_C(x) & 0x000000FF00000000) >> 32)
#define GET_4(x)		 ((UINT64_C(x) & 0x00000000FF000000) >> 24)   // get fourth parameter
#define GET_3(x) 		 ((UINT64_C(x) & 0x0000000000FF0000) >> 16)   // get third parameter
#define GET_2(x) 		 ((UINT64_C(x) & 0x000000000000FF00) >> 8)    // get second parameter
#define GET_1(x)          (UINT64_C(x) & 0x00000000000000FF)          // get first parameter

/* setters */
#define SET_ID(x,y)		 (x |= (UINT64_C(y) << 56))
#define SET_COMM(x,y)    (x |= (UINT64_C(y) << 55))
#define SET_REG(x,y)	 (x |= (UINT64_C(y) << 48))
#define SET_ERROR(x,y)   (x |= (UINT64_C(y) << 48))
#define SET_PARAM(x,y)   (x |= (UINT64_C(y) << 40))
#define SET_INST(x,y)    (x |= (UINT64_C(y) << 32))
#define SET_4(x,y) 		 (x |= (UINT64_C(y) << 24))
#define SET_3(x,y) 		 (x |= (UINT64_C(y) << 16))
#define SET_2(x,y) 		 (x |= (UINT64_C(y) << 8))
#define SET_1(x,y)       (x |= UINT64_C(y))

/* clearers */
#define CLEAR_ID(x)		(x &= ~0xFF00000000000000)
#define CLEAR_COMM(x)   (x &= ~0x0080000000000000)
#define CLEAR_REG(x)	(x &= ~0x007F000000000000)
#define CLEAR_PARAM(x)	(x &= ~0x0000FF0000000000)
#define CLEAR_INST(x)	(x &= ~0x000000FF00000000)
#define CLEAR_4(x)		(x &= ~0x00000000FF000000)
#define CLEAR_3(x)		(x &= ~0x0000000000FF0000)
#define CLEAR_2(x)		(x &= ~0x000000000000FF00)
#define CLEAR_1(x)		(x &= ~0x00000000000000FF)

/* checksum generator */
void checksum_gen(uint8_t byte);

#endif /* MSP430_DXL_DYNAMIXEL_H_ */

