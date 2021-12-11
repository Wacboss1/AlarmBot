/*
 * gui.c
 * GUI sends one byte at a time with the UART. on the software level, each message is 8 bytes long. The first byte is the message type
 *  Created on: Dec 9, 2021
 *      Author: ndgartin
 */
#include "uart.h"
#include "scan.h"
#include "gui.h"
#include "movement.h"
/*
 * Start message is sent to gui to say that we started executing a command
 * completed message is sent at the end of the command execution, or if the command is unrecognized
 */
 const msg_struct START_MSG= {START_GUI,{0,0,0,0,0,0,0}};
const msg_struct COMPLETED_MSG = {COMPLETED_GUI,{0,0,0,0,0,0,0}};

extern int FieldEdgeFound;


///// Send a message
/// over uart
int sendMsg(msg_struct * msg) {
    char * curByte = msg;
    int i;
    for (i=0;i<MSGSIZE;i++) {
        uart_sendChar(*curByte);
        curByte++;
    }

}
/*
 * Processes a scanned object into a message and sends it
 * sends distance, angle, and radius
 */
int sendScannedObj(scanned_obj * obj) {
    obj_msg msg;
    msg.msgType=OBJ_GUI;
    msg.dist=(char)obj->distance;
    msg.radius=(char)(obj->straight_width);
    msg.angle=(char)(obj->angle+(obj->radial_width/2));
    sendMsg((msg_struct*)&msg);
    return 0;

}
/*
 * testing function to check that memory is read correctly
 */

msg_struct buildObjMsg(scanned_obj * obj) {
    obj_msg msg;
    msg.msgType=OBJ_GUI;
    msg.dist=(char)obj->distance;
    msg.radius=(char)(obj->straight_width);
    msg.angle=(char)(obj->angle+(obj->radial_width/2));
    //sendMsg((msg_struct*)&msg);
    msg_struct rawmsg;
    int i;
    char * ch=&msg;
    char * otherchar = &rawmsg;
    for ( i =0;i<8;i++) {
        *otherchar=*ch;
        otherchar++;
        ch++;
    }
    return rawmsg;

}
/*
 *sends bump flags message, contains Bump, cliff, edge , and also distance traveled before bot hit edge
 */
int sendBumpMsg(unsigned int bumpFlags, int dist) {
    bump_msg msg;
    msg.msgType=BUMP_GUI;
    msg.bumpFlags=(bumpFlags & (HIT_LEFT_BUMPER |HIT_RIGHT_BUMPER ));
    msg.EdgeFlags=((FieldEdgeFound));
    msg.cliffFlags=(bumpFlags & (CLIFFFRONTLEFT | CLIFFLEFT |CLIFFFRONTRIGHT |CLIFFRIGHT ));
    msg.distTraveled =dist;
    sendMsg((msg_struct*)&msg);

}
