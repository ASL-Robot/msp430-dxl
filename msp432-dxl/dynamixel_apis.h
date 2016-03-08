/*
 * dynamixel_apis.h
 *
 *  Created on: Feb 26, 2016
 *      Author: Brian
 */

#ifndef DYNAMIXEL_APIS_H_
#define DYNAMIXEL_APIS_H_

#include <stdint.h>

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

/* gesture call */
void gesture(uint8_t g_id);

#endif /* DYNAMIXEL_APIS_H_ */
