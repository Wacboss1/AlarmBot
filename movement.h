/*
 * movement.h
 *
 *  Created on: Sep 7, 2021
 *      Author: ndgartin
 */


#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#define MAGIC_DiSTANCE_IONCREASE 40
#define MOVE_FORWARD oi_setWheels(200, 200)
#define MOVE_BACKWARD oi_setWheels(-200, -200)
#define TURN_LEFT oi_setWheels(100,-100)
#define TURN_RIGHT oi_setWheels(-100, 100)
#define HIT_LEFT_BUMPER 0x10
#define HIT_RIGHT_BUMPER 0x20
#define HIT_BOTH_BUMPER 0x30
#define HIT_EITHER_BUMPER(i) ((HIT_BOTH_BUMPER | i)>0)

#define CLIFFFRONTLEFT 0x1
#define CLIFFLEFT 0x2
#define CLIFFFRONTRIGHT 0x4
#define CLIFFRIGHT 0x8
#define FRONT_CLIFF_HIT (CLIFFFRONTLEFT || CLIFFFRONTRIGHT)
#define BOTHRIGHT 0xD
#define BOTHFRONT 0x5
#define BOTHLEFT 0x3


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
    int configure_wheels(oi_t *sensor);
    /*
     * DOESNT ACTUALLY MOVE uNTiL hit. IT MOVES AROUND THE ObJECT
     */
    void move_until_hit(oi_t *sensor, int centimeters);
    /*
     * Actually just moves given direction until a bumper detects something. Returns flag with values defined in the HIT_BUMPER defines
     *
     */
    unsigned int actually_move_until_detect_obstacle(oi_t *sensor, int cm);
    int update_move_data(int dist);
    int update_rotation_data(int deg);

#endif /* MOVEMENT_H_ */

