#include "dynamixel.h"
#include "calculations.h"
#include <stdint.h>

uint8_t curl_data[41] =     { 40, THUMB_KNUCKLE, GET_1(R2T(-2*PI/5)), GET_2(R2T(-2*PI/5)), 0x75, 0x01,
							  THUMB_POINT, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x75, 0x01,
							  PINKY_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  PINKY_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							  INDEX_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  INDEX_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  INDEX_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01 };

uint8_t open_data[46] =     { 45, THUMB_BASE, 0x00, 0x02, 0x50, 0x01,
							  THUMB_KNUCKLE, 0x00, 0x02, 0x10, 0x01,
							  THUMB_POINT, 0x00, 0x02, 0x15, 0x01,
							  PINKY_BASE, 0x00, 0x02, 0x75, 0x01,
							  PINKY_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							  PINKY_POINT, 0x00, 0x02, 0x50, 0x01,
							  INDEX_BASE, 0x00, 0x02, 0x75, 0x01,
							  INDEX_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							  INDEX_POINT, 0x00, 0x02, 0x50, 0x01 };

uint8_t thumbs_up_data[41] =  { 40, THUMB_KNUCKLE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x75, 0x01,
							  THUMB_POINT, GET_1(R2T(0)), GET_2(R2T(0)), 0x75, 0x01,
							  PINKY_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  PINKY_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							  INDEX_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  INDEX_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  INDEX_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01 };

uint8_t point_data[46] =    { 45, THUMB_BASE, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x50, 0x01,
							  THUMB_KNUCKLE, GET_1(R2T(-(PI/8))), GET_2(R2T(-(PI/8))), 0x50, 0x01,
							  THUMB_POINT, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x50, 0x01,
							  PINKY_BASE, 0x00, 0x02, 0x50, 0x01,
							  PINKY_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							  PINKY_POINT, 0x00, 0x02, 0x50, 0x01,
							  INDEX_BASE, GET_1(R2T(0.6541)), GET_2(R2T(0.6541)), 0x50, 0x01,
							  INDEX_KNUCKLE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							  INDEX_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01 };

uint8_t okay_data[46] =     { 45, THUMB_BASE, GET_1(R2T(-(PI/2))+2), GET_2(R2T(-(PI/2))+2), 0x00, 0x02,
							  THUMB_KNUCKLE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							  THUMB_POINT, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x50, 0x01,
							  PINKY_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  PINKY_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							  INDEX_BASE, 0x00, 0x02, 0x50, 0x01,
							  INDEX_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							  INDEX_POINT, 0x00, 0x02, 0x50, 0x01 };
void curl()
{
	sync_ids[0] = THUMB_KNUCKLE;
	sync_positions[0] = R2T(-2*PI/5);
	sync_speeds[0] = 0x175;

	sync_ids[1] = THUMB_POINT;
	sync_positions[1] = R2T(-(PI/4));
	sync_speeds[1] = 0x175;

	sync_ids[2] = PINKY_BASE;
	sync_positions[2] = R2T(2*PI/5);
	sync_speeds[2] = 0x150;

	sync_ids[3] = PINKY_KNUCKLE;
	sync_positions[3] = R2T(2*PI/5);
	sync_speeds[3] = 0x150;

	sync_ids[4] = PINKY_POINT;
	sync_positions[4] = R2T(PI/4);
	sync_speeds[4] = 0x150;

	sync_ids[5] = INDEX_BASE;
	sync_positions[5] = R2T(2*PI/5);
	sync_speeds[5] = 0x150;

	sync_ids[6] = INDEX_KNUCKLE;
	sync_positions[6] = R2T(2*PI/5);
	sync_speeds[6] = 0x150;

	sync_ids[7] = INDEX_POINT;
	sync_positions[7] = R2T(PI/4);
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
	sync_positions[0] = R2T(PI/2);
	sync_speeds[0] = 0x175;

	sync_ids[1] = THUMB_POINT;
	sync_positions[1] = R2T(0);
	sync_speeds[1] = 0x175;

	sync_ids[2] = PINKY_BASE;
	sync_positions[2] = R2T(2*PI/5);
	sync_speeds[2] = 0x150;

	sync_ids[3] = PINKY_KNUCKLE;
	sync_positions[3] = R2T(2*PI/5);
	sync_speeds[3] = 0x150;

	sync_ids[4] = PINKY_POINT;
	sync_positions[4] = R2T(PI/4);
	sync_speeds[4] = 0x150;

	sync_ids[5] = INDEX_BASE;
	sync_positions[5] = R2T(2*PI/5);
	sync_speeds[5] = 0x150;

	sync_ids[6] = INDEX_KNUCKLE;
	sync_positions[6] = R2T(2*PI/5);
	sync_speeds[6] = 0x150;

	sync_ids[7] = INDEX_POINT;
	sync_positions[7] = R2T(PI/4);
	sync_speeds[7] = 0x150;

	sync_write(8);
}

void point()
{
	sync_ids[0] = THUMB_BASE;
	sync_positions[0] = R2T(-(PI/4));
	sync_speeds[0] = 0x150;

	sync_ids[1] = THUMB_KNUCKLE;
	sync_positions[1] = R2T(-(PI/8));
	sync_speeds[1] = 0x150;

	sync_ids[2] = THUMB_POINT;
	sync_positions[2] = R2T(-(PI/4));
	sync_speeds[2] = 0x150;

	sync_ids[3] = PINKY_BASE;
	sync_positions[3] = R2T(2*PI/5);
	sync_speeds[3] = 0x150;

	sync_ids[4] = PINKY_KNUCKLE;
	sync_positions[4] = R2T(2*PI/5);
	sync_speeds[4] = 0x150;

	sync_ids[5] = PINKY_POINT;
	sync_positions[5] = R2T(PI/4);
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
	sync_positions[0] = R2T(-(PI/2))+2;
	sync_speeds[0] = 0x200;

	sync_ids[1] = THUMB_KNUCKLE;
	sync_positions[1] = R2T(PI/4);
	sync_speeds[1] = 0x150;

	sync_ids[2] = THUMB_POINT;
	sync_positions[2] = R2T(-(PI/4));
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
	sync_positions[6] = R2T(0.6541);
	sync_speeds[6] = 0x150;

	sync_ids[7] = INDEX_KNUCKLE;
	sync_positions[7] = R2T(PI/4);
	sync_speeds[7] = 0x150;

	sync_ids[8] = INDEX_POINT;
	sync_positions[8] = R2T(PI/4);
	sync_speeds[8] = 0x150;

	sync_write(9);
}
