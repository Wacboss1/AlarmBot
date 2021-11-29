/*
*   uart.h
*   Used to set up the UART [UART1 at 115200]
*
 * Description: This is file is meant for those that would like a little
 *              extra help with formatting their code.
*/

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

// These two varbles have been declared
// in the file containing main
extern volatile  char uart_data;  // Your UART interupt code can place read data here
extern volatile  char flag;       // Your UART interupt can update this flag
                                  // to indicate that it has placed new data
                                  // in uart_data       


///this one checks interupt status returns 1 if there is input that needs to be read
char is_input();

//blocking. just calls char_input(NULL)
char getChar();
///this one is blocking if NULL input or waits for time in ms.
char char_input(int wait_time_ms);


void uart_init(int baud);

void uart_sendChar(char data);

char uart_receive(void);

void uart_sendStr(const char *data);

void uart_interrupt_init();

void uart_interrupt_handler();

#endif /* UART_H_ */
