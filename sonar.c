/*
 * sonar.c
 *
 *  Created on: Oct 26, 2021
 *      Author: wclemms
 */

#include "Timer.h"
#include "sonar.h"

int const SPEED_OF_SOUND = 34000;

/*
 * work in progress, making an init to run at the beginning of the program so less code in the sendstart
void SonarInit() {
    ///clocks
         SYSCTL_RCGCGPIO_R |= 0x2;              //Enable PORTB clock
         SYSCTL_RCGCTIMER_R |= 0x8;//set the clock for the clock, how else does a clock tell time, except for to check a clock?
         GPIO_PORTB_DEN_R |= 0x8;               //Enable PORTB_pin3
         GPIO_PORTB_DIR_R |= 0x00000008;         //Set PORTB_pin3 as output
         GPIO_PORTB_AFSEL_R |= 0x8; //Set pin3 as an alternate function
         GPIO_PORTB_PCTL_R &=0xFFFFF0FF;
         GPIO_PORTB_PCTL_R |=0x700;
}
*/

void SonarSendStart()
{
    //TIMER3_CTL_R &= 0xEFF;
    TIMER3_IMR_R &=0x00000000;
    //make port B wire 3 and output
    GPIO_PORTB_DIR_R |= 0b00001000;
    GPIO_PORTB_AFSEL_R &= 0b11110111;

    GPIO_PORTB_DATA_R |= 0b00001000;
    timer_waitMicros(10);
    GPIO_PORTB_DATA_R &= 0b11110111;

    TIMER3_CTL_R |= 0x100;
    //make port B wire 3 and input
    GPIO_PORTB_DIR_R &= 0b11110111;
    GPIO_PORTB_AFSEL_R |= 0b00001000;

    ///NEED TO BE MORE PRECISE with clear interrupts (MAYBE)!!!!
TIMER3_ICR_R |=0xFFFFFFFF;                      //Clear interrupts
TIMER3_IMR_R |=0x400;               //set mask interrupts .. why do we unmask interupts here


   // TIMER3_CTL_R |= 0x100;

}

//TODO rename
void SonarConfig()
{
    SYSCTL_RCGCGPIO_R |= 0b00000010;
    GPIO_PORTB_DEN_R |= 0b00001000;
    GPIO_PORTB_DIR_R &= 0b11110111;
    GPIO_PORTB_AFSEL_R |= 0b00001000;
    GPIO_PORTB_PCTL_R |= 0x7000;

    SYSCTL_RCGCTIMER_R |= 0b1000;
    TIMER3_IMR_R &= 0xBFF;
    TIMER3_CTL_R &= 0xEFF;
    TIMER3_CFG_R |= 0x4;
    TIMER3_TBMR_R |= 0b0111;
    /*
     * ahaaa
     *  these are settings for timerA
     */
    //TIMER3_CTL_R |= 0b01100;
    TIMER3_CTL_R |= 0xE00;
    TIMER3_TBPR_R |= 0xFF;
    TIMER3_TBILR_R |= 0xFFFF;
    TIMER3_IMR_R |= 0x400;
    TIMER3_ICR_R |= 0x400;

    //
    IntRegister(INT_TIMER3B, Timer3Handler);
    NVIC_EN1_R |= 0b10000;

    //IntMasterEnable();
}

void Timer3Handler()
{
    //TODO Get the interupt to trigger
    if(TIMER3_MIS_R & 0x400)
    {
        TIMER3_ICR_R |= 0x400;

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
    else {
    }
    //TIMER3_ICR_R |= 0x400;
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
