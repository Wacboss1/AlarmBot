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

void move_specific_distance(oi_t *sensor, int centimeters)
{
    //init vars
    double distance_traveled = 0;
    signed char direction = 1;
    if (centimeters < 0)
    {
        direction = -1;
        centimeters *= direction;
    }
    //lcd_init();
    timer_init();
    oi_setWheels(200 * direction, 200 * direction); //rwheel, lwheel
    //while loop to travel the set distance
    while (distance_traveled < centimeters * 10)
    {
        oi_update(sensor);
        distance_traveled += (sensor->distance * direction);
        if (data_received_flag && data_received == 't')
        {
            break;
        }
        //lcd_printf("The Distance traveled: %lf",distance_traveled);
    }

    //stop, update display
    oi_setWheels(0, 0);

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

unsigned char actually_move_until_detect_obstacle(oi_t *sensor, int cm)
{
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
        oi_update(sensor);
        distance_traveled += (sensor->distance * direction);

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
void move_around_obstacle(oi_t *sensor, char leftBumper, char rightBumper)
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
