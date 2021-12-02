/*
 * traversal.h
 *
 *  Created on: Nov 30, 2021
 *      Author: ndgartin
 */

#ifndef TRAVERSAL_H_
#define TRAVERSAL_H_
int test_object_shit(oi_t * sensor);
int enter_endpoint(int deg, float dist);
int obj_in_way(scanned_obj * obj);
int scan();
int turnRight90(oi_t *sensor, int * cur);
#define FORWARD_OA 0
#define RIGHT_OA 1
#define BACK_OA 2
#define LEFT_OA 3
#define CELLSIZE_OA 50
turnLeft90(oi_t *sensor, int * cur);
faceMiddle(oi_t * sensor, int * cur, int * gridX);
move_forward_OA(oi_t *sensor, int * cur, int * gridX, int * gridY);
go_around_big_obstacles(oi_t *sensor);
find_endpoint(oi_t * sensor);
#endif /* TRAVERSAL_H_ */
