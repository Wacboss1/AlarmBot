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
#include "open_interface.h"
#include "files_for_alarm/movement.h"
#include "files_for_alarm/uart.h"
#include "files_for_alarm/distancesensor.h"
#include "files_for_alarm/botconfig.h"


int main(void)
{
    adc_init();
    button_init();
    lcd_init();
    ServoInit();
    timer_init();
    uart_init(115200);


    oi_t* interface = oi_alloc();

}
