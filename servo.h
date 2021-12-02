/*
 * servo.h
 *
 *  Created on: Nov 2, 2021
 *      Author: ndgartin
 */

#ifndef SERVO_H_
#define SERVO_H_

void better_servo_init();
int turn_servo_deg(char degrees);
void servo_calibration();
char get_deg();
//0 degrees
extern double servo_upper;
///180 deg default
extern double servo_lower;


#endif /* SERVO_H_ */
