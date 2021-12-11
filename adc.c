/*
 * adc.c
 *
 *  Created on: Oct 19, 2021
 *      Author: wclemms
 */

#include "adc.h"
#include "Timer.h"
#include <inc/tm4c123gh6pm.h>

///initiates IR sensor
//// uses 16 hardware averaging values
void adc_init()
{
    //configure GPIO ports for the ADC
    SYSCTL_RCGCADC_R |= 0b00000001;
    SYSCTL_RCGCGPIO_R |= 0b00000010;
    GPIO_PORTB_AFSEL_R |= 0b10000;
    GPIO_PORTB_DEN_R &= 0b11101111;
    GPIO_PORTB_AMSEL_R |= 0b00010000;
    GPIO_PORTB_ADCCTL_R |= 0b0001000;

    //configure sample sequencers
    ADC0_ACTSS_R &= 0b1110;
    ADC0_EMUX_R &= 0xFFF0;
    ADC0_SSMUX0_R |= 0xA;
    ADC0_SSCTL0_R |= 0b110;
    ADC0_ACTSS_R |= 0b0001;
    ADC0_SAC_R |= 0x4;
}

///read an IR sensor value
int adc_read(void)
{
    ADC0_PSSI_R = 0b0001;
    while(~ADC0_RIS_R & 0b0001){}
    int val = ADC0_SSFIFO0_R;
    ADC0_ISC_R = 0b0001;
    return val;
}
