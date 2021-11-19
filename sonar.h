/*
 * sonar.h
 *
 *  Created on: Oct 26, 2021
 *      Author: wclemms
 */

#ifndef SONAR_H_
#define SONAR_H_


#include "Timer.h"
#include "uart.h"

volatile bool high;
volatile bool done;
volatile unsigned int risingTime;
volatile unsigned int fallingTime;

typedef struct Pings
{
    int risingTime;
    int fallingTime;
    double pulseWidth;
    int overflows;
}Ping;


//Used to configure the robot to read the sonar signal
void SonarConfig(void);

void SonarSendStart(void);

int GetSonarDistance(Ping*);

int SonarCalcDistance(Ping*);

void Timer3Handler(void);
#endif /* SONAR_H_ */
