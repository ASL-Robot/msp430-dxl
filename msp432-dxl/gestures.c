#include "dynamixel.h"
#include "gestures.h"
#include "calculations.h"
#include <stdint.h>

uint8_t curl[41] =	     { 40, THUMB_KNUCKLE, GET_1(R2T(-2*PI/5)), GET_2(R2T(-2*PI/5)), 0x75, 0x01,
							  THUMB_POINT, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x75, 0x01,
							  PINKY_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  PINKY_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							  INDEX_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  INDEX_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  INDEX_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01 };

uint8_t open[56] =    	 { 55, THUMB_BASE, 0x00, 0x02, 0x50, 0x01,
							   THUMB_KNUCKLE, 0x00, 0x02, 0x10, 0x01,
							   THUMB_POINT, 0x00, 0x02, 0x15, 0x01,
							   PINKY_BASE, 0x00, 0x02, 0x75, 0x01,
							   PINKY_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							   PINKY_POINT, 0x00, 0x02, 0x50, 0x01,
							   INDEX_BASE, 0x00, 0x02, 0x75, 0x01,
							   INDEX_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							   INDEX_POINT, 0x00, 0x02, 0x50, 0x01,
							   PINKY_SPADE, 0x00, 0x02, 0x50, 0x01,
							   INDEX_SPADE, 0x00, 0x02, 0x50, 0x01 };

uint8_t thumbs_up[41] =  { 40, THUMB_KNUCKLE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x75, 0x01,
								THUMB_POINT, GET_1(R2T(0)), GET_2(R2T(0)), 0x75, 0x01,
								PINKY_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
								PINKY_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
								PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
								INDEX_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
								INDEX_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
								INDEX_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01 };

uint8_t point[46] =    { 45, THUMB_BASE, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x50, 0x01,
							  THUMB_KNUCKLE, GET_1(R2T(-(PI/8))), GET_2(R2T(-(PI/8))), 0x50, 0x01,
							  THUMB_POINT, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x50, 0x01,
							  PINKY_BASE, 0x00, 0x02, 0x50, 0x01,
							  PINKY_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							  PINKY_POINT, 0x00, 0x02, 0x50, 0x01,
							  INDEX_BASE, GET_1(R2T(0.6541)), GET_2(R2T(0.6541)), 0x50, 0x01,
							  INDEX_KNUCKLE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							  INDEX_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01 };

uint8_t okay[46] =     { 45, THUMB_BASE, GET_1(R2T(-(PI/2))+2), GET_2(R2T(-(PI/2))+2), 0x00, 0x02,
							  THUMB_KNUCKLE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							  THUMB_POINT, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x50, 0x01,
							  PINKY_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  PINKY_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							  PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							  INDEX_BASE, 0x00, 0x02, 0x50, 0x01,
							  INDEX_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							  INDEX_POINT, 0x00, 0x02, 0x50, 0x01 };

uint8_t letter_a[46] = { 45, THUMB_BASE, 0x00, 0x02, 0x00, 0x02,
						     THUMB_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
		 	  	   	         THUMB_POINT, 0x00, 0x02, 0x50, 0x01,
		 	  	   	    	 PINKY_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
	 	  	  	     	     PINKY_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
		 	  	   	   	     PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
	 	  	   	    	 	 INDEX_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
	 	  	   	    	     INDEX_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
	 	  	   	    	 	 INDEX_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01 };

uint8_t letter_b[11] = { 10, THUMB_KNUCKLE, GET_1(R2T(-(PI/10))), GET_2(R2T(-(PI/10))), 0x50, 0x01,
					  	     THUMB_POINT, GET_1(R2T(-(PI/8))), GET_2(R2T(-(PI/8))), 0x50, 0x01 };

uint8_t letter_c[46] = { 45, THUMB_BASE, GET_1(R2T(-(PI/3))), GET_2(R2T(-(PI/3))), 0x50, 0x01,
							 THUMB_KNUCKLE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 THUMB_POINT, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x50, 0x01,
							 PINKY_BASE, 0x00, 0x02, 0x50, 0x01,
							 PINKY_KNUCKLE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 INDEX_BASE, 0x00, 0x02, 0x50, 0x01,
							 INDEX_KNUCKLE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 INDEX_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01 };

uint8_t letter_d[46] = { 45, THUMB_BASE, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x50, 0x01,
							 THUMB_KNUCKLE, GET_1(R2T(-(PI/8))), GET_2(R2T(-(PI/8))), 0x50, 0x01,
							 THUMB_POINT, GET_1(R2T(-(PI/6))), GET_2(R2T(-(PI/6))), 0x50, 0x01,
							 PINKY_BASE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 PINKY_KNUCKLE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x50, 0x01,
							 PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 INDEX_BASE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_KNUCKLE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_POINT, 0x00, 0x02, 0x00, 0x02 };

uint8_t letter_e[46] = { 45, THUMB_BASE, 0x00, 0x02, 0x50, 0x01,
							 THUMB_KNUCKLE, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x50, 0x01,
							 THUMB_POINT, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x50, 0x01,
							 PINKY_BASE, 0x00, 0x02, 0x00, 0x02,
							 PINKY_KNUCKLE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x00, 0x02,
							 PINKY_POINT, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x00, 0x02,
							 INDEX_BASE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_KNUCKLE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x00, 0x02,
							 INDEX_POINT, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x00, 0x02 };

uint8_t letter_f[46] = { 45, THUMB_BASE, GET_1(R2T(-(PI/2)))+2, GET_2(R2T(-(PI/2)))+2, 0x00, 0x02,
							 THUMB_KNUCKLE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 THUMB_POINT, GET_1(R2T(-(PI/2.75))), GET_2(R2T(-(PI/2.75))), 0x50, 0x01,
							 PINKY_BASE, 0x00, 0x02, 0x50, 0x01,
							 PINKY_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							 PINKY_POINT, 0x00, 0x02, 0x50, 0x01,
							 INDEX_BASE, GET_1(R2T(PI/10)), GET_2(R2T(PI/10)), 0x50, 0x01,
							 INDEX_KNUCKLE, GET_1(R2T(PI/2.75)), GET_2(R2T(PI/2.75)), 0x50, 0x01,
							 INDEX_POINT, GET_1(R2T(PI/3)), GET_2(R2T(PI/3)), 0x50, 0x01 };

uint8_t letter_g[46] = { 45, THUMB_BASE, GET_1(R2T(-(PI/2.25))), GET_2(R2T(-(PI/2.25))), 0x50, 0x01,
							 THUMB_KNUCKLE, GET_1(R2T(PI/12)), GET_2(R2T(PI/12)), 0x50, 0x01,
							 THUMB_POINT, 0x00, 0x02, 0x50, 0x01,
							 PINKY_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							 PINKY_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							 PINKY_POINT,  GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 INDEX_BASE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_KNUCKLE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_POINT, 0x00, 0x02, 0x00, 0x02 };

uint8_t letter_h[46] = { 45, THUMB_BASE, GET_1(R2T(-(PI/2.25))), GET_2(R2T(-(PI/2.25))), 0x50, 0x01,
							 THUMB_KNUCKLE, GET_1(R2T(PI/12)), GET_2(R2T(PI/12)), 0x50, 0x01,
							 THUMB_POINT, 0x00, 0x02, 0x50, 0x01,
							 PINKY_BASE, 0x00, 0x02, 0x50, 0x01,
							 PINKY_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							 PINKY_POINT, 0x00, 0x02, 0x50, 0x01,
							 INDEX_BASE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_KNUCKLE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_POINT, 0x00, 0x02, 0x00, 0x02 };

uint8_t letter_i[46] = { 45, THUMB_BASE, 0x00, 0x02, 0x50, 0x01,
							 THUMB_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							 THUMB_POINT, GET_1(R2T(-(PI/6))), GET_2(R2T(-(PI/6))), 0x01,
							 PINKY_BASE, 0x00, 0x02, 0x50, 0x01,
							 PINKY_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							 PINKY_POINT, 0x00, 0x02, 0x50, 0x01,
							 INDEX_BASE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x00, 0x02,
							 INDEX_KNUCKLE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x00, 0x02,
							 INDEX_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x00, 0x02 };

uint8_t letter_k[46] = { 45, THUMB_BASE, GET_1(R2T(-(PI/2.25))), GET_2(R2T(-(PI/2.25))), 0x50, 0x01,
							 THUMB_KNUCKLE, GET_1(R2T(PI/12)), GET_2(R2T(PI/12)), 0x50, 0x01,
							 THUMB_POINT, 0x00, 0x02, 0x50, 0x01,
							 PINKY_BASE, GET_1(R2T(PI/2.5)), GET_2(R2T(PI/2.5)), 0x50, 0x01,
							 PINKY_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							 PINKY_POINT, 0x00, 0x02, 0x50, 0x01,
							 INDEX_BASE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_KNUCKLE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_POINT, 0x00, 0x02, 0x00, 0x02 };

uint8_t letter_l[46] = { 45, THUMB_BASE, 0x00, 0x02, 0x50, 0x01,
							 THUMB_KNUCKLE, GET_1(R2T(PI/2.5)), GET_2(R2T(PI/2.5)), 0x75, 0x01,
							 THUMB_POINT, 0x00, 0x02, 0x75, 0x01,
							 PINKY_BASE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							 PINKY_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							 PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 INDEX_BASE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_KNUCKLE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_POINT, 0x00, 0x02, 0x00, 0x02 };

uint8_t letter_n[56] = { 55, THUMB_BASE, 0x00, 0x02, 0x00, 0x02,
							 THUMB_KNUCKLE, GET_1(R2T(-(PI/2.75))), GET_2(R2T(-(PI/2.75))), 0x50, 0x02,
							 THUMB_POINT, 0x00, 0x02, 0x75, 0x01,
							 PINKY_BASE, GET_1(R2T(PI/2.25)), GET_2(R2T(PI/2.25)), 0x50, 0x01,
							 PINKY_KNUCKLE, GET_1(R2T(PI/2.75)), GET_2(R2T(PI/2.75)), 0x50, 0x01,
							 PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 INDEX_BASE, GET_1(R2T(PI/2.25)), GET_2(R2T(PI/2.25)), 0x50, 0x01,
							 INDEX_KNUCKLE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 INDEX_POINT, GET_1(R2T(PI/3)), GET_2(R2T(PI/3)), 0x50, 0x01,
			 	  	   	     PINKY_SPADE, GET_1(R2T(-(PI/6.5))), GET_2(R2T(-(PI/6.5))), 0x50, 0x01,
	 	  	   	   	    	 INDEX_SPADE, GET_1(R2T(PI/6)), GET_2(R2T(PI/6)), 0x50, 0x01 };

uint8_t letter_o[46] = { 45, THUMB_BASE, GET_1(R2T(-(PI/2)))+2, GET_2(R2T(-(PI/2)))+2, 0x00, 0x02,
							 THUMB_KNUCKLE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 THUMB_POINT, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x50, 0x01,
							 PINKY_BASE, GET_1(R2T(PI/8)), GET_2(R2T(PI/8)), 0x50, 0x01,
							 PINKY_KNUCKLE, GET_1(R2T(PI/3)), GET_2(R2T(PI/3)), 0x50, 0x01,
							 PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 INDEX_BASE, GET_1(R2T(PI/8)), GET_2(R2T(PI/8)), 0x50, 0x01,
							 INDEX_KNUCKLE, GET_1(R2T(PI/3)), GET_2(R2T(PI/3)), 0x50, 0x01,
							 INDEX_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01 };

uint8_t letter_r[56] = { 55, THUMB_BASE, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x00, 0x02,
							 THUMB_KNUCKLE, GET_1(R2T(-(PI/4.25))), GET_2(R2T(-(PI/4.25))), 0x50, 0x02,
							 THUMB_POINT, 0x00, 0x02, 0x75, 0x01,
							 PINKY_BASE, 0x00, 0x02, 0x50, 0x01,
							 PINKY_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							 PINKY_POINT, 0x00, 0x02, 0x50, 0x01,
							 INDEX_BASE, GET_1(R2T(PI/4.25)), GET_2(R2T(PI/4.25)), 0x50, 0x02,
							 INDEX_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							 INDEX_POINT, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x50, 0x01,
			 	  	   	     PINKY_SPADE, GET_1(R2T(PI/8)), GET_2(R2T(PI/8)), 0x50, 0x01,
	 	  	   	   	    	 INDEX_SPADE, GET_1(R2T(-(PI/4.5))), GET_2(R2T(-(PI/4.5))), 0x50, 0x01 };

uint8_t letter_s[46] = { 45, THUMB_BASE, 0x00, 0x02, 0x50, 0x01,
							 THUMB_KNUCKLE, GET_1(R2T(PI/6)), GET_2(R2T(PI/6)), 0x50, 0x01,
							 THUMB_POINT, GET_1(R2T(-(PI/2.25))), GET_2(R2T(-(PI/2.25))), 0x50, 0x01,
							 PINKY_BASE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x50, 0x01,
							 PINKY_KNUCKLE, GET_1(R2T(PI/3)), GET_2(R2T(PI/3)), 0x50, 0x01,
							 PINKY_POINT, GET_1(R2T(PI/6)), GET_2(R2T(PI/6)), 0x50, 0x01,
							 INDEX_BASE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x00, 0x02,
							 INDEX_KNUCKLE, GET_1(R2T(PI/3)), GET_2(R2T(PI/3)), 0x00, 0x02,
							 INDEX_POINT, GET_1(R2T(PI/6)), GET_2(R2T(PI/6)), 0x00, 0x02 };

uint8_t letter_t[56] = { 55, THUMB_BASE, 0x00, 0x02, 0x50, 0x01,
							 THUMB_KNUCKLE, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x50, 0x02,
							 THUMB_POINT, 0x00, 0x02, 0x75, 0x01,
							 PINKY_BASE, GET_1(R2T(PI/3)), GET_2(R2T(PI/3)), 0x50, 0x01,
							 PINKY_KNUCKLE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x50, 0x01,
							 PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 INDEX_BASE, GET_1(R2T(PI/6)), GET_2(R2T(PI/6)), 0x50, 0x02,
							 INDEX_KNUCKLE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x50, 0x01,
							 INDEX_POINT, GET_1(R2T(PI/3)), GET_2(R2T(PI/3)), 0x50, 0x01,
			 	  	   	     PINKY_SPADE, 0x00, 0x02, 0x50, 0x01,
	 	  	   	   	    	 INDEX_SPADE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01 };

uint8_t letter_u[51] = { 50, THUMB_BASE, 0x00, 0x02, 0x50, 0x01,
							 THUMB_KNUCKLE, GET_1(R2T(-(PI/3))), GET_2(R2T(-(PI/3))), 0x75, 0x01,
							 THUMB_POINT, 0x00, 0x02, 0x75, 0x01,
							 PINKY_BASE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 PINKY_KNUCKLE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x50, 0x01,
							 PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 INDEX_BASE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_KNUCKLE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_POINT, 0x00, 0x02, 0x00, 0x02,
			 	  	   	     PINKY_SPADE, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x00, 0x02 };

uint8_t letter_v[56] = { 55, THUMB_BASE, 0x00, 0x02, 0x50, 0x01,
							 THUMB_KNUCKLE, GET_1(R2T(-(PI/3))), GET_2(R2T(-(PI/3))), 0x75, 0x01,
							 THUMB_POINT, 0x00, 0x02, 0x75, 0x01,
							 PINKY_BASE, 0x00, 0x02, 0x00, 0x02,
							 PINKY_KNUCKLE, 0x00, 0x02, 0x00, 0x02,
							 PINKY_POINT, 0x00, 0x02, 0x00, 0x02,
							 INDEX_BASE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_KNUCKLE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_POINT, 0x00, 0x02, 0x00, 0x02,
			 	  	   	     PINKY_SPADE, GET_1(R2T(-(PI/14))), GET_2(R2T(-(PI/14))), 0x00, 0x02,
	 	  	   	   	    	 INDEX_SPADE, GET_1(R2T(PI/14)), GET_2(R2T(PI/14)), 0x00, 0x02 };

uint8_t letter_x[46] = { 45, THUMB_BASE, 0x00, 0x02, 0x50, 0x01,
							 THUMB_KNUCKLE, GET_1(R2T(-(PI/3))), GET_2(R2T(-(PI/3))), 0x75, 0x01,
							 THUMB_POINT, GET_1(R2T(-(PI/4))), GET_2(R2T(-(PI/4))), 0x75, 0x01,
							 PINKY_BASE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x50, 0x01,
							 PINKY_KNUCKLE, GET_1(R2T(2*PI/5)), GET_2(R2T(2*PI/5)), 0x50, 0x01,
							 PINKY_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x50, 0x01,
							 INDEX_BASE, 0x00, 0x02, 0x00, 0x02,
							 INDEX_KNUCKLE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x00, 0x02,
							 INDEX_POINT, GET_1(R2T(PI/3)), GET_2(R2T(PI/3)), 0x00, 0x02 };

uint8_t letter_y[46] = { 45, THUMB_BASE, 0x00, 0x02, 0x50, 0x01,
							 THUMB_KNUCKLE, GET_1(R2T(PI/3)), GET_2(R2T(PI/3)), 0x50, 0x01,
							 THUMB_POINT, 0x00, 0x02, 0x50, 0x01,
							 PINKY_BASE, 0x00, 0x02, 0x50, 0x01,
							 PINKY_KNUCKLE, 0x00, 0x02, 0x50, 0x01,
							 PINKY_POINT, 0x00, 0x02, 0x50, 0x01,
							 INDEX_BASE, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x00, 0x02,
							 INDEX_KNUCKLE, GET_1(R2T(PI/2)), GET_2(R2T(PI/2)), 0x00, 0x02,
							 INDEX_POINT, GET_1(R2T(PI/4)), GET_2(R2T(PI/4)), 0x00, 0x02 };
