/*
 * author: Brian Ruiz
 * date:   February 18, 2016
 * school: George Mason University
 */

#ifndef CALCULATIONS_H_
#define CALCULATIONS_H_

#define PI 		3.14159f			// 'f' added to make single precision float

/* calculation functions */
#define XR2T(x) 	((((x*(180/PI))/0.29) + 512) > 1023) ? 0x3FF : (((x*(180/PI))/0.29) + 512)
#define R2T(x) 		((180*x)/(0.29*PI))
#define T2R(x)		((0.29*PI*x)/180)
#define S2T(x)		(((2.271f)/1000000)*x)

#endif /* CALCULATIONS_H_ */
