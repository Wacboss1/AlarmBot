/*
 * uart.h
 *
 *  Created on: Nov 16, 2021
 *      Author: wclemms
 */
/*
 *
 *
 Currently using uart in files_for_alarm folder
#ifndef UART_H_
#define UART_H_

#include "Timer.h"
#include <inc/tm4c123gh6pm.h>

void uart_init(void);

void uart_sendChar(char data);

char uart_receive(void);

void uart_sendStr(const char *data);

void UART1_Handler(void);


#endif /* UART_H_ */

