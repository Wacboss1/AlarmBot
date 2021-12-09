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

void manual_mode(oi_t * sensor) {
    int ch =0;
    while (ch!='Q') {

    ch= getChar();
    switch (ch) {
    case 'w':

        break;
    case 's':

        break;
    case 'a':
        rotate_90_degrees(sensor);
        break;
    case 'd':
        turn_robot_degrees(sensor, 90);

        break;
    case 'j':
        turn_robot_degrees(sensor, 10);

        break;
    case 'k':
        turn_robot_degrees(sensor, -10);

        break;
    case 'h':
        ;
        signed short data;
             get_rotation(&data);
             botprintf("%d\n\r",(int)(data/16));

        break;


    }
    }
}
