/*
 * author: Brian Ruiz
 * date:   February 21, 2015
 * school: George Mason University
 */

#ifndef MSP430_DXL_DYNAMIXEL_H_
#define MSP430_DXL_DYNAMIXEL_H_

#include <stdint.h>
#include <msp.h>

/* for sync_write */
extern uint8_t sync_ids[10];			// holds the ids that need to be written to/read from
extern uint16_t sync_positions[10];		// holds positions to move to
extern uint16_t sync_speeds[10];		// holds speeds to move to above positions to
extern uint8_t gesture; 				// holds the gesture the motors must perform

/* for sync_read, for xl-320s only! */
extern uint16_t sync_readings[10];		// holds current positions of motors from sync_read()

extern uint64_t packet;					// global packet for sending
extern uint16_t return_packet; 			// global packet for receiving
extern uint8_t checksum_1;				// global checksum for communication protocol one
extern uint16_t checksum_2; 			// global checksum for communication protocol two
extern uint8_t split; 					// global split to define split in sync write

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

/* motor ids */
#define THUMB_BASE 		0x10
#define THUMB_KNUCKLE 	0x11
#define THUMB_POINT		0x12
#define PINKY_BASE		0x13
#define PINKY_KNUCKLE	0x14
#define PINKY_POINT		0x15
#define INDEX_BASE		0x16
#define INDEX_KNUCKLE	0x17
#define INDEX_POINT		0x18

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
void checksum_gen();

/* read/write primitives */
void motor_write();
void sync_write(uint8_t len);
uint16_t motor_read();
void sync_read(uint8_t len);

/* error handling */
void trap_error(uint8_t error);

#endif /* MSP430_DXL_DYNAMIXEL_H_ */

