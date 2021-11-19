/*
 * movement.c
 *
 *  Created on: Sep 7, 2021
 *      Author: wclemms
 */
#include "movement.h"
#include "open_interface.h"
#include "lcd.h"
#include <math.h>

void stopWheels(void)
{
    oi_setWheels(0, 0);
}



void move(int right_wheel, int left_wheel,double totalDistance,bool notRecurssive, oi_t* sensor)
{
    stopWheels();
    double distance = 0;
    //adjustForHardware(&right_wheel, &left_wheel);
    //direction noted by wheel sign (negative or positive)
    while(distance < totalDistance)
    {
        oi_setWheels(right_wheel, left_wheel);
        oi_update(sensor);
        distance += sensor->distance;
        lcd_printf("Distance traveled:\n %lf", distance);
        if((sensor->bumpLeft) && notRecurssive)
        {
            distance -= 150.0f;
            LeftBumperHit(sensor);
            break;
        } else if(sensor->bumpRight){
            distance -= 150.0f;
            RightBumperHit(sensor);
            break;
        }
    }
    stopWheels();
}

void LeftBumperHit(oi_t* sensor)
{
    contact = true;
    reverse(-200, -200, -20, sensor);
    rotateRight(90, sensor);
    move(200, 200, 200, true, sensor);
    rotateLeft(90, sensor);
}

void RightBumperHit(oi_t* sensor)
{
    contact = true;
    reverse(-200, -200, -10, sensor);
    rotateLeft(90, sensor);
    move(200, 200, 200, true, sensor);
    rotateRight(90, sensor);
}

void reverse(int right_wheel, int left_wheel, double totalDistance,  oi_t* sensor)
{
    double distance = 0;
    adjustForHardware(&right_wheel, &left_wheel);

    while(distance > totalDistance)
        {
            oi_setWheels(right_wheel, left_wheel);
            oi_update(sensor);
            distance += sensor->distance;
            lcd_printf("Distance traveled:\n %lf", distance);
        }
}

void adjustForHardware(int* right_wheel, int* left_wheel)
{
    //adjust for hardware
    *left_wheel -= (1 - .65);
}
void rotateLeft(double degrees, oi_t *sensor)
{
    stopWheels();
    double angle = 0;
    //adjust for hardware
    int right_wheel = 50, left_wheel = -50;

    oi_setWheels(right_wheel, left_wheel);
    while(abs(angle) < degrees)
    {
        oi_update(sensor);
        angle += sensor->angle;
        lcd_printf("Amount turned:\n %f", abs(angle));
    }
    stopWheels();
}

void rotateRight(double degrees, oi_t *sensor)
{
    stopWheels();
    double angle = 0;
    //adjust for hardware
    int right_wheel =-50, left_wheel = 50;

    oi_setWheels(right_wheel, left_wheel);
    while(abs(angle) < degrees)
    {
        oi_update(sensor);
        angle += sensor->angle;
        lcd_printf("Amount turned:\n %f", abs(angle));
    }
    stopWheels();
}

void ControlMove(char command, int moveSpeed, int rotateSpeed)
{
    if (command == 'w')
    {
        //turn on the wheels
        oi_setWheels(moveSpeed, moveSpeed);
    }
    else if (command == 's')
    {
        oi_setWheels(-moveSpeed, -moveSpeed);
    }
    else if (command == 'a')
    {
        oi_setWheels(rotateSpeed, -rotateSpeed);
    }
    else if (command == 'd')
    {
        oi_setWheels(-rotateSpeed, rotateSpeed);
    }
    else if (command == 'e')
    {
        moveSpeed += 10;
    }
    else if (command == 'q')
    {
        moveSpeed -= 10;
    }
    else if (command == '-')
    {
        //turn off the wheels
        stopWheels();
    }
}
