/*
 * servo.h
 *
 *  Created on: Nov 2, 2021
 *      Author: wclemms
 */

#ifndef SERVO_H_
#define SERVO_H_

#include "Timer.h"
#include "lcd.h"

int rightValue;
int leftValue;
int pulsePeriod;

void ServoInit(void);
void RotateServo(float angle);
void ResetServo(void);
#endif /* SERVO_H_ */
