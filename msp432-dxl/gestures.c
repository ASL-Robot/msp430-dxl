#include "dynamixel.h"
#include "gestures.h"
#include "calculations.h"
#include <stdint.h>

uint8_t curl[41] =	     { 40, THUMB_KNUCKLE, GET_1(XR2T(-2*PI/5)), GET_2(XR2T(-2*PI/5)), 0x75, 0x01,
							  THUMB_POINT, GET_1(XR2T(-(PI/4))), GET_2(XR2T(-(PI/4))), 0x75, 0x01,
							  PINKY_BASE, GET_1(XR2T(2*PI/5)), GET_2(XR2T(2*PI/5)), 0x50, 0x01,
							  PINKY_KNUCKLE, GET_1(XR2T(2*PI/5)), GET_2(XR2T(2*PI/5)), 0x50, 0x01,
							  PINKY_POINT, GET_1(XR2T(PI/4)), GET_2(XR2T(PI/4)), 0x50, 0x01,
							  INDEX_BASE, GET_1(XR2T(2*PI/5)), GET_2(XR2T(2*PI/5)), 0x50, 0x01,
							  INDEX_KNUCKLE, GET_1(XR2T(2*PI/5)), GET_2(XR2T(2*PI/5)), 0x50, 0x01,
							  INDEX_POINT, GET_1(XR2T(PI/4)), GET_2(XR2T(PI/4)), 0x50, 0x01 };

uint8_t open[46] =    	 { 45, THUMB_BASE, 0x00, 0x02, 0x50, 0x01,
							   THUMB_KNUCKLE, 0x00, 0x02, 0x10, 0x01,
							   THUMB_POINT, 0x00, 0x02, 0x15, 0x01,
							   PINKY_BASE, 0x00, 0x02, 0x75, 0x01,
							   PINKY_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							   PINKY_POINT, 0x00, 0x02, 0x50, 0x01,
							   INDEX_BASE, 0x00, 0x02, 0x75, 0x01,
							   INDEX_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							   INDEX_POINT, 0x00, 0x02, 0x50, 0x01 };

uint8_t thumbs_up[41] =  { 40, THUMB_KNUCKLE, GET_1(XR2T(PI/2)), GET_2(XR2T(PI/2)), 0x75, 0x01,
								THUMB_POINT, GET_1(XR2T(0)), GET_2(XR2T(0)), 0x75, 0x01,
								PINKY_BASE, GET_1(XR2T(2*PI/5)), GET_2(XR2T(2*PI/5)), 0x50, 0x01,
								PINKY_KNUCKLE, GET_1(XR2T(2*PI/5)), GET_2(XR2T(2*PI/5)), 0x50, 0x01,
								PINKY_POINT, GET_1(XR2T(PI/4)), GET_2(XR2T(PI/4)), 0x50, 0x01,
								INDEX_BASE, GET_1(XR2T(2*PI/5)), GET_2(XR2T(2*PI/5)), 0x50, 0x01,
								INDEX_KNUCKLE, GET_1(XR2T(2*PI/5)), GET_2(XR2T(2*PI/5)), 0x50, 0x01,
								INDEX_POINT, GET_1(XR2T(PI/4)), GET_2(XR2T(PI/4)), 0x50, 0x01 };

uint8_t point[46] =    { 45, THUMB_BASE, GET_1(XR2T(-(PI/4))), GET_2(XR2T(-(PI/4))), 0x50, 0x01,
							  THUMB_KNUCKLE, GET_1(XR2T(-(PI/8))), GET_2(XR2T(-(PI/8))), 0x50, 0x01,
							  THUMB_POINT, GET_1(XR2T(-(PI/4))), GET_2(XR2T(-(PI/4))), 0x50, 0x01,
							  PINKY_BASE, 0x00, 0x02, 0x50, 0x01,
							  PINKY_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							  PINKY_POINT, 0x00, 0x02, 0x50, 0x01,
							  INDEX_BASE, GET_1(XR2T(0.6541)), GET_2(XR2T(0.6541)), 0x50, 0x01,
							  INDEX_KNUCKLE, GET_1(XR2T(PI/4)), GET_2(XR2T(PI/4)), 0x50, 0x01,
							  INDEX_POINT, GET_1(XR2T(PI/4)), GET_2(XR2T(PI/4)), 0x50, 0x01 };

uint8_t okay[46] =     { 45, THUMB_BASE, GET_1(XR2T(-(PI/2))+2), GET_2(XR2T(-(PI/2))+2), 0x00, 0x02,
							  THUMB_KNUCKLE, GET_1(XR2T(PI/4)), GET_2(XR2T(PI/4)), 0x50, 0x01,
							  THUMB_POINT, GET_1(XR2T(-(PI/4))), GET_2(XR2T(-(PI/4))), 0x50, 0x01,
							  PINKY_BASE, GET_1(XR2T(2*PI/5)), GET_2(XR2T(2*PI/5)), 0x50, 0x01,
							  PINKY_KNUCKLE, GET_1(XR2T(2*PI/5)), GET_2(XR2T(2*PI/5)), 0x50, 0x01,
							  PINKY_POINT, GET_1(XR2T(PI/4)), GET_2(XR2T(PI/4)), 0x50, 0x01,
							  INDEX_BASE, 0x00, 0x02, 0x50, 0x01,
							  INDEX_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							  INDEX_POINT, 0x00, 0x02, 0x50, 0x01 };
