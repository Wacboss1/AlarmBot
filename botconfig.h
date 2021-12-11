/*
 * botconfig.h
 *
 * Most of the configurations for different bots are here. Most of the debugging/output defines are here
 *
 *  Created on: Oct 18, 2021
 *      Author: ndgartin
 */


#ifndef BOTCONFIG_H_
#define BOTCONFIG_H_

/*
 * BIG STUFF HERE
 */
///WHICH BOT
#define MYBOT 9
//print more data
#define VERBOSE 2
#if VERBOSE==2
#define VERY_VERBOSE 1
#else
#define VERY_VERBOSE 0
#endif
///show putty
#define PUTTY 0
//use data from array instead of scans
#define USE_DEBUG_DATA 0

extern int GUI;

#if PUTTY==0
#else
#endif

//#define _SIMULATOR 0
#ifdef _SIMULATOR
#define BASE_IR_VALUE_MULTIPLIER 2000000
#define MIN_IR_VALUE 600
#define convert_raw_ir_to_distance(raw) raw
#else
#define MIN_IR_VALUE 1000
#endif


///formula used for raw ir to dist
//#define DSTFRMIR(raw) (IR_CONST + IR_COEFF * pow(raw,IR_EXP))

/*
 * BOT adjustments and shieeeeeeeet
* null values:
*   IR_EXP = 1
*   IR_COEFF = 1
*   IR_COEEFF1 = 0
*   IR_CONST = 0
*
 */


#if MYBOT==14
#define IR_EXP -1.872
#define IR_COEFF 20000000
#define SERVO_LEFT 0x50AC
#define SERVO_RIGHT 0xBCAC
#endif

#if MYBOT==5
#define IR_EXP 2
#define IR_COEFF .00009
#define IR_COEFF1 -.2568
#define IR_CONST 191.04

/*MORE TRASH
#define IR_EXP 2
#define IR_COEFF .00005
#define IR_COEFF1 .2466
#define IR_CONST 189.04
*/
/*
 * This one is marking nothing as something. trash
 */
/*
#define IR_CONST 0
#define IR_EXP -2.6
#define IR_COEFF 2000000000
*/
#endif
//cybot 0
#if MYBOT==3
#define IR_EXP -1.803
#define IR_COEFF 10000000
#define SERVO_LEFT 1246000
#define SERVO_RIGHT 248500


#endif
#if MYBOT==2
#define IR_EXP -1.604
#define IR_COEFF 3000000
#define SERVO_LEFT 0x5040
#define SERVO_RIGHT 0xC340
#endif


#if MYBOT==10
#define IR_EXP -1.02 //-1.072
#define IR_COEFF 19278

//just stole bot zero servo vals:
#define SERVO_LEFT 1293250
#define SERVO_RIGHT 269500

#endif

///THIS CAN't be right
#if MYBOT==0
#define CYBOT_EXP -1.461
#define CYBOT_COEFF 2000000
#define IR_CONST 0
#define IR_EXP -1.419
#define IR_COEFF 690614
#define SERVO_LEFT 1293250
#define SERVO_RIGHT 269500
#endif


#if MYBOT==1
#define SERVO_LEFT 0x5380
#define SERVO_RIGHT 0xC780
#endif
#if MYBOT==9
#define SERVO_LEFT 0x52D3
#define SERVO_RIGHT 0xC0D3
#endif

#if MYBOT==12
#define SERVO_LEFT 0x5008
#define SERVO_RIGHT 0xBE48
#endif
//null values:
#ifndef IR_EXP
#define IR_EXP  1
#endif
#ifndef IR_COEFF
#define IR_COEFF  1
#endif
#ifndef IR_COEFF1
#define IR_COEFF1  0
#endif
#ifndef IR_CONST
#define IR_CONST  0
#endif
#ifndef SERVO_RIGHT
#define SERVO_LEFT 1
#define SERVO_RIGHT 1
#endif


#endif /* BOTCONFIG_H_ */


