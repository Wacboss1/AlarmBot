/*
 * manual_mode.c
 *
 *  Created on: Dec 7, 2021
 *      Author: ndgartin
 */

#include "uart.h"
#include "movement.h"
#include "manual_mode.h"
#include "Timer.h"
#include "scan.h"
scanned_obj front_objects[40];

void manual_mode(oi_t * sensor) {
    int ch =0;
    scan_handle  scn;

    while (ch!='Q') {

    ch= getChar();
    switch (ch) {
    case 'w':
        actually_move_until_detect_obstacle(sensor, 50);

        break;
    case 's':
        move_specific_distance(sensor, -50);
        break;
    case 'a':
        rotateDegrees(sensor, 90);

        //rotate_90_degrees(sensor);
        break;
    case 'd':
        rotateDegrees(sensor, -90);

        break;
    case 'j':
        rotateDegrees(sensor, 10);

        break;
    case 'k':
        rotateDegrees(sensor, -10);

        break;
    case 'y':
        scan180_alarmbot();
        objsFrmScns(front_objects);
        print_found_objects(front_objects);
        break;
    case 'h':
        ;
        while (1){
            timer_waitMillis(10);
            print_vel();
        }
    case 'S':
        ;
        simpleScan(90, &scn);
        botprintf("IR: %d, PING: %d\n\r",scn.IR,scn.ping);
        break;
    case 'D':
        ;
        simpleScan(0, &scn);
        botprintf("IR: %d, PING: %d\n\r",scn.IR,scn.ping);
        break;
    case 'A':
        ;
                simpleScan(180, &scn);
                botprintf("IR: %d, PING: %d\n\r",scn.IR,scn.ping);

        //signed short data;
             //get_rotation(&data);
             //botprintf("%d\n\r",(int)(data/16));

        break;


    }
    }
}
