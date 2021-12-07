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
#include "botconfig.h"
#include "scan.h"
#include "imu.h"

///used with simplescam.
///mimics the old scan structure from cybotscan

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



    botprintf("battery: %d out of %d\n\r",interface->batteryCharge,interface->batteryCapacity );
    lcd_printf("battery: %d out of %d\n",interface->batteryCharge,interface->batteryCapacity);
    //configure_wheels(interface);
    init_imu();
    int county=10;
    while (county>0) {
        county--;
    unsigned int value;
    get_uint(ACC_X_L, &value);
    botprintf("plase work: %d, %X\n\r",value,value);
    timer_waitMillis(50);
    }


    scanned_obj objects[4];
    scan180_alarmbot(objects);




    ///this is a bunch of tests, we can put it into a function later sometime
        //
        //uart
        //char c = char_input(NULL);
        //botprintf("%c",c);


    //FindStartPostition(interface);


//    while(1){
//        lcd_printf("FrontLeftSignal:\n%d\nFrontRightSignal:\n%d",interface->cliffFrontLeftSignal, interface->cliffFrontRightSignal);
//        oi_update(interface);
//    }


    ///servo calibration tests
    turn_servo_deg(0);


        while (1) {
            if (is_input()) {
                char c =  getChar();



                switch (c) {
                case '1':
                    botprintf("servo calibrate");
                    servo_calibration();
                    break;
                case '2':
                    //scan tests
                    botprintf("scan tests");

                    break;
                case '3':
                    ///test the cliff sensors functions
                    while (true){
                        oi_update(interface);
                        lcd_printf("cliffFrontLeftSignal: %d  cliff: %d  dirtDetect: %d",interface->cliffFrontLeftSignal, interface->cliffFrontLeft, interface->dirtDetect);
                        botprintf("cliffFrontLeftSignal: %d  cliff: %d  dirtDetect: %d\n\r",interface->cliffFrontLeftSignal, interface->cliffFrontLeft, interface->dirtDetect);
                    }

                    break;
                case '4':
                 //   test_scans_print_scans();
                   // test_object_shit(interface);
                    break;

                case '5':
                    FindStartPostition(interface);
                    break;
                case 'g':
                    ;


                    unsigned int value;
                       get_uint(ACC_X_L, &value);
                       botprintf("plase work: %d, %X\n\r",value,value);
                    break;
                case 'Q':
                    //oi_shutoff_init();
                    oi_free(interface);
                    break;
                default:
                   break;


                }

            }


        }



}
