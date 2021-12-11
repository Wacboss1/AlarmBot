/*
 * gui.h
 *
 *  Created on: Dec 9, 2021
 *      Author: ndgartin
 */

#ifndef GUI_H_
#define GUI_H_
#include "scan.h"

/*
 * Message types: COMPLETED, START, OBJ, BUMP
 */
#define MSGSIZE 8
#define COMPLETED_GUI 0
#define START_GUI 1
#define OBJ_GUI 2
#define BUMP_GUI 3


/*
 * message base structure
 */

typedef struct msg_struct {
    char msgType;
    char buffer[7];

}msg_struct;

/*
 * object message structure
 */
typedef struct obj_msg {
    char msgType;
    char angle;
    char radius;
    char dist;

}obj_msg;
/*
 * bump message structure
 */
typedef struct bump_msg {
    char msgType;
    char bumpFlags;
    char cliffFlags;
    char EdgeFlags;
    char distTraveled;
}bump_msg;
/*
 * prebuilt messages
 */
extern const msg_struct START_MSG; //= {START_GUI,{0,0,0,0,0,0,0}};
extern const msg_struct COMPLETED_MSG;// = {COMPLETED_GUI,{0,0,0,0,0,0,0}};

////Send a message
int sendMsg(msg_struct * msg);
////Send a scanned object
int sendScannedObj(scanned_obj * obj);
/////send a bump message
int sendBumpMsg(unsigned int bumpFlags, int dist);
//// build a message from a scanned object, but do not send
msg_struct buildObjMsg(scanned_obj * obj);


#endif /* GUI_H_ */
