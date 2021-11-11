/*
 * sonar.c
 *
 *  Created on: Oct 26, 2021
 *      Author: wclemms
 */

#include "Timer.h"
#include "sonar.h"

int const SPEED_OF_SOUND = 34000;

void SonarSendStart()
{
    TIMER3_CTL_R &= 0xEFF;

    //make port B wire 3 and output
    GPIO_PORTB_DIR_R |= 0b00001000;
    GPIO_PORTB_AFSEL_R &= 0b11110111;

    GPIO_PORTB_DATA_R |= 0b00001000;
    timer_waitMillis(.7);
    GPIO_PORTB_DATA_R &= 0b11110111;

    //make port B wire 3 and input
    GPIO_PORTB_DIR_R &= 0b11110111;
    GPIO_PORTB_AFSEL_R |= 0b00001000;

    TIMER3_CTL_R |= 0x100;

}

//TODO rename
void SonarConfig()
{
    SYSCTL_RCGCGPIO_R |= 0b00000010;
    GPIO_PORTB_DEN_R |= 0b00001000;
    GPIO_PORTB_DIR_R &= 0b11110111;
    GPIO_PORTB_AFSEL_R |= 0b00001000;
    GPIO_PORTB_PCTL_R |= 0x7000;

    TIMER3_IMR_R &= 0xBFF;
    TIMER3_CTL_R &= 0xEFF;
    TIMER3_CFG_R = 0x4;
    TIMER3_TBMR_R = 0b0111;
    TIMER3_CTL_R |= 0b01100;
    TIMER3_TBPR_R = 0xFF;
    TIMER3_TBILR_R = 0xFFFF;
    TIMER3_IMR_R |= 0x400;
    TIMER3_ICR_R |= 0x400;

    NVIC_EN1_R |= 0b10000;
    //
    IntRegister(INT_TIMER3B, Timer3Handler);
    IntMasterEnable();
}

void Timer3Handler()
{
    //TODO Get the interupt to trigger
    if(TIMER3_MIS_R & 0x400)
    {
        if(high)
        {
            fallingTime = TIMER3_TBR_R;
            done = 1;
        }
        else
        {
            risingTime = TIMER3_TBR_R;
        }
        high = !high;
    }
    TIMER3_ICR_R |= 0x400;
}

int SonarCalcDistance(Ping* currentPing)
{
    currentPing->pulseWidth = currentPing->risingTime - currentPing->fallingTime;
    if(currentPing->pulseWidth < 0)
    {
        currentPing->overflows++;
        return 0;
    }
    double time = 1 / 16e6;
    return (currentPing->pulseWidth * time * SPEED_OF_SOUND) / 2;
}

int GetSonarDistance(Ping* currentPing)
{
    high = 0;
    done = 0;
    SonarConfig();
    SonarSendStart();

    while(!done){}
    currentPing->fallingTime = fallingTime;
    currentPing->risingTime = risingTime;

    return SonarCalcDistance(currentPing);
}
