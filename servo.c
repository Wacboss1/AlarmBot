/*
 * servo.c
 *
 *  Created on: Nov 2, 2021
 *      Author: ndgartin
 */
/*
 * clocks
 SYSCTL_RCGCGPIO_R |= 0x2;              //Enable PORTB clock
 SYSCTL_RCGCTIMER_R |= 0x8;//set the clock for the clock, how else does a clock tell time, except for to check a clock?
 //

 //gpio

 GPIO_PORTB_DEN_R |= 0x8;               //Enable PORTB_pin3
 GPIO_PORTB_DIR_R |= 0x00000008;         //Set PORTB_pin3 as output
 GPIO_PORTB_AFSEL_R |= 0x8; //Set pin3 as an alternate function
 GPIO_PORTB_PCTL_R &=0xFFFFF0FF;
 GPIO_PORTB_PCTL_R |=0x700;
 *
 */
#include "Timer.h"
#include "driverlib/interrupt.h"
#include "lcd.h"
#include "botconfig.h"
#include "button.h"
#include <inc/tm4c123gh6pm.h>


extern volatile char data_received_flag;
///0 deg default
unsigned long servo_zero = 0xC380;
///180 deg default
unsigned long servo_180 = 0x5380;

//#include "distancesensor.h"

void better_servo_init()
{
    SYSCTL_RCGCGPIO_R |= 0x2;              //Enable PORTB clock
    SYSCTL_RCGCTIMER_R |= 0x2;

    //gpio for servo
    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_DIR_R |= 0x20;
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R &= 0xFF0FFFFF;
    GPIO_PORTB_PCTL_R |= 0x00700000;

    //timerstuff

    //disable timer
    TIMER1_CTL_R &= 0xFFFFFEFF;

    ///from the book
    TIMER1_CFG_R &= 0xFFFFFFF8;
    TIMER1_CFG_R |= 0x4;
    //first 2 bits two for periodic,
    TIMER1_TBMR_R &= 0xFFFFF000;
    //TnAMS = 0x1
    //TncMR = 0x0
    //TnMR = 0x2
    TIMER1_TBMR_R |= 0xA;

    //320000 = 0x4_E200
    //trying with 20 ms in between instead of 19
    //336000 -> 0x52080
    //didn'twork
    ///start value
    TIMER1_TBILR_R = 0xE200;
    TIMER1_TBPR_R &= 0x00;
    //prescale
    TIMER1_TBPR_R |= 0x4;

    ///target value
    TIMER1_TBMATCHR_R = 0xC380; //use this later
                                // C380 roughly appears to be 0. for bot 00
                                // 8380 is roughly 90 (a little more than 90)
                                // 5380 is roughly 180
    TIMER1_TBPMR_R = 0x4;
    //enable
    TIMER1_CTL_R |= 0x00000100;

    // while (true) {
    //botprintf("%0x clockvalue\n\r",TIMER3_TBR_R)
    //   }

    servo_zero = SERVO_RIGHT;
    servo_180=SERVO_LEFT;
}

int deg_to_clkF(float degrees) {
   // float val = ((servo_zero - servo_180) * (180 - degrees)) / 180.0;
       double dval =(((double)servo_zero - (double)servo_180) * ((double)180 - (double)degrees)) / (double)180.0;
       //return servo_180 + val;
       return servo_180+ (int)dval;

}
//Take degrees, turn it into a clock value to set the TIMER1_TBMATCHR_R to
unsigned int deg_to_clk(char degrees)
{
    // ((servo_zero - servo_180) * degrees) / 180 = value to add to servo_180 to get angle;

    int val = ((servo_zero - servo_180) * (180 - degrees)) / 180.0;
    double dval =(((double)servo_zero - (double)servo_180) * ((double)180 - (double)degrees)) / (double)180.0;
    //return servo_180 + val;
    return servo_180+ (int)dval;

    /*
     unsigned int clck = degrees * (servo_zero / degrees);
     return clck;
     */
}
//Call microfunctions to turn the servo
///I added a little thing that makes it wait for time based off of how far the motor has to go
//it just waits for (change in degrees * 2) for now
///last degrees is held in last_deg
int turn_servo_deg(char degrees)
{
    static int last_deg;
    if (degrees < 181)
    {
        unsigned int clock_cycles = deg_to_clk(degrees);
        move_servo(clock_cycles);
        timer_waitMillis(abs(last_deg-degrees)*10);
        last_deg = degrees;

        return 0;
    }

    return -1;
}
/*
 * turn servo by a float, not implmemented
 */
int turn_servo_degF(float degrees) {
    static float last_deg=0;
    if (degrees < 181) {
        unsigned int clock_cycles = deg_to_clk(degrees);
               move_servo(clock_cycles);
               timer_waitMillis(abs(last_deg-degrees)*10);
               last_deg = degrees;
    }
}
/*
 * move serbo by clock cycle
 */
int move_servo(unsigned int clck_cycle)
{
    //timer_waitMillis(100);
    TIMER1_TBMATCHR_R = clck_cycle;
   // timer_waitMillis(100);
    return 0;
}
/*
 * calibrate servo
 */
void servo_calibration()
{
    botprintf(
            "Move bot to 0 degrees using 'a' and 'd' keys. Send space key to confirm value.\n\r");
    enum calibstate
    {
        GET_0_DEG, GET_180_DEG, DONE
    };
    enum calibstate curstate = GET_0_DEG;
    int incamount = 0x40;
    do
    {
        char keydown = char_input(NULL);
        if (keydown == 'a')
        {
            TIMER1_TBMATCHR_R -= incamount;
        }
        else if (keydown == 'd')
        {
            TIMER1_TBMATCHR_R += incamount;
        }
        else if (keydown == ' ')
        {
            if (curstate == GET_0_DEG)
            {
                curstate++;
                servo_zero = TIMER1_TBMATCHR_R & 0xFFFF;
                botprintf(
                        "0 degrees = %X\n\r do the same thing for the 180 degrees: \n\r",
                        servo_zero);
            }
            else
            {
                curstate++;
                servo_180 = TIMER1_TBMATCHR_R & 0xFFFF;
                botprintf("180 degrees = %X\n\r", servo_180);
            }
        }

    }
    while (curstate != DONE);
}
/*
 * another calibration but using lcd and buttons instead of uart
 */
void lab10_part2()
{
    button_init();
    turn_servo_deg(90);
    data_received_flag = 0;
    lcd_printf("Match Register Value:\n %X", TIMER1_TBMATCHR_R);
    enum dirState
    {
        clockwise, anticlockwise, DONE
    };
    enum dirState curstate = clockwise;
    char button_pushed = 0;
    do
    {
        lcd_printf("Match Register Value:\n %X", TIMER1_TBMATCHR_R);
        char button = button_getButton();

        switch (button)
        {
        case 0:
            button_pushed = 0;
            break;
        case 1:
            botprintf("Case 1\n\r");
            if (curstate == clockwise && button_pushed == 0)
            {
                turn_servo_deg(get_deg() - 1);
            }
            else if (curstate == anticlockwise && button_pushed == 0)
            {
                turn_servo_deg(get_deg() + 1);
            }
            button_pushed = 1;
            botprintf("Button Pushed: %d\n\rGetDeg(): %d\n\r", button_pushed, get_deg());
            break;
        case 2:
            botprintf("Case 2\n\r");

            if (curstate == clockwise && button_pushed == 0)
            {
                turn_servo_deg(get_deg() - 5);
            }
            else if (curstate == anticlockwise && button_pushed == 0)
            {
                turn_servo_deg(get_deg() + 5);
            }
            button_pushed = 1;
            botprintf("Button Pushed: %d\n\rGetDeg(): %d\n\r", button_pushed, get_deg());

            break;
        case 3:
            botprintf("Case 3\n\r");

            if (curstate == clockwise && button_pushed == 0)
            {
                curstate = anticlockwise;
            }
            else if (curstate == anticlockwise && button_pushed == 0)
            {
                curstate = clockwise;
            }
            button_pushed = 1;
            botprintf("Button Pushed: %d\n\rGetDeg(): %d\n\r", button_pushed, get_deg());

            break;
        case 4:
            botprintf("Case 4\n\r");

            if (curstate == clockwise && button_pushed == 0)
            {
                turn_servo_deg(0);
            }
            else if (curstate == anticlockwise && button_pushed == 0)
            {
                turn_servo_deg(180);
            }
            button_pushed = 1;
            botprintf("Button Pushed: %d\n\rGetDeg(): %d\n\r", button_pushed, get_deg());

            break;
        default:
            break;
        }
        if (data_received_flag){
            curstate = DONE;
            data_received_flag = 0;
        }

    }
    while (curstate != DONE);
}
/*
 *  int val = ((servo_zero - servo_180) * (180 - degrees)) / 180;
 return servo_180 + val;
 *
 */
/*
 * get our current servo location as a degree
 */
char get_deg()
{

    char deg = -((((TIMER1_TBMATCHR_R) - servo_180) * 180)
            / (servo_zero - servo_180)) + 179;
    double ddeg = -(((((double)TIMER1_TBMATCHR_R) - (double)servo_180) * (double)180)
            / ((double)servo_zero - (double)servo_180)) + (double)180;

    return (char)ddeg;
}

