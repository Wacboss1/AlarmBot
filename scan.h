/*
 * scan.h
 *
 *  Created on: Nov 19, 2021
 *      Author: ndgartin
 */

#ifndef SCAN_H_
#define SCAN_H_

#include "sonar.h"

#define MAX_DISTANCE 50


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






#define SCANNABLE_DISTANCE 50





#define NFRONTOBJ num_objs_list[0]

typedef struct scan_handle {
    int angle;
    int  ping;
    int IR;
} scan_handle;

char scan180_alarmbot();

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
