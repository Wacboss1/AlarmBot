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

////defined in scan.c
extern scanned_obj front_objects[40];

///defined in scan.c
extern int num_objs_list[4];

///defined in movement.c
extern int FieldEdgeFound;
//defined in uart.c
extern int GUI;

/*
 * get a custom rotation from the gui and rotate that amount, rotation is a char
 */
int gui_rotate(oi_t * sensor,int dir)
{
    sendMsg(&COMPLETED_MSG);


    char ch = getChar();

    sendMsg(&START_MSG);

    rotateDegrees(sensor, ((signed short) ch * dir));

    sendMsg(&COMPLETED_MSG);

}
/*
 * the main loop for our manual mode
 *  Can be used with putty or with GUI. To switch between GUI and putty send ';'
 *  some commands are not used with GUI, some are only for GUI
 *
 */
void manual_mode(oi_t *sensor)
{
    int ch = 0;
    scan_handle scn;

    while (ch != 'Q') /// press Q to go to the main.c command loop
    {

        ch = getChar();
        switch (ch)
        {
        case 'w': ///move forward 50 cm
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
        case 's': /// move backward 50 cm
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
        case 'a': ///rotate 90 degrees to the left
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

        case 'd': ///rotate 90 degrees to the right
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
        case 'j': ///rotate left 10 degrees
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
        case 'k': ///rotate right 10 degrees
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
        case 'y': ///scan 180 degrees, print scans and objects if in putty, in gui, sends object packets
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
        case 'W': ///move forward custom amount. GUI only
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
        case 'B': ///move backwards custom amount. GUI only
                    if (GUI)
                    {

                        sendMsg(&COMPLETED_MSG);
                        int dist = -((int)getChar());
                        sendMsg(&START_MSG);
                        int distTraveled;
                        move_specific_distance(sensor, dist);
                        if (flags || FieldEdgeFound)
                        {
                            sendBumpMsg(flags,distTraveled);
                        }

                        sendMsg(&COMPLETED_MSG);

                    }
                    break;
        case 'h': ///prints the linear acceleration every 10 milliseconds
            ;
            while (1)
            {
                timer_waitMillis(10);
                print_vel();
            }
        case 'S': ///scans front of bot and prints sensor values
            ;
            simpleScan(90, &scn);
            botprintf("IR: %d, PING: %d\n\r", scn.IR, scn.ping);
            break;
        case 'D':///scans right of bot and prints sensor values
            ;
            simpleScan(0, &scn);
            botprintf("IR: %d, PING: %d\n\r", scn.IR, scn.ping);
            break;
        case 'A': ///scans left of bot and prints sensor values
            ;
            simpleScan(180, &scn);
            botprintf("IR: %d, PING: %d\n\r", scn.IR, scn.ping);

            //signed short data;
            //get_rotation(&data);
            //botprintf("%d\n\r",(int)(data/16));

            break;
        case 'R': //rotate custom amount. to the left
            if (GUI)
            {
                gui_rotate(sensor,1);
            }
            ;
            break;
        case 'T': ///rotates custom amount to the right
            ;
            if (GUI)
            {
                gui_rotate(sensor,-1);
            }
            break;
        default:
            if (GUI)   ///always send a completed_msg if the gui message is not understood to keep gui from hanging
            {
                sendMsg(&COMPLETED_MSG);

            }
            break;
        case ';': ///toggle GUI mode
                          GUI ^= 1;
                          if (GUI) {
                              sendMsg(&COMPLETED_MSG);

                          }
                         break;
        }
    }
}
