/*
 * movement.h
 *
 *  Created on: Sep 7, 2021
 *      Author: ndgartin
 */


#ifndef MOVEMENT_H_
#define MOVEMENT_H_

int FieldEdgeFound;
#define MAGIC_DiSTANCE_IONCREASE 40
#define MOVE_FORWARD oi_setWheels(200, 200)
#define MOVE_BACKWARD oi_setWheels(-200, -200)
#define TURN_LEFT oi_setWheels(100,-100)
#define TURN_RIGHT oi_setWheels(-100, 100)
#define HIT_LEFT_BUMPER 0x01
#define HIT_RIGHT_BUMPER 0x02
#define HIT_BOTH_BUMPER 0x03
#define HIT_EITHER_BUMPER(i) ((HIT_BOTH_BUMPER | i)>0)

typedef struct movement_data{
    double x;
    double y;
    int rotation;
    double target_x;
    double target_y;
    double waypoint_x;
    double waypoint_y;

} move_data;

#include "open_interface.h"
    extern int current_deg;
    void move_distance(oi_t *sensor, move_data * bot_data);
    void move_specific_distance(oi_t *sensor, int centimeters); //Use a sensor to move a specific distance, in cm
    void turn_robot_degrees(oi_t *sensor, int degrees_to_turn); //Use a sensor to turn a specific number of degrees
    void move_around_obstacles(oi_t *sensor, int centimeters);
    void FindStartPostition(oi_t* sensor);
    int StopOnLine(oi_t* sensor);
    /*
     * DOESNT ACTUALLY MOVE uNTiL hit. IT MOVES AROUND THE ObJECT
     */
    void move_until_hit(oi_t *sensor, int centimeters);
    /*
     * Actually just moves given direction until a bumper detects something. Returns flag with values defined in the HIT_BUMPER defines
     *
     */
    unsigned char actually_move_until_detect_obstacle(oi_t * sensor, int cm);

#endif /* MOVEMENT_H_ */

