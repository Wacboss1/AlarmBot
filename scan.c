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
#include "botconfig.h"
#include "uart.h"
//#include "distancesensor.h"
#define   _SCAN(a,b) simpleScan(a, b)

extern int num_objs_list[4];
scanned_obj front_objects[40];

scan_handle all_scans[180];

#ifndef IR_TO_DIST(a)
#define IR_TO_DIST(a)  GetActualDistance(a)
#endif



int simpleScan(char deg, scan_handle * scn) {
    turn_servo_deg(deg);
    ///waiting for a long time
    ///i reccomend guessing the amount of time it will take the servo to move (current_deg - new_deg) * (10 milli per degree)
    //timer_waitMillis(500);
 //   timer_waitMillis()
    scn->angle=deg;
    Ping png;
    scn->ping = GetSonarDistance(&png);
    scn->IR = IRDistanceScan(deg);
    return 0;
}

char scan180_alarmbot(scanned_obj objarray[])
{

    unsigned char nscans = 40;
    int deg_multiplier = 180 / nscans;
    int i = 0;
    int direction =0;
    num_objs_list[direction] = 0;


        // ========Do the whole first scan===========
        int num_samples = 3;
        float averaged_ir_value = 0;
        scan_handle temp;
        for (i = 0; i < 180; i += deg_multiplier)
        {
            averaged_ir_value = 0;
            int j = 0;
            for (j = 0; j < num_samples; j++)
            {
                ///if using our adc, only use the cybotscan once to aim the servo
                if (  j==0) {
                _SCAN(i, &temp);
                }

                averaged_ir_value += temp.IR;

                if (VERY_VERBOSE)
                {
                    botprintf("Angle %d, Raw value: %d, DistVal: %d\n\r", i,
                              temp.IR, IR_TO_DIST(temp.IR));
                }
            }

            if (VERY_VERBOSE)
            {

                botprintf("Angle: %d, sum: %f, Average: %f, cast to int %d\n\r",
                          i, averaged_ir_value,
                          (averaged_ir_value / (float) num_samples),
                          ((int) (averaged_ir_value / (float) num_samples)));
            }
            averaged_ir_value = averaged_ir_value / (float) num_samples;
            //do we need this line?
            // _SCAN(ANGLE_FRM_SCAN(i), &all_scans[i]);
            all_scans[i].IR = (int) averaged_ir_value;
            all_scans[i].ping= temp.ping;
        }

   // output_scan_data();
    return 0;
}

char objectsFrmScns(scanned_obj* objectArray)
{
    Ping* ping = malloc(sizeof(Ping));

    unsigned char nscans = 40;
    int degMulti = 180 / nscans;
    int objectNumber = 0;
    int i = 0;

    for(i = 0; i < 180; i += degMulti)
    {
        int firstAngle, secondAngle;
        scan_handle currentScan = all_scans[i];
        int distance = GetActualDistance(currentScan.IR);
        if(distance <= 50)
        {
            firstAngle = i;
            while(distance <= 50 && i < 180)
            {
                i += degMulti;
                currentScan = all_scans[i];
            }
            secondAngle = i - degMulti;

            //Does not use sonar for distance
            scanned_obj newObject;
            newObject.angle = firstAngle;
            newObject.angle2 = secondAngle;
            newObject.radial_width = secondAngle - firstAngle;
            newObject.distance = SonarScan((float)newObject.radial_width/2, ping);
            newObject.straight_width = GetStraightWidth(newObject.distance, newObject.radial_width);

            objectArray[objectNumber] = newObject;


            if (secondAngle - firstAngle > 0 &&
                firstAngle != 0 &&
                !(secondAngle >= (180 - degMulti)))
            {
                objectNumber++;
            }
        }
    }
}

float GetStraightWidth(float distance, int angle)
{
    float new_angle = angle * 3.14159 / 180;
    return 2 * distance * sin((double) (new_angle) / 2);
}

void printScn(scan_handle * scn) {
    //int dist = GetActualDistance(scn->IR);
    botprintf("deg: %d, ping: %d, dist(IR): %d\n\r",scn->angle,scn->ping,scn->IR);
}

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
