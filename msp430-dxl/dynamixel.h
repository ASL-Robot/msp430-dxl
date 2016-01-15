/*
 * author: Brian Ruiz
 * date:   December 19, 2015
 * school: George Mason University
 */

#ifndef MSP430_DXL_DYNAMIXEL_H_
#define MSP430_DXL_DYNAMIXEL_H_

#include <stdint.h>

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
#define XL_TORQUE		15		// two parameter write
#define XL_RETURN		17
#define XL_CURR_POS		37		// two paramter read

/* instruction types */
#define PING			0x01
#define READ			0x02
#define WRITE			0x03
#define REG_WRITE		0x04
#define ACTION			0x05
#define SYNC_READ		0x82
#define SYNC_WRITE 		0x83
#define BULK_READ		0x92
#define BULK_WRITE		0x93

/* macro functions/variables that may be helpful */
/* the send packet layout for the write primitive is as follows:
 *		[63:56] - id of motor to read/write to
 *		[55:48] - register to read/write to
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
#define GET_REG(x)		 ((UINT64_C(x) & 0x00FF000000000000) >> 48)
#define GET_ERROR(x) 	 ((UINT64_C(x) & 0x00FF000000000000) >> 48)
#define GET_PARAM(x)	 ((UINT64_C(x) & 0x0000FF0000000000) >> 40)
#define GET_INST(x) 	 ((UINT64_C(x) & 0x000000FF00000000) >> 32)
#define GET_4(x)		 ((UINT64_C(x) & 0x00000000FF000000) >> 24)   // get fourth parameter
#define GET_3(x) 		 ((UINT64_C(x) & 0x0000000000FF0000) >> 16)   // get third parameter
#define GET_2(x) 		 ((UINT64_C(x) & 0x000000000000FF00) >> 8)    // get second parameter
#define GET_1(x)          (UINT64_C(x) & 0x00000000000000FF)          // get first parameter
#define XL_GET_2(x) 		 ((UINT16_C(x) & 0xFF00) >> 8)    // get second parameter
#define XL_GET_1(x)          (UINT16_C(x) & 0xFF)            // get first parameter


/* setters */
#define SET_ID(x,y)		 (x |= (UINT64_C(y) << 56))
#define SET_REG(x,y)	 (x |= (UINT64_C(y) << 48))
#define SET_ERROR(x,y)   (x |= (UINT64_C(y) << 48))
#define SET_PARAM(x,y)   (x |= (UINT64_C(y) << 40))
#define SET_INST(x,y)    (x |= (UINT64_C(y) << 32))
#define SET_4(x,y) 		 (x |= (UINT64_C(y) << 24))
#define SET_3(x,y) 		 (x |= (UINT64_C(y) << 16))
#define SET_2(x,y) 		 (x |= (UINT64_C(y) << 8))
#define SET_1(x,y)       (x |= UINT64_C(y))

/* checksum generator */
uint16_t checksum_gen(uint64_t packet);						// for comm. one

/* read/write primitives */
void motor_write(uint64_t packet);
void sync_write(uint8_t *id, uint16_t *position, uint16_t *speed, uint8_t len);
uint16_t motor_read(uint64_t packet);

/* initialization APIs */
void set_id(uint8_t old_id, uint8_t new_id);
void set_baud(uint8_t id, uint8_t rate);
void set_return(uint8_t id, uint8_t level);
void set_delay(uint8_t id, uint8_t delay);
void normalize(uint8_t id);
void torque_enable(uint8_t id);
void torque_disable(uint8_t id);
void set_torque(uint8_t id, uint16_t torque);
void joint_mode(uint8_t id);
uint8_t ping(uint8_t id);
void led_on(uint8_t id);
void led_off(uint8_t id);

/* performance APIs */
void goal_position(uint8_t id, uint16_t position, uint16_t speed);
uint16_t curr_position(uint8_t id);
void register_goal_position(uint8_t id, uint16_t position, uint16_t speed);
void action(uint8_t id);

/* error handling */
void trap_error(uint8_t error);

#endif /* MSP430_DXL_DYNAMIXEL_H_ */
