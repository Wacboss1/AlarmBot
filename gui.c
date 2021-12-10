/*
 * gui.c
 *
 *  Created on: Dec 9, 2021
 *      Author: ndgartin
 */
#include "uart.h"
#include "scan.h"
#include "gui.h"
#include "movement.h"

 const msg_struct START_MSG= {START_GUI,{0,0,0,0,0,0,0}};
const msg_struct COMPLETED_MSG = {COMPLETED_GUI,{0,0,0,0,0,0,0}};

extern int FieldEdgeFound;

int sendMsg(msg_struct * msg) {
    char * curByte = msg;
    int i;
    for (i=0;i<MSGSIZE;i++) {
        uart_sendChar(*curByte);
        curByte++;
    }

}

int sendScannedObj(scanned_obj * obj) {
    obj_msg msg;
    msg.msgType=OBJ_GUI;
    msg.dist=(char)obj->distance;
    msg.radius=(char)(obj->straight_width);
    msg.angle=(char)(obj->angle+(obj->radial_width/2));
    sendMsg((msg_struct*)&msg);
    return 0;

}

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

int sendBumpMsg(unsigned int bumpFlags, int dist) {
    bump_msg msg;
    msg.msgType=BUMP_GUI;
    msg.bumpFlags=(bumpFlags & (HIT_LEFT_BUMPER |HIT_RIGHT_BUMPER ));
    msg.EdgeFlags=((FieldEdgeFound));
    msg.cliffFlags=(bumpFlags & (CLIFFFRONTLEFT | CLIFFLEFT |CLIFFFRONTRIGHT |CLIFFRIGHT ));
    msg.distTraveled =dist;
    sendMsg((msg_struct*)&msg);

}
