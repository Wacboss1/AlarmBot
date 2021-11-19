/*
*
*   uart.c
*
*
*
*
*
*   @author
*   @date
*/

#include "uart.h"
#include <inc/tm4c123gh6pm.h>

char data;

void uart_init(void){
    SYSCTL_RCGCUART_R |= 0b00000010;
    SYSCTL_RCGCGPIO_R |= 0b000010;
    GPIO_PORTB_AFSEL_R |= 0b00000011;
    GPIO_PORTB_PCTL_R  |= 0x00000011;       // pmc0 and pmc1       (page 688)  also refer to page 650
    GPIO_PORTB_DEN_R   |= 0b00000011;        // enables pb0 and pb1
    GPIO_PORTB_DIR_R   = (GPIO_PORTB_DIR_R | 0b00000010) & 0b11111110;        // sets pb0 as output, pb1 as input

    //compute baud values [UART clock= 16 MHz]
    double fbrd;
    int    ibrd;

    fbrd = 44; // page 903
    ibrd = 8;

    UART1_CTL_R &= 0b11111110;      // disable UART1 (page 918)
    UART1_IBRD_R = ibrd;        // write integer portion of BRD to IBRD
    UART1_FBRD_R = fbrd;   // write fractional portion of BRD to FBRD
    UART1_LCRH_R = 0b01100000;        // write serial communication parameters (page 916) * 8bit and no parity
    UART1_CC_R   = 0x0;          // use system clock as clock source (page 939)


    UART1_ICR_R |= 0b00010000;
    UART1_IM_R |= 0b00010000;
    NVIC_PRI1_R |= 0x00200000;
    NVIC_EN0_R = 0b01000000;

    IntRegister(INT_UART1, UART1_Handler);
    IntMasterEnable();
    UART1_CTL_R |= 0b1100000001; // enable UART1


}

void uart_sendChar(char data){
    while((UART1_FR_R & 0x20) != 0);
    UART1_DR_R = data;
}

char uart_receive(void){
    while((UART1_FR_R & 0b00010000) != 0);
    data = (char)(UART1_DR_R & 0xFF);
    return data;
}

void UART1_Handler()
{
    if(UART1_MIS_R & 0b00010000)
    {
        data = (char)(UART1_DR_R & 0xFF);
        uart_sendChar(data);
        uart_sendChar('\n');
        uart_sendChar('\r');
    }
}
