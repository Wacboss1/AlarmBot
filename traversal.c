/*
 * traversal.c
 *
 *  Created on: Nov 30, 2021
 *      Author: ndgartin
 */

/*
 * movement.c
 *
 *  Created on: Nov 29, 2021
 *      Author: ndgartin
 */


#include "movement.h"
#include "button.h"
#include "uart.h"
#include "scan.h"
#include "traversal.h"
#include "math.h"

#define DEG_TO_RAD(a) a*(M_PI*2/360)
#define RAD_TO_DEG(a) a*(360)/(M_PI*2)

//int lowestCliffX,lowestCliffY,

//#include "distancesensor.h"



extern scanned_obj front_objects[40];
extern scan_handle all_scans[180];
extern int num_objs_list[4];

extern int FieldEdgeFound;

int test_object_shit(oi_t * sensor){
   // int scn = scan();
    //if (scn>-1){
    //    lcd_printf("Found object %d",scn);
   // }
   // else if(scn==0) {

//    }
    go_around_big_obstacles(sensor);

    while (button_getButton()==0) {

    }

    return 0;

}




int enter_endpoint(int deg, float dist)
{

    return 0;
}

enum Cliff_status
{
    NO_CLIFF, INCREMENT_CLIFF, DECREMENT_CLIFF, DONE
};
#define CLIFF_HIT 1
#define EDGE_HIT 2

///returns 1 if object is in the way
int obj_in_way(scanned_obj * obj) {
    int boundaryLeft;
    int boundaryRight;
    if ((obj->angle > 60 && obj->angle < 120)  || (obj->angle2 > 60 && obj->angle2 < 120) ){
        return 1;
    }
    else return 0;

}


//returns -1 if clear
///returns -2 if found endpoint
///returns index of object if we need to movearound object
//movearound an object
#define CLEAR_SPACE -1
#define ENDPOINT_FOUND -2
int scan()
{

    //scan_handle scn;
    scan180_alarmbot();

    ///process objects
    objsFrmScns(front_objects);
    print_found_objects(front_objects);
    int i;

    ///if no objects detected return 1
    if (num_objs_list[0]>0) {
        return 1;
    }
    for (i=0; i < (num_objs_list[0]); i++) {
        if (obj_in_way(&front_objects[i])) {
            return (i);
        }

    }


    return 0;
}



int turnRight90(oi_t *sensor, int * cur) {
    if (*cur==3){
        *cur=0;
    }
    else {
        *cur++;
    }
    turn_robot_degrees(sensor, -90);
}
int turnLeft90(oi_t *sensor, int * cur) {
    if (*cur==0){
        *cur=3;
    }
    else {
        *cur--;
    }
    turn_robot_degrees(sensor, 90);
}

int faceMiddle(oi_t * sensor, int * cur, int * gridX) {
    int turnD;
    ///face left
    if (*gridX >0) {
        switch (*cur) {
        case FORWARD_OA:
            turnD=90;
            break;
        case RIGHT_OA:
            turnD=-180;
            break;
        case LEFT_OA:
            turnD=0;
            break;
        case BACK_OA:
            turnD=-90;
                break;
        default:
            break;
        }
        *cur=LEFT_OA;
    }
    else if (*gridX < 0) {
        //face right
        switch (*cur) {
           case FORWARD_OA:
               turnD=-90;
               break;
           case RIGHT_OA:
               turnD=180;
               break;
           case LEFT_OA:
               turnD=0;
               break;
           case BACK_OA:
               turnD=90;
                   break;
           default:
               break;
           }
        *cur=RIGHT_OA;
    }
    turn_robot_degrees(sensor, turnD);
    return 0;

}

int move_forward_OA(oi_t *sensor, int * cur, int * gridX, int * gridY) {
    switch (*cur)  {
    case FORWARD_OA:
        *gridY+=CELLSIZE_OA;
        break;
    case RIGHT_OA:
         *gridX+=CELLSIZE_OA;
         break;
    case BACK_OA:
            *gridY-=CELLSIZE_OA;
            break;
    case LEFT_OA:
            *gridX-=CELLSIZE_OA;
    }
    actually_move_until_detect_obstacle(sensor, CELLSIZE_OA);
    return 0;
}

int go_around_big_obstacles(oi_t *sensor) {

    int gridX=0;
    int gridY=0;
    int directionfacing=0; //0forward,1right,2back,3left
    int scanresult=0;
    do {
        if (gridX==0) {
        //turn to face the right
        turnRight90(sensor, &directionfacing);
        ///scan area to the right
        scanresult=scan();
        if (scanresult==-1) {
        move_forward_OA(sensor, &directionfacing, &gridX, &gridY);
        }
        }
        else if (gridY==0) {
            turnLeft90(sensor, &directionfacing);
            ///scan area to the right
            if (directionfacing!=BACK_OA)
            scanresult=scan();
            else {
                scanresult=-1;
            }
            if (scanresult==-1) {
            move_forward_OA(sensor, &directionfacing, &gridX, &gridY);
            }
        }
        ////After we've turned right, gone forward, and then turned left and gone forward
        //// go right a little more to make sure we are passed the obstacle
        //// this could hit an object, because not scanning but for now just taking that risk
        else if (gridY==CELLSIZE_OA) {
            int incvalue=20;
            gridY+=incvalue;
            actually_move_until_detect_obstacle(sensor, incvalue);

        }
        ///if we get here that means we just need to turn back to the
        else if (gridX!=0) {
            faceMiddle(sensor, &directionfacing,&gridX);
        }
    }
    while (gridX!=0 && gridY<CELLSIZE_OA);

}


int move()
{

}

/*
 * TODO:
 *  SCAN()
 *   FIND TARGET
 *
 *  MOVE()
 *   OBSTACLE AVOIDANCE()
 *
 *  TURN()
 *
 */
#define HYP(a,b) pow((pow(a,2)+pow(b,2)),.5)
int move_around_obj(int objInd, oi_t * sensor) {
    scanned_obj * obj= &front_objects[objInd];
    double radiansToTurn= tanh((((double)obj->straight_width/2)+20 )/(double)obj->distance);
    rotate_degrees(sensor,RAD_TO_DEG(radiansToTurn));
    double distanceToGo=(double)(HYP(obj->straight_width/2,obj->distance));

    unsigned int moveStopped= actually_move_until_detect_obstacle(sensor, distanceToGo);
    ///if we detect anything, we backtrack and try the left side
    if (FieldEdgeFound || moveStopped) {
        move_specific_distance(sensor, -(moveStopped>>8));
    }


    return 0;
}


///after we get to the middle
int find_endpoint(oi_t * sensor)
{
    ///scan
    ///
    scanned_obj inTheWay[3];
    int scn=scan(inTheWay);
    if (scn==CLEAR_SPACE) {
        actually_move_until_detect_obstacle(sensor, 56);
    }
    else if (-2) {
        ///found the endpoint
    }
    else {
        //for now go straight after moving around the object, but it would be better to move around it
        move_around_obj(scn,sensor);
    }






}
    /*
    ///starts from corner
    int current_deg = 0;
    int target_found = 0;
    int lastEdge = 0;
    int hitFlag = 0;
    int cliffHits = 0;
    int maxCliffHits = 0;
    enum Cliff_status status = NO_CLIFF;
    while (!target_found)
    {
        while (hitFlag == 0)
        {
            scan();
            hitFlag = move();
        }
        if (hitFlag == CLIFF_HIT)
        {
            if (status == NO_CLIFF)
            {
                status = INCREMENT_CLIFF;
            }
            if (status == INCREMENT_CLIFF)
            {
                cliffHits++;
            }
            else if (status ==DECREMENT_CLIFF)
            {
                cliffHits--;
                if (cliffHits == 0)
                {
                    status = DONE;
                }
            }
            lastEdge = CLIFF_HIT;
        }
        else if (hitFlag == EDGE_HIT)
        {
            if (lastEdge == EDGE_HIT)
            {
                if (status == INCREMENT_CLIFF)
                {
                    maxCliffHits = cliffHits;
                    status = DECREMENT_CLIFF;
                }
            }
            lastEdge = EDGE_HIT;
        }
        //Turn, scan(), if (DONE) { longforward based on maxcliffnum*2 and short forward regular dist } short_forward, turn
        int nully;
        turnRight90(sensor,&nully);
        scan();
        if (status == DONE){
            int i;
            for (i = 0; i < maxCliffHits * 2; i++){
                int scanR;
                scanR=scan();
                if (scanR>-1) {
                go_around_big_obstacles(sensor);
                }
                move_around_obstacles(sensor, SCANNABLE_DISTANCE);

            }
        }
    }
}*/

/*
 FLAGS:
 Cliff_status
 {
 NO_CLIFF
 INCREMENT_CLIFF
 DECREMENT_CLIFF
 DONE
 }
 cliff_hits last_edge_was_border
 FUNCTION Scan_field(direction)
 {
 while (!target_found)
 {
 while (!cliff && !border)
 {
 scan()
 move
 forward (around obs & detecting cliffs/borders)
 }
 if (cliff)
 {
 if (NO_CLIFF)
 {
 Cliff_status = INCREMENT
 }
 if (INCREMENT)
 {
 increment cliff_hits
 }
 else if (DECREMENT)
 {
 decrement cliff_hits
 if (cliff_hits == 0)
 {
 cliff_status = DONE;
 }
 }
 last_edge_was_border = false;
 }
 else
 {
 if (last_edge_was_border)
 {
 if (cliff_status == INCREMENT)
 {
 global max_cliff_num = cliff_hits
 cliff_status = DECREMENT;
 }
 }
 last_edge_was_border = true;
 }
 turn
 scan()
 if (cliff_status == DONE)
 {
 long forward
 based on(max_cliff_num * 2)
 && short forward regular dist
 short forward
 turn
 }
 }
 }
 */
