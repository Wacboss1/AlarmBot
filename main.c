/*
 * main.c
 *
 *  Created on: Sep 7, 2021
 *      Author: wclemms
 */

#include "adc.h"
#include "button.h"
#include "lcd.h"
#include "servo.h"
#include "sonar.h"
#include "Timer.h"
//#include "mission.h"
#include "open_interface.h"
#include "movement.h"
#include "uart.h"
#include "files_for_alarm/distancesensor.h"
#include "files_for_alarm/botconfig.h"

///used with simplescam.
///mimics the old scan structure from cybotscan
typedef struct scan_handle {
    int angle;
    int  ping;
    int IR;
} scan_handle;

int simpleScam(char deg, scan_handle * scn) {
    turn_servo_deg(deg);
    ///waiting for a long time
    ///i reccomend guessing the amount of time it will take the servo to move (current_deg - new_deg) * (10 milli per degree)
    //timer_waitMillis(500);
 //   timer_waitMillis()
    scn->angle=deg;
    Ping png;
    scn->ping = GetSonarDistance(&png);
    scn->IR = IRDistanceScan(deg);
    return 0;
}

void printScn(scan_handle * scn) {
    //int dist = GetActualDistance(scn->IR);
    botprintf("deg: %d, ping: %d, dist(IR): %d\n\r",scn->angle,scn->ping,scn->IR);
}

int main(void){
    timer_init();
    oi_t* interface = oi_alloc();
    oi_init(interface);


    adc_init();
    button_init();
    lcd_init();
    better_servo_init();
    uart_init(115200);
    SonarConfig();
    uart_interrupt_init();
    botprintf("battery: %d out of %d\n\r",interface->batteryCharge,interface->batteryCapacity);
    lcd_printf("battery: %d out of %d\n",interface->batteryCharge,interface->batteryCapacity);


    scanned_obj* objects = smallest_object_360();



    ///this is a bunch of tests, we can put it into a function later sometime
        //
        //uart
        //char c = char_input(NULL);
        //botprintf("%c",c);

    //FindStartPostition(interface);

    ///servo calibration tests
//    turn_servo_deg(0);
//    while (1) {
//        if (is_input()) {
//            char c =  getChar();
//            switch (c) {
//            case '1':
//                botprintf("servo calibrate");
//                servo_calibration();
//                break;
//            case '2':
//                //scan tests
//                botprintf("scan tests");
//         struct scan_handle scn;
//         simpleScam(180, &scn);
//         printScn(&scn);
//         simpleScam(0, &scn);
//         printScn(&scn);
//         simpleScam(90, &scn);
//         printScn(&scn);
//                break;
//            case '3':
//                ///test the movement functions
//                break;
//            default:
//                break;
//
//            }
//        }
//    }
}
