/*
 * distancesensor.h
 *
 *  Created on: Sep 20, 2021
 *      Author: ccannon1
 */

#ifndef DISTANCESENSOR_H_
#define DISTANCESENSOR_H_
///defines either exponential or polynomial to order 2
#define IR_TO_DIST(raw) ((float) (double)IR_CONST + (double)IR_COEFF * (double)pow((double)raw,(double)IR_EXP) + ((double)IR_COEFF1 * raw))

#define NSCANS 180

#ifndef CYBOT_SCAN_H_
// Scan value
typedef struct{
    float sound_dist;  // Distance from Ping Sensor
    int IR_raw_val;    // Raw ADC value from IR sensro
} cyBOT_Scan_t;
#endif

typedef struct
{
    int angle;
    float distance;
    int angle2;
    int radial_width;
    float straight_width;
    int scan_direction;
} scanned_obj;

typedef struct {
    //start angle
    int angle;
    float distance;
    char posx;
    char posy;
    float width;


}vect_obj;


///sets initial radial width to 0/ NOT USED!!!!!!!
//#define INIT_SCANNED_OBJ(i) {ANGLE_FRM_SCAN(i),all_scans[i].sound_dist,0}

//funcion defines
void detect_output_objects();
void cyBot_sendString(char *s);
void scan_180();
char scan_360();
void botprintf(char *format, ...);
void output_scan_data();
void print_found_objects();
void     scan_range();
int smallest_object_angle();
scanned_obj* smallest_object_360();
int which_direction(scanned_obj * p);

char refined_scan180(scanned_obj *objarray);
char objsFrmScns(scanned_obj *objarray);
char sonar_dist_scan(scanned_obj * objarray);

void our_scan(cyBOT_Scan_t * scan);


extern cyBOT_Scan_t all_scans[NSCANS];

//Get the angle from an all_scans index
//#define ANGLE_FRM_SCAN(a) (a*180/NSCANS)

//extern scanned_obj all_objects[NSCANS];
extern scanned_obj front_objects[40];
extern scanned_obj back_objects[40];
extern scanned_obj right_objects[40];
extern scanned_obj left_objects[40];



#define FRONT_I 0
#define RIGHT_I 3
#define BACK_I 2
#define LEFT_I 1

#define FRONTDEG 0
#define RIGHTDEG 270
#define BACKDEG 180
#define LEFTDEG 90
extern int obj_list[4];



#endif /* DISTANCESENSOR_H_ */
