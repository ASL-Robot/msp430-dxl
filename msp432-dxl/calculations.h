/*
 * author: Brian Ruiz
 * date:   February 18, 2016
 * school: George Mason University
 */

#ifndef CALCULATIONS_H_
#define CALCULATIONS_H_

#define PI 		3.14159f			// 'f' added to make single precision float

/* calculation functions */
#define R2T(x) 		((((x*(180/PI))/0.29) + 512) > 1023) ? 0x3FF : (((x*(180/PI))/0.29) + 512)
#define T2R(x)		((tick*0.29f)*(PI/180))
#define S2T(x)		(((2.271f)/1000000)*sec)

#endif /* CALCULATIONS_H_ */
