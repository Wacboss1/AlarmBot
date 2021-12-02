/*
 * distancesensor.c
 *
 *  Created on: Sep 20, 2021
 *      Author: ccannon1
 */

extern volatile char data_received_flag;
extern volatile char data_received;
#include "Timer.h"
#include "lcd.h"
#include "uart.h"
//#include "cyBot_Scan.h"
#include "open_interface.h"
#include "movement.h"
#include "distancesensor.h"
#include "botconfig.h"
#include "stdarg.h"
//#include "calibration.h"
#include "servo.h"
#include "sonar.h"


//#define convert_raw_ir_to_distance(raw) ((double) CYBOT_BASE * (double) pow((double) raw, (double) CYBOT_EXP))
#define OBJECT_DISTANCE_OFFSET 2
#define MAX_DIST 50
#define SONAR_SCAN_ANGLE 5
#define ANGLE_FRM_SCAN(a) (a)
const char * SCND_DIR_STR [] ={ "FRONT",  "LEFT", "BACK", "RIGHT"};

#define _SCAN(d,s) scan_deg(d, s)


void our_scan(cyBOT_Scan_t * scan) {

 /*   int d = adc_read();
        scan->sound_dist=better_ping_read();
        botprintf("sound scan initial value: %lf",scan->sound_dist);
        scan->IR_raw_val=d;*/
}


//very confusing defines
//if DEBUGTHIS is true it will print the scans for copying
/// if it is defined than it will include the testdata.h which contains the sample scans copied from previous debugthis trues
// if USEDEBUG DATA is true than it will use the sample data instead of making new scans
#define DEBUGTHIS 0
#ifdef DEBUGTHIS
//#include "testdata.h"
#endif

//NSCANS is the size of all_scans, ACTUALNSCANS is the actual number of scans done of that 180
//todo: get rid of nscans and replace with 180
//#define ACTUALNSCANS 40
//#define DEBUG_THIS_FILE
#define WITHINRANGE(a,b) (a > b-OBJECT_DISTANCE_OFFSET && a < b+OBJECT_DISTANCE_OFFSET)

void scan_deg(unsigned int deg, cyBOT_Scan_t * scan) {
    if (turn_servo_deg((char)deg)){
        botprintf("illegal turn of the servo\n\r");
    }
    our_scan(scan);


}
void CyBOT_sendStr(char *s)
{
    if (PUTTY)
    {
        char *c = s;
        while (*c != 0)
        {
            uart_sendChar(*c);
            c++;
        }
    }
    else
        uart_sendStr(s);
}



cyBOT_Scan_t all_scans[180];

//Get the angle from an all_scans index
//#define ANGLE_FRM_SCAN(a) (a*180/NSCANS)

//scanned_obj all_objects[NSCANS];
scanned_obj front_objects[40];
scanned_obj back_objects[40];
scanned_obj right_objects[40];
scanned_obj left_objects[40];

int num_objs_list[4];

/*
 * Logic for a scan:
 * Scan at 1 degree resolution
 * Search sonar values wide angles first, to generate intervals for infrared analysis.
 *      In each interval:
 *          - Log distance
 *          - parse IR data to determine where objects are.
 *              - Search for first angle, or assume first angle is sonar?
 *                  Search, log first angle
 *              - While val is within a range, increment,
 *                  once out, log last angle
 *
 * check all_scans[i].sound_dist after each scan.
 * if it is within MAX_DIST range, call a function to g
 */
float get_width(float dist1, int angle)
{
    float new_angle = angle * 3.14159 / 180;
    return 2 * dist1 * sin((double) (new_angle) / 2);
}

int wait_for_ms(int ms)
{
    int start = timer_getMillis();
    while (timer_getMillis() - start < ms)
    {
        printf("%d", timer_getMillis() - start);
        continue;
    }
    return 0;

}
//returns object with smallest width
int smallest_object_angle(scanned_obj *objarray)
{
    int direction = which_direction(objarray);

    int i = 0;
    float min_width = 99999;
    int min_width_ind = -1;
    for (i = 0; i < num_objs_list[direction]; i++)
    {
        if (objarray[i].straight_width < min_width)
        {
            min_width = objarray[i].straight_width;
            min_width_ind = i;
        }
    }
    return min_width_ind;
}

scanned_obj* smallest_object_360()
{
    int smallestangles[4];

    smallestangles[FRONT_I] = smallest_object_angle(front_objects);
    smallestangles[LEFT_I] = smallest_object_angle(left_objects);
    smallestangles[RIGHT_I] = smallest_object_angle(right_objects);
    smallestangles[BACK_I] = smallest_object_angle(back_objects);

    scanned_obj *all_smallest[4];

    all_smallest[FRONT_I] = &front_objects[smallestangles[FRONT_I]];
    all_smallest[LEFT_I] = &left_objects[smallestangles[LEFT_I]];
    all_smallest[RIGHT_I] = &right_objects[smallestangles[RIGHT_I]];
    all_smallest[BACK_I] = &back_objects[smallestangles[BACK_I]];

    int i = 0;
int smallestDirection=-1;
    scanned_obj *smallestObj = all_smallest[0];

    for (i = 0; i < 4; i++)
    {
        if (all_smallest[i]->straight_width < smallestObj->straight_width && (all_smallest[i]!= -1))
        {
            smallestObj = all_smallest[i];
            smallestDirection = i;
        }
    }
    if (VERY_VERBOSE) {
        botprintf("Smallest object is from %s scan list.  dist: %f, radial width: %d \n\r", SCND_DIR_STR[smallestDirection],smallestObj->distance,smallestObj->radial_width);
    }
    return smallestObj;
}

int which_direction(scanned_obj *p)
{
    if (p == front_objects)
        return FRONT_I;
    else if (p == right_objects)
        return RIGHT_I;
    else if (p == back_objects)
        return BACK_I;
    else if (p == left_objects)
        return LEFT_I;

}

char switch_autonomous()
{
    if (data_received_flag && data_received == 't')
    {
        return 1;
    }
    return -1;
}
char scan_360(oi_t *sensor)
{
    if (data_received_flag && data_received == 't')
    {
        return 0;
    }
    else
    {
        //scan_range(&front_objects);
        refined_scan180(front_objects);
        objsFrmScns(front_objects);
        sonar_dist_scan(front_objects);
        print_found_objects(&front_objects);
        turn_robot_degrees(sensor, -90);
    }
    if (data_received_flag && data_received == 't')
    {
        return 0;
    }
    else
    {
        //  scan_range(&right_objects);
        refined_scan180(right_objects);
        objsFrmScns(right_objects);
        sonar_dist_scan(right_objects);
        print_found_objects(&right_objects);
        turn_robot_degrees(sensor, -90);

    }
    if (data_received_flag && data_received == 't')
    {
        return 0;
    }
    else
    {
        // scan_range(&back_objects);
        refined_scan180(back_objects);
        objsFrmScns(back_objects);
        sonar_dist_scan(back_objects);
        print_found_objects(&back_objects);
        turn_robot_degrees(sensor, -90);
    }
    if (data_received_flag && data_received == 't')
    {
        return 0;
    }
    else
    {
        //  scan_range(&left_objects);
        refined_scan180(left_objects);
        objsFrmScns(left_objects);
        sonar_dist_scan(left_objects);
        print_found_objects(&left_objects);
        //print_found_objects(&left_objects);
        turn_robot_degrees(sensor, -90);
    }
    botprintf("Front objects: \n\r");
    print_found_objects(&front_objects);

    botprintf("Right objects: \n\r");
    print_found_objects(&right_objects);

    botprintf("Back objects: \n\r");
    print_found_objects(&back_objects);

    botprintf("Left objects: \n\r");
    print_found_objects(&left_objects);

    return 1;
}

char clear_scans()
{
    int i = 0;
    int max = sizeof(all_scans) / sizeof(cyBOT_Scan_t);
    for (i = 0; i < max; i++)
    {
        all_scans[i].IR_raw_val = 0;
        all_scans[i].sound_dist = 0;
    }
    return 0;
}

/*
 * making new scan range not done, probably suck
 * features nscans as constant variable not define, for easy testing, and will have
 * two different scan sessions, first to detect objects and second to increment to the left and then right of the scan where we found object
 * resulting in all_scans having degrees that weren't scanned, which means we have to rely on clear_scans
 * it should be faster because less degrees scanned
 * Manual recomendation:
 * 1. Scan some degrees of  0-180
 * 2. build objects from the scans
 * 3. point the sonar sensor and get the distance.
 * which is what we already do just not in that order
 */

//decide whichIR scan to use here
#define USE_ADC_IR 1


char refined_scan180(scanned_obj *objarray)
{

    unsigned char nscans = 40;
    int deg_multiplier = 180 / nscans;
    int i = 0;
    int direction = which_direction(objarray);
    num_objs_list[direction] = 0;

    if (!USE_DEBUG_DATA)
    {
        // ========Do the whole first scan===========
        int num_samples = 3;
        float averaged_ir_value = 0;
        cyBOT_Scan_t temp;
        for (i = 0; i < 180; i += deg_multiplier)
        {
            averaged_ir_value = 0;
            int j = 0;
            for (j = 0; j < num_samples; j++)
            {
                ///if using our adc, only use the cybotscan once to aim the servo
                if ( (!USE_ADC_IR) || j==0) {
                _SCAN(ANGLE_FRM_SCAN(i), &temp);
                }
                if (USE_ADC_IR) {
                    //use our scan
                    our_scan(&temp);
                }

                averaged_ir_value += temp.IR_raw_val;

                if (VERY_VERBOSE)
                {
                    botprintf("Angle %d, Raw value: %d, DistVal: %f\n\r", i,
                              temp.IR_raw_val, IR_TO_DIST(temp.IR_raw_val));
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
            all_scans[i].IR_raw_val = (int) averaged_ir_value;
            all_scans[i].sound_dist = temp.sound_dist;
        }
    }
    output_scan_data();
    return 0;
}
char objsFrmScns(scanned_obj *objarray)
{
    //these and those in the above function should be in a struct or something
    unsigned char nscans = 40;
    int deg_multiplier = 180 / nscans;
    int i = 0;
    int direction = which_direction(objarray);

    // =======Find edges of objects=======
    for (i = 0; i < 180; i += deg_multiplier)
    {
        int first_angle;
        int second_angle;

        cyBOT_Scan_t current_scan = all_scans[i];

        if ((IR_TO_DIST(current_scan.IR_raw_val)) < 50)
        {
            first_angle = i;
            while ((IR_TO_DIST(current_scan.IR_raw_val)) < 50)
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

            // These two will be updated on our second scan with the PING sensor's distance;
            objarray[num_objs_list[direction]].distance = IR_TO_DIST(
                    all_scans[second_angle].IR_raw_val);
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
                    CyBOT_sendStr("(starting angle is zero)\n\r");
                if (second_angle >= 180 - deg_multiplier)
                    botprintf("2nd angle above the limit");

            }
            if (VERY_VERBOSE)
            {
                if (objarray[num_objs_list[direction]].distance != 0
                        && num_objs_list[direction] == 0)
                {
                    CyBOT_sendStr("Detected secret shit! Dumping...\n\r");
                    int true_num_obj = num_objs_list[direction];
                    num_objs_list[direction] = 5;
                    print_found_objects(objarray);
                    CyBOT_sendStr("*Flush*\n\r");
                    num_objs_list[direction] = true_num_obj;

                }
            }

        }
    }

    return 0;

}

char sonar_dist_scan(scanned_obj *objarray)
{

    int direction = which_direction(objarray);

    // =======Do second scan, pointing ping sensor at center of objects========
    int k = 0;
    for (k = 0; k < num_objs_list[direction]; k++)
    {
        scanned_obj *current_object = &objarray[k];
        cyBOT_Scan_t current_scan;

        int angle_to_scan = (current_object->angle + current_object->angle2) / 2;
        _SCAN(angle_to_scan, &current_scan);

        current_object->distance = current_scan.sound_dist;
        current_object->straight_width = get_width(
                current_object->distance,
                current_object->angle2 - current_object->angle);
        timer_waitMicros(300);
    }
    return 0;

}

///Runs scans, stores to all_scans
void output_scan_data()
{
    CyBOT_sendStr(
            "Degrees    Distance (cm)    IR (voltage?)    IR to Distance Formula\n");
    int i = 0;
    for (i = 0; i < 180; i++)
    {
        if (all_scans[i].IR_raw_val)
        {
            char toSend[200];
            if (VERBOSE)
            {
                sprintf(toSend, "%d      %lf      %d    %f\n\r",
                        ANGLE_FRM_SCAN(i), all_scans[i].sound_dist,
                        all_scans[i].IR_raw_val,
                        IR_TO_DIST(all_scans[i].IR_raw_val));
            }
            else
            {

                sprintf(toSend, "%d      %lf      %d\n\r", ANGLE_FRM_SCAN(i),
                        all_scans[i].sound_dist, all_scans[i].IR_raw_val);
            }
            CyBOT_sendStr(toSend);
        }
    }
    if (DEBUGTHIS)
        for (i = 0; i < 180; i++)
        {
            char toSend[100];
            sprintf(toSend, "{%lf,  %d},", all_scans[i].sound_dist,
                    all_scans[i].IR_raw_val);

            CyBOT_sendStr(toSend);
        }
    CyBOT_sendStr("\n\r");
}

void print_found_objects(scanned_obj *objarray)
{
    int direction = which_direction(objarray);

    CyBOT_sendStr(
            "Object#     Angle        ANGLE2     Distance     RadialWidth (degrees)     Width (cm)\n\r");
    int j = 0;
    if (PUTTY)
    {
        for (j = 0; j < num_objs_list[direction]; j++)
        {
            char toSend[100];
            sprintf(toSend,
                    "%d           %d        %d          %lf          %d     %lf\n\r",
                    j, objarray[j].angle, objarray[j].angle2,
                    objarray[j].distance, objarray[j].radial_width,
                    objarray[j].straight_width);
            CyBOT_sendStr(toSend);
        }
    }
    else
    {
        for (j = 0; j < num_objs_list[direction]; j++)
        {
            char toSend[100];
            sprintf(toSend, "%d, %d, %d, %lf, %d,%lf\n", j, objarray[j].angle,
                    objarray[j].angle2, objarray[j].distance,
                    objarray[j].radial_width, objarray[j].straight_width);
            CyBOT_sendStr(toSend);
        }
        CyBOT_sendStr("ENDOFOBJ");
    }

}

