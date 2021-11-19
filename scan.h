/*
 * scan.h
 *
 *  Created on: Nov 19, 2021
 *      Author: ndgartin
 */

#ifndef SCAN_H_
#define SCAN_H_

#include "sonar.h"


int SonarScan(int angle, Ping* ping);
int IRRawScan(int angle);
int IRDistanceScan(int angle);
int GetActualDistance(int x);





#endif /* SCAN_H_ */
