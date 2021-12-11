/*
 * scan.h
 *
 *  Created on: Nov 19, 2021
 *      Author: ndgartin
 */

#ifndef SCAN_H_
#define SCAN_H_

#include "sonar.h"
/////////Maximum distance for an object
#define MAX_DISTANCE 50

////holds the object information, everything is used but angle and scan direction
typedef struct
{
    int angle;
    float distance;
    float leftDist;
    float rightDist;
    int angle2;
    int radial_width;
    float straight_width;
    int scan_direction;
} scanned_obj;





///////////i don't remember which one we use. this or MAX_DISTANCE
#define SCANNABLE_DISTANCE 60





#define NFRONTOBJ num_objs_list[0]
///a scan handle
typedef struct scan_handle {
    int angle;
    int  ping;
    int IR;
} scan_handle;

char scan180_alarmbot();
float get_width(float dist1, int angle);
int simpleScan(char deg, scan_handle * scn);
int scan();

int SonarScan(int angle, Ping* ping);
int IRRawScan(int angle);
int IRDistanceScan(int angle);
int GetActualDistance(int x);
void output_scan_data(scanned_obj  objectlist[]);
void print_found_objects(scanned_obj * objectlist);
int test_scans_print_scans();
#endif /* SCAN_H_ */
