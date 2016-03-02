/*
 * author: Brian Ruiz
 * date:   December 19, 2015
 * school: George Mason University
 */

#ifndef MSP430_DXL_CALCULATIONS_H_
#define MSP430_DXL_CALCULATIONS_H_

#include <stdint.h>

/* variable macros */
#define PI		3.14159

/* conversions */
uint16_t rad_to_tick(float rad);
float tick_to_rad(uint16_t tick);
uint16_t sec_to_tick(float sec);

#endif /* MSP430_DXL_CALCULATIONS_H_ */
