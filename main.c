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


typedef struct scan_handle {
    int angle;
    int  ping;
    int IR;
} scan_handle;

int simpleScam(char deg, scan_handle * scn) {
    turn_servo_deg(deg);
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
int main(void)
{
    timer_init();
    oi_t * interface = oi_alloc();
    oi_init(interface);


    adc_init();
    button_init();
    lcd_init();
    better_servo_init();
    uart_init(115200);
   //  SonarConfig();
    uart_interrupt_init();
    botprintf("battery: %d out of %d\n\r",interface->batteryCharge,interface->batteryCapacity );

    ///this is a bunch of tests, we can put it into a function later sometime
        //
        //uart
        //char c = char_input(NULL);
        //botprintf("%c",c);
        struct scan_handle scn;
        simpleScam(180, &scn);
        printScn(&scn);
        simpleScam(0, &scn);
        printScn(&scn);
        simpleScam(90, &scn);
        printScn(&scn);
        //scan

        //movement

       // botprintf("battery: %d out of %d\n\r",interface->batteryCharge,interface->batteryCapacity );

        while (1) {

        }



}
