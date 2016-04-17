/*
 * author: Brian Ruiz
 * date:   February 18, 2016
 * school: George Mason University
 */

#ifndef CALCULATIONS_H_
#define CALCULATIONS_H_

#define PI 		3.14159f			// 'f' added to make single precision float

/* calculation functions */
#define R2T(x) 		(((((180*x)/(0.29*PI)) + 0x200) > 0x3FF) ? 0x3FF : (((180*x)/(0.29*PI)) + 0x200))
#define T2R(x)		((0.29*PI*(x-0x200))/180)
#define MR2T(x)		(((((180*x)/(0.088*PI)) + 0x800) > 0xFFF) ? 0xFFF : (((180*x)/(0.088*PI)) + 0x800))
#define MT2R(x)		((0.088*PI*(x-0x800))/180)
//#define S2T(x)		(((2.271f)/1000000)*x)

#endif /* CALCULATIONS_H_ */
