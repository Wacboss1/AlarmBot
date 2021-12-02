/*
 * movement.c
 *
 *  Created on: Sep 7, 2021
 *      Author: ndgartin
 */

#include "movement.h"

int current_deg = 0;

static double POWER_OFFSET = 1;
extern volatile char data_received;
extern volatile char data_received_flag;
void move_around_obstacle(oi_t *sensor, char leftBumper, char rightBumper);

//Added defines for turnrobot_degrees function
//Currently multiplyes by POWER_OFFSET
///Speed of right wheel while it is turning
#define TURNING_FWD_SPD 150
//speed of left wheel
#define TURNING_BKWD_SPD (-150 * POWER_OFFSET)


#define CLIFFFRONTLEFT 0x1
#define CLIFFLEFT 0x2
#define CLIFFFRONTRIGHT 0x4
#define CLIFFRIGHT 0x8
#define FRONT_CLIFF_HIT (CLIFFFRONTLEFT || CLIFFFRONTRIGHT)
#define BOTHRIGHT 0xD
#define BOTHFRONT 0x5
#define BOTHLEFT 0x3


int detect_cliff(oi_t * sensor) {
    int return_value=0;
    if (sensor->cliffLeft) {
           return_value|=CLIFFLEFT;
    }
    if (sensor->cliffFrontLeft) {
        return_value|=CLIFFFRONTLEFT;

    }
     if (sensor->cliffRight) {
         return_value|=CLIFFRIGHT;

    }
    else if (sensor->cliffFrontRight) {
        return_value|=CLIFFFRONTRIGHT;

    }
    return return_value;

}

void turn_robot_degrees(oi_t *sensor, int degrees_to_turn)
{

    current_deg += degrees_to_turn;

    //ofset degrees
    int degrees_offset = 1;

    //Use angle member of oi_t struct
    //Init vars
    lcd_init();
    timer_init();

    double angle_turned = 0;

    //two cases, one for turning each direction.
    //even if the angle to turn is negative, we add the sensor's angle field to the total, because it should also be negative(?)

    //Need to turn right
    if (degrees_to_turn > 0)
    {
        oi_setWheels(TURNING_FWD_SPD, TURNING_BKWD_SPD);
        while (angle_turned <( degrees_to_turn - degrees_offset))
        {
            oi_update(sensor);
            angle_turned += sensor->angle;
            if (data_received_flag && data_received == 't')
            {
                break;
            }
        }
    }
    ///Need to turn left
    else
    {
        oi_setWheels(TURNING_BKWD_SPD, TURNING_FWD_SPD);
        while (angle_turned > (degrees_to_turn + degrees_offset))
        {
            oi_update(sensor);
            angle_turned += sensor->angle;
            if (data_received_flag && data_received == 't')
            {
                break;
            }
        }
    }
    oi_setWheels(0, 0);

}
void FindStartPostition(oi_t* sensor)
{
    //Poke around to find an nearby edge of the playing field
    int i;
    for(i = 0; i < 4; i++){
        actually_move_until_detect_obstacle(sensor, 20);
        actually_move_until_detect_obstacle(sensor, -20);
        if(FieldEdgeFound)
        {
            break;
        }
        turn_robot_degrees(sensor, 90);
    }
    //orient self to move along edge
    //TODO add if statement for if an edge is found after looking around
    move_specific_distance(sensor, -10);
    turn_robot_degrees(sensor, -90);
    FieldEdgeFound = 0;
    //move along the edge until a corner is found
    while(!FieldEdgeFound)
    {
        actually_move_until_detect_obstacle(sensor, 30);
    }
    //orient self to being searching for goal
    move_specific_distance(sensor, -10);
    turn_robot_degrees(sensor, -90);
}

int StopOnLine(oi_t* sensor)
{
    if(sensor->cliffFrontLeftSignal >= 2700 ||
       sensor->cliffFrontRightSignal >= 2700)
    {
            oi_setWheels(0,0);
            FieldEdgeFound = 1;
            return 1;
    }
    return 0;
}

//returns y displacement from goal
unsigned int on_detect_cliff(oi_t * sensor,  move_data * bot_move_data, int which_cliff){
    if (which_cliff==0) {
        return 0;
    }
    botprintf("which_cliff: %d\n\r", which_cliff);
    oi_setWheels(0, 0);
    return 0;
}


///returns y displacement from goal
int move_around_cliff (oi_t * sensor, move_data * bot_move_data, int which_cliff) {
    int how_far_to_back_off_from_cliff = 7;
    if (data_received_flag && data_received == 't')
    {
        return;
    }
    //backwards (15 cm)
    int RIGHT = 90;
    int LEFT = -90;



       ////////MAKE SURE THIS IS MOVING IN NEGATIVE DIRECTION!!!!!!!!!!!!!!!
       move_specific_distance(sensor, -7);
       //rotate(90 * direction)

       ///if CLIFFLEFT only
           ///move 5ish cm to the right
       ///same for Right only

       ///if both left ones, move right a little more
       //same for right

      ///if both front ones
           //move 90 degrees and move 50 cm to the right/ move around it
      //if just one front sesnor
       //turn 30ish degrees
           //and then move forward for 50ish cm and then go back to the right line


/*
 * #define CLIFFFRONTLEFT 0x1
#define CLIFFLEFT 0x2
#define CLIFFFRONTRIGHT 0x4
#define CLIFFRIGHT 0x8
#define FRONT_CLIFF_HIT (CLIFFFRONTLEFT || CLIFFFRONTRIGHT)
 */

       //imagine the angle that the bot hits the cliff. we won't need to turn 90 degrees when it only barely skids the cliff, just back up and adjustturn about 15 degrees

       int how_far_rotate;

       switch (which_cliff) {
       case BOTHFRONT:
           how_far_rotate = -90;
           break;
       case CLIFFRIGHT:
           how_far_rotate = 15;
           break;
       case CLIFFLEFT:
           how_far_rotate = -15;

           break;
       case BOTHLEFT:
            how_far_rotate = -45;
                       break;
       case BOTHRIGHT:
           how_far_rotate = 45;
           break;
       default:
           how_far_rotate=-90;
           break;


       }

    //   turn_robot_degrees(sensor, RIGHT * rightBumper + LEFT * leftBumper);
       //forward(25cm)
    //   move_specific_distance(sensor, horizontal_amount);

       //rotate (90* opposite direction)
      // turn_robot_degrees(sensor, -(RIGHT * rightBumper + LEFT * leftBumper));

       ///done


}

/////this one is used by user
///it calls move_distance after instantiating a bot_data with the target y value
////
void move_specific_distance(oi_t *sensor, int cm) {
    move_data bot_move_data = {0,0,0,cm};
    int SPEED = 200;
    double distance_traveled = 0;
    signed char direction = 1;
    if (cm < 0)
    {
        direction = -1;
        cm *= direction;
    }
    timer_init();
    oi_setWheels(SPEED * direction, SPEED * direction); //rwheel, lwheel
    while (distance_traveled < cm * 10)
    {
        oi_update(sensor);
        distance_traveled += (sensor->distance * direction);
        bot_move_data.y=distance_traveled;
    }
    oi_setWheels(0, 0);
}

unsigned char actually_move_until_detect_obstacle(oi_t *sensor, int cm)
{
    FieldEdgeFound = 0;
    move_data bot_move_data = {0,0,0,cm};
    int SPEED = 200;
    double distance_traveled = 0;
    signed char direction = 1;
    if (cm < 0)
    {
        direction = -1;
        cm *= direction;
    }
    //lcd_init();
    timer_init();
    oi_setWheels(SPEED * direction, SPEED * direction); //rwheel, lwheel
    //while loop to travel the set distance
    unsigned char returnflag = 0;

    while (distance_traveled < cm * 10)
    {
        if(StopOnLine(sensor))
        {
            break;
        }
        oi_update(sensor);
        distance_traveled += (sensor->distance * direction);
        bot_move_data.y=distance_traveled;
        int which_cliff=detect_cliff(sensor);
        if (which_cliff>0)
        {
            on_detect_cliff(sensor,&bot_move_data, which_cliff);
        }
        if (sensor->bumpLeft)
            returnflag = returnflag | HIT_LEFT_BUMPER;
        if (sensor->bumpRight)
            returnflag = returnflag | HIT_RIGHT_BUMPER;
        if (returnflag > 0)
        {
            oi_setWheels(0, 0);
            return returnflag;
        }
        if (data_received_flag && data_received == 't')
        {
            break;
        }
    }
    oi_setWheels(0, 0);
    return returnflag;
}

const int backwards_amount = 0;
const int horizontal_amount = 10;
void move_around_obstacles(oi_t *sensor, int centimeters)
{

    //init vars
    int SPEED = 200;
    double distance_traveled = 0;
    double hor_pos = 0;



    signed char direction = 1;
    if (centimeters < 0)
    {
        direction = -1;
        centimeters *= direction;
    }
      centimeters+MAGIC_DiSTANCE_IONCREASE;
    //lcd_init();
    timer_init();
    oi_setWheels(SPEED * direction, SPEED * direction); //rwheel, lwheel
    //while loop to travel the set distance
    while (distance_traveled < centimeters * 10)
    {
        oi_update(sensor);
        distance_traveled += (sensor->distance * direction);
        if (sensor->bumpLeft && sensor->bumpRight)
        {
            move_around_obstacle(sensor, 1, 0);
            hor_pos-=horizontal_amount;
            distance_traveled -= backwards_amount;
            oi_setWheels(SPEED * direction, SPEED * direction);
        }
        else if (sensor->bumpLeft)
        {
            move_around_obstacle(sensor, 1, 0);
            hor_pos-=horizontal_amount;

            distance_traveled -= backwards_amount;
            oi_setWheels(SPEED * direction, SPEED * direction);
        }
        else if (sensor->bumpRight)
        {
            move_around_obstacle(sensor, 0, 1);
            hor_pos+=horizontal_amount;

            distance_traveled -= backwards_amount;
            oi_setWheels(SPEED * direction, SPEED * direction);
        }
        else if (data_received_flag && data_received == 't')
        {
            break;
        }

        //lcd_printf("The Distance traveled: %lf",distance_traveled);
    }
    int turn = 0;
    if (hor_pos > .42) {
        turn = -90;
        turn_robot_degrees(sensor, turn);
    }
    else if (hor_pos < -.42){
        turn = 90;
        turn_robot_degrees(sensor,turn);
        hor_pos *= -1;
    }

    actually_move_until_detect_obstacle(sensor, horizontal_amount);
    if (turn != 0)turn_robot_degrees(sensor, -turn);
    //stop, update display
    oi_setWheels(0, 0);

}
void move_around_obstacle(oi_t * sensor, char leftBumper, char rightBumper)
{
    if (data_received_flag && data_received == 't')
          {
              return;
          }
    //backwards (15 cm)
    int RIGHT = 90;
    int LEFT = -90;
    move_specific_distance(sensor, -backwards_amount);
    //rotate(90 * direction)

    turn_robot_degrees(sensor, RIGHT * rightBumper + LEFT * leftBumper);
    //forward(25cm)
    move_specific_distance(sensor, horizontal_amount);

    //rotate (90* opposite direction)
    turn_robot_degrees(sensor, -(RIGHT * rightBumper + LEFT * leftBumper));

    ///done

}
