/*
 * scan.c
 *
 *  Created on: Nov 19, 2021
 *      Author: ndgartin
 */


#include <math.h>
#include "scan.h"
#include "sonar.h"
#include "adc.h"
#include "servo.h"



int SonarScan(int angle, Ping* ping)
{
    turn_servo_deg(angle);
    timer_waitMillis(500);
    return GetSonarDistance(ping);
}

int IRRawScan(int angle)
{
    turn_servo_deg(angle);
    return adc_read();
}
int IRDistanceScan(int angle)
{
    turn_servo_deg(angle);
    return GetActualDistance(adc_read());
}
int GetActualDistance(int x)
{
    int actual = (-6.426e-15 * pow(x, 5)) + (5.919e-11 * pow(x, 4)) + (-2.175e-7 * pow(x, 3)) + (4.075e-4 * pow(x, 2)) + (-4.079e-1 * x) + 1.991e2;
    return actual;
}
