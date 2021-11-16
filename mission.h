/*
 * Mission.h
 *
 *  Created on: Nov 15, 2021
 *      Author: wclemms
 */

#ifndef MISSION_H_
#define MISSION_H_

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <WMuart.h>

#include "open_interface.h"
#include "Timer.h"
#include "lcd.h"
#include "movement.h"
#include "adc.h"
#include "sonar.h"
#include "servo.h"
#include "button.h"

const int NUMBER_OF_SCANS = 50;
const int MAXOBJECTS = 7;
const int LOOK_ANGLE = 180;
const int IDENTIFY_DISTANCE = 700;

typedef struct FoundObjects
{
    int objectNumber;
    int endAngle;
    int distanceAway;
    int width;
    int startAngle;
} FoundObject;

int SonarScan(int angle, Ping* ping)
{
    RotateServo(angle);
    timer_waitMillis(500);
    return GetSonarDistance(ping);
}

FoundObject CreateNewFoundObject(int objectsFound, int startAngle, float endAngle, int newWidth, Ping* ping)
{
    FoundObject newObject;
    newObject.objectNumber = objectsFound;
    newObject.endAngle = endAngle;
    newObject.startAngle = startAngle;
    newObject.width = newWidth;
    newObject.distanceAway = SonarScan((startAngle + endAngle)/2, ping);
    return newObject;
}

void PrintToPutty(char message[100])
{
    int j = 0;
    for (j = 0; j < strlen(message); j++)
    {
        uart_sendChar(message[j]);
    }
}

int GetActualDistance(int x)
{
    int actual = (-6.426e-15 * pow(x, 5)) + (5.919e-11 * pow(x, 4)) + (-2.175e-7 * pow(x, 3)) + (4.075e-4 * pow(x, 2)) + (-4.079e-1 * x) + 1.991e2;
    return actual;
}

int IRRawScan(int angle)
{
    RotateServo(angle);
    return adc_read();
}

int IRDistanceScan(int angle)
{
    RotateServo(angle);
    return GetActualDistance(adc_read());
}

void LogScan(int i, int* IR_val, int* actual, char message[]){
    *IR_val = IRRawScan(i);
    *actual = GetActualDistance(*IR_val);
    sprintf(message, "%d\t %d\t\t %d\n\r", i, *IR_val, *actual);
    PrintToPutty(message);
}

void IdentifyObjects(int i,int* IR_val, int* actual_ptr, int* foundAngle,
                     int*startAngle, bool* sameObject,
                     int* objectCount, FoundObject* objectsSeen, Ping* ping)
{
    if (*IR_val > IDENTIFY_DISTANCE && *sameObject)
    {
        if(*startAngle == 0)
        {
            *startAngle = i;
        }
        *foundAngle = i;
        *sameObject = false;
    }
    else if (*IR_val < IDENTIFY_DISTANCE && !*sameObject)
    {
        int objectWidth = i - *foundAngle;
        if (objectWidth > LOOK_ANGLE / NUMBER_OF_SCANS)
        {
            objectsSeen[*objectCount] = CreateNewFoundObject(*objectCount, *startAngle, *foundAngle,
                                                            objectWidth, ping);
            (*objectCount)++;
        }
        *sameObject = true;
        *startAngle = 0;
    }
}

FoundObject* FindThinestObject(int* objectCount, FoundObject objectsSeen[MAXOBJECTS], char message[])
{
    FoundObject* lowestWidthObject = objectsSeen;
    int j;
    sprintf(message, "Object#:\t Angle:\t Distance:\t Width:\n\r");
    PrintToPutty(message);
    for (j = 0; j < *objectCount; j++)
    {
        sprintf(message, "%d\t\t %d\t\t %d\t\t %d\n\r",
                objectsSeen[j].objectNumber, objectsSeen[j].endAngle,
                objectsSeen[j].distanceAway, objectsSeen[j].width);
        PrintToPutty(message);
        if (abs(objectsSeen[j].width) < abs(lowestWidthObject->width))
        {
            lowestWidthObject = &objectsSeen[j];
        }
    }
    return lowestWidthObject;
}

void RotateTowardsObject(oi_t* openInterface,FoundObject* lowestWidthObject, char message[])
{
    int targetAngle = lowestWidthObject->startAngle;
    sprintf(message,"Lowest width object found at angle: %d\n\r", targetAngle);
    PrintToPutty(message);

    if(targetAngle < 90)
    {
        rotateRight(90 - targetAngle, openInterface);
        sprintf(message,"Rotating %d degrees to the right\n\r", 90 - targetAngle);
    } else {
        rotateLeft(abs(targetAngle - 90) , openInterface);
        sprintf(message,"Rotating %d degrees to the left\n\r", targetAngle - 90);
    }
    PrintToPutty(message);
}

void Mission()
{
    int moveSpeed = 250;
    int rotateSpeed = moveSpeed / 2;

    char message[100];
    int i, j;
    int smallestDistance;
    contact = false;

    FoundObject objectsSeen[MAXOBJECTS] = malloc(
            sizeof(FoundObject) * MAXOBJECTS);
    Ping* ping = malloc(sizeof(Ping));
    int *actual_ptr = malloc(sizeof(int));
    int *IR_val = malloc(sizeof(int));
    int *foundAngle = malloc(sizeof(int));
    int *startAngle = malloc(sizeof(int));
    bool *sameObject = malloc(sizeof(bool));
    *sameObject = true;
    int *objectCount = malloc(sizeof(int));
    *objectCount = 0;
    oi_t *openInterface = oi_alloc();



    oi_init(openInterface);

    PrintToPutty("Angle:\t IR Distance:\t Actual Distance:\n\r");
    for (i = 0; i <= LOOK_ANGLE; i += LOOK_ANGLE / NUMBER_OF_SCANS)
    {
        LogScan(i, IR_val, actual_ptr, message);
        IdentifyObjects(i, IR_val, actual_ptr, foundAngle, startAngle,
                        sameObject, objectCount, objectsSeen, ping);
    }
    FoundObject *lowestWidthObject = FindThinestObject(objectCount, objectsSeen,
                                                       message);
    RotateTowardsObject(openInterface, lowestWidthObject, message);
    sprintf(message, "Lowest width object is %d cm away\n\r",
            lowestWidthObject->distanceAway);
    PrintToPutty(message);
    int distanceToMove = (lowestWidthObject->distanceAway * 10) - 60;
    move(100, 100, distanceToMove, true, openInterface);

    free(objectsSeen);
    free(ping);
    free(actual_ptr);
    free(IR_val);
    free(foundAngle);
    free(startAngle);
    free(sameObject);
    free(objectCount);
    free(openInterface);
}

void PrintIRValue()
{
    char message[100];
    int sensorData = adc_read();
    sprintf(message, "%d\t %d\n\r", sensorData, GetActualDistance(sensorData));
    PrintToPutty(message);
}


#endif /* MISSION_H_ */
