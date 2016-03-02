#include "dynamixel.h"
#include "calculations.h"
#include <stdint.h>

void curl()
{
	sync_ids[0] = THUMB_KNUCKLE;
	sync_positions[0] = rad_to_tick(-2*PI/5);
	sync_speeds[0] = 0x175;

	sync_ids[1] = THUMB_POINT;
	sync_positions[1] = rad_to_tick(-(PI/4));
	sync_speeds[1] = 0x175;

	sync_ids[2] = PINKY_BASE;
	sync_positions[2] = rad_to_tick(2*PI/5);
	sync_speeds[2] = 0x150;

	sync_ids[3] = PINKY_KNUCKLE;
	sync_positions[3] = rad_to_tick(2*PI/5);
	sync_speeds[3] = 0x150;

	sync_ids[4] = PINKY_POINT;
	sync_positions[4] = rad_to_tick(PI/4);
	sync_speeds[4] = 0x150;

	sync_ids[5] = INDEX_BASE;
	sync_positions[5] = rad_to_tick(2*PI/5);
	sync_speeds[5] = 0x150;

	sync_ids[6] = INDEX_KNUCKLE;
	sync_positions[6] = rad_to_tick(2*PI/5);
	sync_speeds[6] = 0x150;

	sync_ids[7] = INDEX_POINT;
	sync_positions[7] = rad_to_tick(PI/4);
	sync_speeds[7] = 0x150;

	sync_write(8);
}

void open()
{
	uint8_t i, id = 0x10;

	for (i = 0; i < 9; i++)
	{
		sync_ids[i] = id;
		sync_positions[i] = 512;
		switch(id)
		{
			case THUMB_KNUCKLE:
				sync_speeds[i] = 0x110;
				break;
			case THUMB_POINT:
				sync_speeds[i] = 0x115;
				break;
			case INDEX_BASE:
			case PINKY_BASE:
				sync_speeds[i] = 0x175;
				break;
			default:
				sync_speeds[i] = 0x150;
				break;
		}
		id++;
	}
	sync_write(9);
}

void thumbs_up()
{
	sync_ids[0] = THUMB_KNUCKLE;
	sync_positions[0] = rad_to_tick(PI/2);
	sync_speeds[0] = 0x175;

	sync_ids[1] = THUMB_POINT;
	sync_positions[1] = rad_to_tick(0);
	sync_speeds[1] = 0x175;

	sync_ids[2] = PINKY_BASE;
	sync_positions[2] = rad_to_tick(2*PI/5);
	sync_speeds[2] = 0x150;

	sync_ids[3] = PINKY_KNUCKLE;
	sync_positions[3] = rad_to_tick(2*PI/5);
	sync_speeds[3] = 0x150;

	sync_ids[4] = PINKY_POINT;
	sync_positions[4] = rad_to_tick(PI/4);
	sync_speeds[4] = 0x150;

	sync_ids[5] = INDEX_BASE;
	sync_positions[5] = rad_to_tick(2*PI/5);
	sync_speeds[5] = 0x150;

	sync_ids[6] = INDEX_KNUCKLE;
	sync_positions[6] = rad_to_tick(2*PI/5);
	sync_speeds[6] = 0x150;

	sync_ids[7] = INDEX_POINT;
	sync_positions[7] = rad_to_tick(PI/4);
	sync_speeds[7] = 0x150;

	sync_write(8);
}

void point()
{
	sync_ids[0] = THUMB_BASE;
	sync_positions[0] = rad_to_tick(-(PI/4));
	sync_speeds[0] = 0x150;

	sync_ids[1] = THUMB_KNUCKLE;
	sync_positions[1] = rad_to_tick(-(PI/8));
	sync_speeds[1] = 0x150;

	sync_ids[2] = THUMB_POINT;
	sync_positions[2] = rad_to_tick(-(PI/4));
	sync_speeds[2] = 0x150;

	sync_ids[3] = PINKY_BASE;
	sync_positions[3] = rad_to_tick(2*PI/5);
	sync_speeds[3] = 0x150;

	sync_ids[4] = PINKY_KNUCKLE;
	sync_positions[4] = rad_to_tick(2*PI/5);
	sync_speeds[4] = 0x150;

	sync_ids[5] = PINKY_POINT;
	sync_positions[5] = rad_to_tick(PI/4);
	sync_speeds[5] = 0x150;

	sync_ids[6] = INDEX_BASE;
	sync_positions[6] = 512;
	sync_speeds[6] = 0x200;

	sync_ids[7] = INDEX_KNUCKLE;
	sync_positions[7] = 512;
	sync_speeds[7] = 0x200;

	sync_ids[8] = INDEX_POINT;
	sync_positions[8] = 512;
	sync_speeds[8] = 0x200;

	sync_write(9);
}

void okay()
{
	sync_ids[0] = THUMB_BASE;
	sync_positions[0] = rad_to_tick(-(PI/2))+2;
	sync_speeds[0] = 0x200;

	sync_ids[1] = THUMB_KNUCKLE;
	sync_positions[1] = rad_to_tick(PI/4);
	sync_speeds[1] = 0x150;

	sync_ids[2] = THUMB_POINT;
	sync_positions[2] = rad_to_tick(-(PI/4));
	sync_speeds[2] = 0x150;

	sync_ids[3] = PINKY_BASE;
	sync_positions[3] = 512;
	sync_speeds[3] = 0x150;

	sync_ids[4] = PINKY_KNUCKLE;
	sync_positions[4] = 512;
	sync_speeds[4] = 0x150;

	sync_ids[5] = PINKY_POINT;
	sync_positions[5] = 512;
	sync_speeds[5] = 0x150;

	sync_ids[6] = INDEX_BASE;
	sync_positions[6] = rad_to_tick(0.6541);
	sync_speeds[6] = 0x150;

	sync_ids[7] = INDEX_KNUCKLE;
	sync_positions[7] = rad_to_tick(PI/4);
	sync_speeds[7] = 0x150;

	sync_ids[8] = INDEX_POINT;
	sync_positions[8] = rad_to_tick(PI/4);
	sync_speeds[8] = 0x150;

	sync_write(9);
}
