/*
 * imu.h
 *
 *  Created on: Dec 6, 2021
 *      Author: ndgartin
 */

#ifndef IMU_H_
#define IMU_H_
#define DEFAULTI2C  0b00101001/// 0b00101001
                      ///0x29


///register address
#define GYRO_X_U 0x15
#define GYRO_X_L 0x14
///gyro is 0x14 to 0x19
////xl,xu,yl,yu,zl,zu

///accel is 0x8 to 0xD
//same as gyro

#define ACC_X_U 0x09
#define ACC_X_L 0x08
#define HEADING_L 0x1A
#define SYS_ERR 0x3A
#define LINEAR_VEL 0x28
/*
 * Read : 0 No error
1 Peripheral initialization error
2 System initialization error
3 Self test result failed
4 Register map value out of range
5 Register map address out of range
6 Register map write error
7 BNO low power mode not available for selected operation mode
8 Accelerometer power mode not available
9 Fusion algorithm configuration error
A Sensor configuration error
 */


#define UNIT_SELECT 0x3B

#define GET_EUL_UNIT(a) ((a>>2) & 0x1) ///degrees =0. radians = 1

#define RECIEVE_I2C 0x1
#define TRANSMIT_I2C 0x0
#define OPR_MODE 0x3D  ///the defaualt value = config = 0x1C write to bits 3-0
#define SYS_TRIGGER 0x3F /// set bit 5 to reset the system

///operational modes
#define GYRO_ACCR_MODE 0b0101   ///this is raw output with no hardware calcations
#define M4G_MODE 0b1010 ///this is the good one for getting orientation
#define NDOF_MODE 0b1100
#define POWER_MODE 0x3E   //should be 0x00 by default
int init_imu();
//unsigned int get_16bit(char add);

/*
 * get multiple bytes
 */
int get_bytes(char add, char buffer[], int countC);
/*
 * get gyroscop values
 */
int get_gyro(char gyrodata[]);
/*
 * start fusion mode
 */
int init_high_speed();
/*
 * get heading, pitch, and roll, needs 6 byte buffer
 */
int get_orientation(signed short buffer[]);
/*
 * get heading only, needs 2 byte buffer
 */
int get_rotation(signed short * data);
/*
 * returns the imu  error register value
 */
int get_imu_error();
/*
 * gets acceleration
 */
int getAcc(signed short buffer[]);
/*
 * gets linear acceleration
 */
int getVel(signed short buffer[]);

#endif /* IMU_H_ */
