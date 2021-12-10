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
#include "traversal.h"
#include "scan.h"
#include "botconfig.h"
#include "gui.h"
extern scanned_obj front_objects[40];
extern int num_objs_list[4];
extern int FieldEdgeFound;

int gui_rotate(oi_t * sensor,int dir)
{
    sendMsg(&COMPLETED_MSG);


    char ch = getChar();

    sendMsg(&START_MSG);

    rotateDegrees(sensor, ((signed short) ch * dir));

    sendMsg(&COMPLETED_MSG);

}
void manual_mode(oi_t *sensor)
{
    int ch = 0;
    scan_handle scn;

    while (ch != 'Q')
    {

        ch = getChar();
        switch (ch)
        {
        case 'w':
            if (GUI)
            {
                sendMsg(&START_MSG);
            }
            int distTraveled;
            unsigned int flags = actually_move_until_detect_obstacle(sensor,
                                                                     50,&distTraveled);
            if (flags || FieldEdgeFound)
            {
                if (GUI) {
                sendBumpMsg(flags,distTraveled);}
                else {
                    botprintf("%X botflags\n\r",flags);
                }
            }
            if (GUI)
            {
                sendMsg(&COMPLETED_MSG);

            }

            break;
        case 's':
            if (GUI)
            {
                sendMsg(&START_MSG);
            }
            move_specific_distance(sensor, -50);
            if (GUI)
            {
                sendMsg(&COMPLETED_MSG);

            }
            break;
        case 'a':
            if (GUI)
            {
                sendMsg(&START_MSG);
            }
            rotateDegrees(sensor, 90);
            if (GUI)
            {
                sendMsg(&COMPLETED_MSG);

            }

            //rotate_90_degrees(sensor);
            break;

        case 'd':
            if (GUI)
            {
                sendMsg(&START_MSG);
            }
            rotateDegrees(sensor, -90);
            if (GUI)
            {
                sendMsg(&COMPLETED_MSG);

            }

            break;
        case 'j':
            if (GUI)
            {
                sendMsg(&START_MSG);
            }
            rotateDegrees(sensor, 10);
            if (GUI)
            {
                sendMsg(&COMPLETED_MSG);

            }

            break;
        case 'k':
            if (GUI)
            {
                sendMsg(&START_MSG);
            }
            rotateDegrees(sensor, -10);
            if (GUI)
            {
                sendMsg(&COMPLETED_MSG);

            }
            break;
        case 'y':
            if (GUI)
            {
                sendMsg(&START_MSG);
            }
            scan180_alarmbot();
            objsFrmScns(front_objects);
            print_found_objects(front_objects);
            print_objs_in_way();

            if (GUI)
            {
                int i;
                for (i = 0; i < num_objs_list[0]; i++)
                {
                    sendScannedObj(&front_objects[i]);
                }
                sendMsg(&COMPLETED_MSG);

            }
            else
            {
                int i;
                for (i = 0; i < num_objs_list[0]; i++)
                {
                    msg_struct msg = buildObjMsg(&front_objects[i]);
                    char *width = &msg;
                    width++;
                    width++;
                    botprintf("%d object straightwidth: %d\n\r", i, *width);
                }
            }
            break;
        case 'W':
            if (GUI)
            {

                sendMsg(&COMPLETED_MSG);
                char dist = getChar();
                sendMsg(&START_MSG);
                int distTraveled;
                unsigned int flags = actually_move_until_detect_obstacle(
                        sensor, (int) dist,&distTraveled);
                if (flags || FieldEdgeFound)
                {
                    sendBumpMsg(flags,distTraveled);
                }

                sendMsg(&COMPLETED_MSG);

            }
            break;
        case 'h':
            ;
            while (1)
            {
                timer_waitMillis(10);
                print_vel();
            }
        case 'S':
            ;
            simpleScan(90, &scn);
            botprintf("IR: %d, PING: %d\n\r", scn.IR, scn.ping);
            break;
        case 'D':
            ;
            simpleScan(0, &scn);
            botprintf("IR: %d, PING: %d\n\r", scn.IR, scn.ping);
            break;
        case 'A':
            ;
            simpleScan(180, &scn);
            botprintf("IR: %d, PING: %d\n\r", scn.IR, scn.ping);

            //signed short data;
            //get_rotation(&data);
            //botprintf("%d\n\r",(int)(data/16));

            break;
        case 'R':
            if (GUI)
            {
                gui_rotate(sensor,1);
            }
            ;
            break;
        case 'T':
            ;
            if (GUI)
            {
                gui_rotate(sensor,-1);
            }
            break;
        default:
            if (GUI)
            {
                sendMsg(&COMPLETED_MSG);

            }
            break;
        }
    }
}
