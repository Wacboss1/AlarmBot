/*
 * main.c
 *
 *  Created on: Sep 7, 2021
 *      Author: wclemms
 */

#include "mission.h"



int main(void)
 {
    //TODO figure out why we can't remove cyBOT_Scan
    cyBOT_init_Scan();
    lcd_init();
    uart_init();
    adc_init();
    ServoInit();
    button_init();

    rightValue = 50856;
    leftValue = 22106;

//    ServoConfigure(); //used for configureation


    do
    {
        ResetServo();
        Mission();
    } while(contact == true);
    return 0;
}
