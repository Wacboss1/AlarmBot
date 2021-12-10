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
    ////reboot it to clear any f errors
    oi_close();
    oi_free(interface);
    interface = oi_alloc();
        oi_init(interface);

    adc_init();
    button_init();
    lcd_init();
    better_servo_init();
    uart_init(115200);
   //  SonarConfig();
    uart_interrupt_init();


    botprintf("battery: %d out of %d\n\r",interface->batteryCharge,interface->batteryCapacity );
    lcd_printf("battery: %d out of %d\n",interface->batteryCharge,interface->batteryCapacity);
    //configure_wheels(interface);

    init_imu();
    int error=init_high_speed();
        if (error) {
            botprintf("failed init highspeed\n\r");
            return;
}

    //TODO check if cliffSignal is greater than 2700 for white tape

//    while(1){
//        lcd_printf("FrontLeftSignal:\n%d\nFrontRightSignal:\n%d",interface->cliffFrontLeftSignal, interface->cliffFrontRightSignal);
//        oi_update(interface);
//    }


    ///servo calibration tests
    turn_servo_deg(0);

    manual_mode(interface);


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

                    while (1) {
                        signed short orientation[3];
                         get_orientation(orientation);
                    }

                    break;
                case 'Q':
                    //oi_shutoff_init();
                    oi_free(interface);
                    break;
                case 'M':
                    manual_mode(interface);
                    break;
                default:
                   break;


                }

            }


        }



}
