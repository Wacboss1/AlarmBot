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

 int num_objs_list[4];
scanned_obj front_objects[40];

scan_handle all_scans[180];


#define IR_TO_DIST(a)  (a)

#define USE_DUMMYSCANS
#ifdef USE_DUMMYSCANS
char scan180_alarmbot() {
    scan_handle dummy_scans[180] = {{0,141,148},{0,0,0},{0,0,0},{0,0,0},{0,74,134},{0,0,0},{0,0,0},{0,0,0},{0,66,177},{0,0,0},{0,0,0},{0,0,0},{0,141,177},{0,0,0},{0,0,0},{0,0,0},{0,141,166},{0,0,0},{0,0,0},{0,0,0},{0,61,177},{0,0,0},{0,0,0},{0,0,0},{0,73,170},{0,0,0},{0,0,0},{0,0,0},{0,85,171},{0,0,0},{0,0,0},{0,0,0},{0,86,160},{0,0,0},{0,0,0},{0,0,0},{0,67,170},{0,0,0},{0,0,0},{0,0,0},{0,73,121},{0,0,0},{0,0,0},{0,0,0},{0,95,171},{0,0,0},{0,0,0},{0,0,0},{0,141,92},{0,0,0},{0,0,0},{0,0,0},{0,261,76},{0,0,0},{0,0,0},{0,0,0},{0,291,115},{0,0,0},{0,0,0},{0,0,0},{0,141,127},{0,0,0},{0,0,0},{0,0,0},{0,72,176},{0,0,0},{0,0,0},{0,0,0},{0,139,176},{0,0,0},{0,0,0},{0,0,0},{0,141,176},{0,0,0},{0,0,0},{0,0,0},{0,28,122},{0,0,0},{0,0,0},{0,0,0},{0,27,25},{0,0,0},{0,0,0},{0,0,0},{0,36,18},{0,0,0},{0,0,0},{0,0,0},{0,20,17},{0,0,0},{0,0,0},{0,0,0},{0,20,16},{0,0,0},{0,0,0},{0,0,0},{0,20,16},{0,0,0},{0,0,0},{0,0,0},{0,20,16},{0,0,0},{0,0,0},{0,0,0},{0,20,17},{0,0,0},{0,0,0},{0,0,0},{0,27,19},{0,0,0},{0,0,0},{0,0,0},{0,27,36},{0,0,0},{0,0,0},{0,0,0},{0,131,97},{0,0,0},{0,0,0},{0,0,0},{0,129,107},{0,0,0},{0,0,0},{0,0,0},{0,127,155},{0,0,0},{0,0,0},{0,0,0},{0,131,176},{0,0,0},{0,0,0},{0,0,0},{0,130,174},{0,0,0},{0,0,0},{0,0,0},{0,131,150},{0,0,0},{0,0,0},{0,0,0},{0,130,122},{0,0,0},{0,0,0},{0,0,0},{0,131,114},{0,0,0},{0,0,0},{0,0,0},{0,131,141},{0,0,0},{0,0,0},{0,0,0},{0,129,114},{0,0,0},{0,0,0},{0,0,0},{0,153,177},{0,0,0},{0,0,0},{0,0,0},{0,168,177},{0,0,0},{0,0,0},{0,0,0},{0,141,177},{0,0,0},{0,0,0},{0,0,0},{0,222,177},{0,0,0},{0,0,0},{0,0,0},{0,141,177},{0,0,0},{0,0,0},{0,0,0},{0,141,177},{0,0,0},{0,0,0},{0,0,0}};

    int i = 0;
    for (i=0;i<180;i++) {
        all_scans[i]=dummy_scans[i];
        all_scans[i].angle=i;
    }
}
#else
char scan180_alarmbot()
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
                              temp.IR, temp.IR);
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

        if (VERBOSE) {
    //output_scan_data(front_objects);
        }
    return 0;
}
#endif

int test_scans_print_scans() {
    scan180_alarmbot();

    int i;
    for (i=0;i<180;i++) {

        botprintf("{%d,%d,%d},",all_scans[i].angle,all_scans[i].ping,all_scans[i].IR);
    }

}

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




float get_width(float dist1, int angle)
{
    float new_angle = angle * 3.14159 / 180;
    return 2 * dist1 * sin((double) (new_angle) / 2);
}

char objsFrmScns(scanned_obj *objarray)
{
    //these and those in the above function should be in a struct or something
    unsigned char nscans = 40;
    int deg_multiplier = 180 / nscans;
    int i = 0;
    int direction = 0;


    // =======Find edges of objects=======
    for (i = 0; i < 180; i += deg_multiplier)
    {
        int first_angle;
        int second_angle;

        scan_handle current_scan = all_scans[i];

        if ((current_scan.IR) < 50)
        {
            first_angle = i;
            while ((current_scan.IR) < 50)
            {
                i += deg_multiplier;
                current_scan = all_scans[i];
                if (i >= 180)
                    break;
            }
            second_angle = i - deg_multiplier;

            // ---===Update Object Values===---
            objarray[num_objs_list[direction]].angle = first_angle;
            objarray[num_objs_list[direction]].angle2 = second_angle;
            objarray[num_objs_list[direction]].radial_width = second_angle
                    - first_angle;

                //using ping for distance at middle of object
            Ping  ping;
            objarray[num_objs_list[0]].distance = (float) SonarScan((objarray[num_objs_list[direction]].radial_width)+first_angle, &ping);
            objarray[num_objs_list[direction]].straight_width = get_width(
                    objarray[num_objs_list[direction]].distance,
                    second_angle - first_angle);

            objarray[num_objs_list[direction]].scan_direction = direction;

            if (second_angle - first_angle > 0 && first_angle != 0
                    && !(second_angle >= (180 - deg_multiplier)))
                num_objs_list[direction]++;
            else if (VERY_VERBOSE)
            {
                botprintf(
                        "Object not added! First angle: %d Second Angle: %d, distance: %f ",
                        first_angle, second_angle,
                        objarray[num_objs_list[direction]].distance);

                if (second_angle - first_angle <= 0)
                    botprintf(
                            "Object with negative or zero dimensions! First angle %d, second angle: %d\n\r",
                            first_angle, second_angle);
                if (first_angle == 0)
                    botprintf("(starting angle is zero)\n\r");
                if (second_angle >= 180 - deg_multiplier)
                    botprintf("2nd angle above the limit");

            }
            if (VERY_VERBOSE)
            {
                if (objarray[num_objs_list[direction]].distance != 0
                        && num_objs_list[direction] == 0)
                {
                    botprintf("Detected secret shit! Dumping...\n\r");
                    int true_num_obj = num_objs_list[direction];
                    num_objs_list[direction] = 5;
                    print_found_objects(objarray);
                    botprintf("*Flush*\n\r");
                    num_objs_list[direction] = true_num_obj;

                }
            }

        }
    }

    return 0;

}
void print_found_objects(scanned_obj * objectlist) {
    int direction = 0;

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
    int actual = (int)((-6.426e-15 * pow((float)x, 5)) + (5.919e-11 * pow((float)x, 4)) + (-2.175e-7 * pow((float)x, 3)) + (4.075e-4 * pow((float)x, 2)) + (-4.079e-1 * (float)x) + 1.991e2);
    return actual;
}
