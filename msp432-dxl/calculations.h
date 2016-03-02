/*
 * author: Brian Ruiz
 * date:   February 18, 2016
 * school: George Mason University
 */

#ifndef CALCULATIONS_H_
#define CALCULATIONS_H_

#define PI 		3.14159f			// 'f' added to make single precision float

/* calculation functions */
uint16_t rad_to_tick(float rad);
float tick_to_rad(uint16_t tick);
uint16_t sec_to_tick(float sec);



#endif /* CALCULATIONS_H_ */
