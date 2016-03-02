/*
 * author: Brian Ruiz
 * date:   February 26, 2015
 * school: George Mason University
 *
 * this file contains all of the high level apis needed for our specific project;
 * this is to be used in conjunction with dynamixel.h, dynamixel.c and dynamixel_apis.h.
 */

#include "dynamixel_apis.h"
#include "dynamixel.h"

void set_id(uint8_t old_id, uint8_t new_id)
{
	packet = 0;
	SET_ID(packet, old_id);
	SET_REG(packet, ID);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, new_id);
	if (old_id == 0xFE)				// not advisable
	{
		checksum_gen(); 			// produce first checksum
		SET_COMM(packet, 0x01);
		checksum_gen(); 			// produce second checksum
		motor_write();
		CLEAR_COMM(packet);
		motor_write();
	}
	else if (old_id < 0x10)
	{
		checksum_gen(); 			// produce first checksum
		motor_write();
	}
	else
	{
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
	}
}

void set_baud(uint8_t id, uint8_t rate)
{
	packet = 0;
	SET_ID(packet, id);
	SET_REG(packet, BAUD);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, rate);
	if (id == 0xFE)
	{
		checksum_gen(); 			// produce first checksum
		SET_COMM(packet, 0x01);
		checksum_gen(); 			// produce second checksum
		motor_write();
		CLEAR_COMM(packet);
		motor_write();
	}
	else if (id < 0x10)
	{
		checksum_gen(); 			// produce first checksum
		motor_write();
	}
	else
	{
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
	}
}

void set_return(uint8_t id, uint8_t level)
{
	packet = 0;
	SET_ID(packet, id);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, level);
	if (id == 0xFE)
	{
		SET_REG(packet, RETURN);
		checksum_gen();				// produce first checksum
		motor_write();
		CLEAR_REG(packet);
		SET_REG(packet, XL_RETURN);
		SET_COMM(packet, 0x01);
		checksum_gen(); 			// produce second checksum
		motor_write();
	}
	else if (id < 0x10)
	{
		SET_REG(packet, RETURN);
		checksum_gen();				// produce first checksum
		motor_write();
	}
	else
	{
		SET_REG(packet, XL_RETURN);
		SET_COMM(packet, 0x01);
		checksum_gen(); 			// produce second checksum
		motor_write();
	}
}

void set_delay(uint8_t id, uint8_t delay)
{
	packet = 0;
	SET_ID(packet, id);
	SET_REG(packet, DELAY);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, delay);
	if (id == 0xFE)
	{
		checksum_gen();
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
		CLEAR_COMM(packet);
		motor_write();
	}
	else if (id < 0x10)
	{
		checksum_gen();
		motor_write();
	}
	else
	{
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
	}
}

void normalize(uint8_t id)
{
	goal_position(id, 0x200, 0x050);
}

void torque_enable(uint8_t id)
{
	packet = 0;
	SET_ID(packet, id);
	SET_REG(packet, ENTORQUE);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, 0x01);
	if (id == 0xFE)
	{
		checksum_gen();
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
		CLEAR_COMM(packet);
		motor_write();
	}
	else if (id < 0x10)
	{
		checksum_gen();
		motor_write();
	}
	else
	{
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
	}
}

void torque_disable(uint8_t id)
{
	packet = 0;
	SET_ID(packet, id);
	SET_REG(packet, ENTORQUE);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, 0x00);
	if (id == 0xFE)
	{
		checksum_gen();
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
		CLEAR_COMM(packet);
		motor_write();
	}
	else if (id < 0x10)
	{
		checksum_gen();
		motor_write();
	}
	else
	{
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
	}
}

void set_torque(uint8_t id, uint16_t torque)
{
	packet = 0;
	SET_ID(packet, id);
	SET_PARAM(packet, 3);
	SET_INST(packet, WRITE);
	SET_1(packet, GET_1(torque));
	SET_2(packet, GET_2(torque));
	if (id == 0xFE)
	{
		SET_REG(packet, TORQUE);
		checksum_gen();
		motor_write();
		CLEAR_REG(packet);
		SET_COMM(packet, 0x01);
		SET_REG(packet, XL_TORQUE);
		checksum_gen();
		motor_write();
	}
	else if (id < 0x10)
	{
		SET_REG(packet, TORQUE);
		checksum_gen();
		motor_write();
	}
	else
	{
		SET_COMM(packet, 0x01);
		SET_REG(packet, XL_TORQUE);
		checksum_gen();
		motor_write();
	}
}

void joint_mode(uint8_t id)
{
	packet = 0;
	SET_ID(packet, id);
	SET_INST(packet, WRITE);
	if (id == 0xFE)
	{
		SET_REG(packet, CW);
		SET_PARAM(packet, 5);
		SET_1(packet, 0x01);
		SET_3(packet, 0xFF);
		SET_4(packet, 0x03);
		checksum_gen();
		motor_write();

		packet = 0;
		SET_ID(packet, id);
		SET_INST(packet, WRITE);
		SET_REG(packet, CONTROL);
		SET_PARAM(packet, 2);
		SET_COMM(packet, 0x01);
		SET_1(packet, 2);
		checksum_gen();
		motor_write();
	}
	else if (id < 0x10)
	{
		SET_REG(packet, CW);
		SET_PARAM(packet, 5);
		SET_1(packet, 0x01);
		SET_3(packet, 0xFF);
		SET_4(packet, 0x03);
		checksum_gen();
		motor_write();
	}
	else
	{
		SET_REG(packet, CONTROL);
		SET_PARAM(packet, 2);
		SET_COMM(packet, 0x01);
		SET_1(packet, 2);
		checksum_gen();
		motor_write();
	}
}

void led_on(uint8_t id)
{
	packet = 0;
	SET_ID(packet, id);
	SET_REG(packet, LED);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, 0x01);
	if (id == 0xFE)
	{
		checksum_gen();
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
		CLEAR_COMM(packet);
		motor_write();
	}
	else if (id < 0x10)
	{
		checksum_gen();
		motor_write();
	}
	else
	{
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
	}
}

void led_off(uint8_t id)
{
	packet = 0;
	SET_ID(packet, id);
	SET_REG(packet, LED);
	SET_PARAM(packet, 2);
	SET_INST(packet, WRITE);
	SET_1(packet, 0x00);
	if (id == 0xFE)
	{
		checksum_gen();
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
		CLEAR_COMM(packet);
		motor_write();
	}
	else if (id < 0x10)
	{
		checksum_gen();
		motor_write();
	}
	else
	{
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
	}
}

void goal_position(uint8_t id, uint16_t position, uint16_t speed)
{
	packet = 0;
	SET_ID(packet, id);
	SET_REG(packet, GOAL_POS);
	SET_PARAM(packet, 5);
	SET_INST(packet, WRITE);
	SET_1(packet, GET_1(position));
	SET_2(packet, GET_2(position));
	SET_3(packet, GET_1(speed));
	SET_4(packet, GET_2(speed));
	if (id == 0xFE)
	{
		checksum_gen();
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
		CLEAR_COMM(packet);
		motor_write();
	}
	else if (id < 0x10)
	{
		checksum_gen();
		motor_write();
	}
	else
	{
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
	}
}

uint16_t curr_position(uint8_t id)
{
	packet = 0;
	SET_ID(packet, id);
	SET_INST(packet, READ);
	SET_1(packet, 0x02);
	if (id == 0xFE)
		while(1);
	else if (id < 0x10)
	{
		SET_REG(packet, CURR_POS);
		SET_PARAM(packet, 2);
		checksum_gen();
		motor_read();
		return return_packet;
	}
	else
	{
		SET_COMM(packet, 0x01);
		SET_REG(packet, XL_CURR_POS);
		SET_PARAM(packet, 3);
		checksum_gen();
		motor_read();
		return return_packet;
	}
}

void register_goal_position(uint8_t id, uint16_t position, uint16_t speed)
{
	packet = 0;
	SET_ID(packet, id);
	SET_REG(packet, GOAL_POS);
	SET_PARAM(packet, 5);
	SET_INST(packet, REG_WRITE);
	SET_1(packet, GET_1(position));
	SET_2(packet, GET_2(position));
	SET_3(packet, GET_1(speed));
	SET_4(packet, GET_2(speed));
	if (id == 0xFE)
	{
		checksum_gen();
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
		CLEAR_COMM(packet);
		motor_write();
	}
	else if (id < 0x10)
	{
		checksum_gen();
		motor_write();
	}
	else
	{
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
	}
}

void action(uint8_t id)
{
	packet = 0;
	SET_ID(packet, id);
	SET_INST(packet, ACTION);
	if (id == 0xFE)
	{
		checksum_gen();
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
		CLEAR_COMM(packet);
		motor_write();
	}
	else if (id < 0x10)
	{
		checksum_gen();
		motor_write();
	}
	else
	{
		SET_COMM(packet, 0x01);
		checksum_gen();
		motor_write();
	}
}
