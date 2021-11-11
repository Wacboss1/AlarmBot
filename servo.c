/*
 * servo.c
 *
 *  Created on: Nov 2, 2021
 *      Author: wclemms
 */

#include "servo.h"
#include "Timer.h"
#include "lcd.h"

void ServoInit()
{
    pulsePeriod = 320000;

    SYSCTL_RCGCGPIO_R |= 0b00000010;
    GPIO_PORTB_DEN_R |= 0b00100000;
    GPIO_PORTB_DIR_R |= 0b00100000;
    GPIO_PORTB_AFSEL_R |= 0b00100000;
    GPIO_PORTB_PCTL_R |= 0x7000;

    SYSCTL_RCGCTIMER_R |= 0b10;
    TIMER1_CTL_R &= 0xEFF;
    TIMER1_CFG_R |= 0x4;
    TIMER1_TBMR_R = 0b1010;
    TIMER1_TBPR_R = pulsePeriod >> 16;
    TIMER1_TBILR_R = pulsePeriod & 0xFFFF;
    TIMER1_TBMATCHR_R = pulsePeriod - 16000;
    TIMER1_CTL_R |= 0x100;
}

void ServoConfigure()
{
    bool done = 0;
    while(!done)
    {
        if((GPIO_PORTE_DATA_R & 0b10) == 0b00000000)
        {
                TIMER1_TBMATCHR_R += 250;
                timer_waitMillis(100);
        }
        else if ((GPIO_PORTE_DATA_R & 0b1) == 0b00000000)
        {
            TIMER1_TBMATCHR_R -= 250;
            timer_waitMillis(100);
        }
        lcd_printf("%d", TIMER1_TBMATCHR_R & 0xFFFF);
    }

}

void RotateServo(float angle)
{
    float slope = (leftValue - rightValue) / 180;
    int newMatch = (slope * angle) + rightValue;
    TIMER1_TBMATCHR_R = newMatch;
    timer_waitMillis(100);
}

void ResetServo()
{
    RotateServo(0);
    timer_waitMillis(500);
}
