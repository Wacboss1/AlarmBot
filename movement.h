/*
 * movement.h
 *
 *  Created on: Sep 7, 2021
 *      Author: wclemms
 */
#include "open_interface.h"
#include "lcd.h"

bool contact;

#ifndef MOVEMENT_H_
#define MOVEMENT_H_
void adjustForHardware(int*, int*);
void stopWheels(void);
void move(int, int, double, bool, oi_t*);
void reverse(int, int, double, oi_t*);
//degrees counter-clockwise
void rotateLeft(double, oi_t*);
void rotateRight(double, oi_t*);
void onLeftBumper(oi_t*);
void onRightBumper(oi_t*);
void ControlMove(char , int, int );
#endif /* MOVEMENT_H_ */
