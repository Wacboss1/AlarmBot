/*
 * imu.h
 *
 *  Created on: Dec 6, 2021
 *      Author: ndgartin
 */

#ifndef IMU_H_
#define IMU_H_
#define DEFAULTI2C 0b00101001
                      ///0x29
#define GYRO_X_U 0x15
#define GYRO_X_L 0x14


#define ACC_X_U 0x09
#define ACC_X_L 0x08

#define RECIEVE_I2C 0x1
#define TRANSMIT_I2C 0x0
#define OPR_MODE 0x3D  ///the defaualt value = config = 0x1C write to bits 3-0
#define GYRO_ACCR_MODE 0b0101

#define POWER_MODE 0x3E   //should be 0x00 by default
int init_imu();
//unsigned int get_16bit(char add);
int get_uint(char add, unsigned int * data);


#endif /* IMU_H_ */
