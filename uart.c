/*
*
*   uart_extra_help.c
* Description: This is file is meant for those that would like a little
*              extra help with formatting their code, and followig the Datasheet.
*/

#include "uart.h"
#include "stdarg.h"
#include "stdio.h"

#include "timer.h"
volatile char data_received;
volatile char data_received_flag = 0;
#define ENABLE_PORTB 0x00000002
#define ENABLE_UART1 0x00000002
#define DISABLE_UART 0xFFFFFFFE
#define ENABLE_UART 0x00000301
#define PORTB_PCTL_SELECT_UART 0x0001;
#define PORTB_PINS_0_1 0x00000003
#define PB1OUT_PB0INPUT 0x00000010
#define CLEAR_IBRD 0xFFFF0000;
#define CLEAR_FBRD 0xFFFFFFC0;
#define IBRD_115200 0x00000008
#define FBRD_115200 0x0000002C
#define IBRD_9600 0x00000068
#define IBRD_9600 0x0000000B
#define FORMAT_FRAME 0x00000060
#define SYS_CLOCK 0x0
#define REPLACE_ME 0x0

void uart_init(int baud)
{
    SYSCTL_RCGCGPIO_R |= 0x02;              // enable clock GPIOB (page 340)
    SYSCTL_RCGCUART_R |= 0x02;              // enable clock UART1 (page 344)

    GPIO_PORTB_AFSEL_R |= 0x03;            // sets PB0 and PB1 as peripherals (page 671)
    GPIO_PORTB_PCTL_R  &= 0xFFFFFF00;
    GPIO_PORTB_PCTL_R  |= 0x11;              // pmc0 and pmc1       (page 688)  also refer to page 650
    GPIO_PORTB_DEN_R   |= 0x03;            // enables pb0 and pb1
    GPIO_PORTB_DIR_R   &= 0xFFFFFFFE;
    GPIO_PORTB_DIR_R   |= 0x2;           // sets pb0 as output, pb1 as input

    //compute baud values [UART clock= 16 MHz] 
    double fbrd;
    int    ibrd;

    fbrd = 16000000.0 / (16 * baud); // page 903
    ibrd = (int) fbrd;
    printf("%d\n", ibrd);
    printf("%lf\n", fbrd);
    fbrd = (int) ((fbrd- ibrd) * 64 + 0.5);
    printf("%d\n", fbrd);

    UART1_CTL_R &= 0xFFFFFFFE;        // disable UART1 (page 918)
    UART1_IBRD_R = ibrd;                // write integer portion of BRD to IBRD
    UART1_FBRD_R = fbrd;                // write fractional portion of BRD to FBRD
    UART1_LCRH_R = 0x60;

    UART1_CC_R   = 0x0;           // use system clock as clock source (page 939)
    UART1_CTL_R |= 0x00000301;         // enable UART1

}

void uart_sendChar(char data)
{
   //TODO
  //  UARTFR &= 0x0000000;
    while (UART1_FR_R & 0x00000020) {
        continue;
    }
    UART1_DR_R = data;

   
}

void botprintf(char *format, ...)
{
    va_list args;
    va_start(args, format);
    char tosend[1024];
    vsnprintf(tosend, 1024, format, args);
    uart_sendStr(tosend);
    va_end(args);

}

#define E_OVERRUN 0x8
#define E_BREAK  0x4
#define E_PAR 0x2
#define E_FRAME 0x01
char uart_error(char clear) {
    //value unimplemented
    char errors;
    if (UART1_RSR_R & E_OVERRUN) {
            printf("OVerrun error\n");

    }
    if (UART1_RSR_R & E_BREAK) {
                printf("break error\n");

        }
    if (UART1_RSR_R & E_PAR) {
            printf("parity error\n");

    }
    if (UART1_RSR_R & E_FRAME) {
        printf("framing error\n");
    }
    if (UART1_FR_R & 0x10){
        printf("Reciever is empty");
    }

    if (clear)
       UART1_ECR_R &= 0x1FF;
return errors;
}


char uart_receive(void)
{
#define NOTRECIEVED 0x00000010

    printf("UART1_DR_R: %0x\n",UART1_DR_R);
    printf("UART1_DR_R & 0xFFF: %0x\n",UART1_DR_R & 0xFFF);

    while ((UART1_FR_R & NOTRECIEVED )== NOTRECIEVED) {
       //unsigned long test= UART1_DR_R;

       /// printf("ONdition = %0x\n",UART1_FR_R & NOTRECIEVED );
        ///printf("notrecieveddefine: %0x\n",NOTRECIEVED);
        //printf("address: %0x\n",&UART1_FR_R);

        //printf("%0x\n",UART1_DR_R & 0xFF);
      //  lcd_putc((char) (UART1_DR_R & 0xFF));
        //reset data
       //UART1_ECR_R = 0000000
       //uart_error(1);
    }
    printf("UART1_DR_R: %0x\n",UART1_DR_R);
    printf("UART1_DR_R & 0xFFF: %0x\n",UART1_DR_R & 0xFFF);

    ///what is this logic?
    ///we need to ignore the most sig bit of the char, because if 1, it gives weird characters
    uart_error(1);

    return (char) (UART1_DR_R & 0xFF);
  //  return (char) (UART1_DR_R);
}

void uart_sendStr(const char *data)
{

    do {
        uart_sendChar(*data);
    }
    while (*(data++));
	
}

// _PART3


void uart_interrupt_init()
{
    // Enable interrupts for receiving bytes through UART1
    UART1_IM_R |= 0x00000010; //enable interrupt on receive - page 924

    // Find the NVIC enable register and bit responsible for UART1 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R |= 0x40; //enable uart1 interrupts - page 104

    // Find the vector number of UART1 in table 2-9 ! UART1 is 22 from vector number page 104
    IntRegister(INT_UART1, &uart_interrupt_handler); //give the microcontroller the address of our interrupt handler - page 104 22 is the vector number

}

void uart_interrupt_handler()
{
// STEP1: Check the Masked Interrup Status
    if (UART1_MIS_R & 0x00000010){
        data_received = UART1_DR_R & 0xFF;
        data_received_flag = 1;
        //printf("input interupt!\n");
        UART1_ICR_R |=0x10;
    }
}

char char_input(int wait_time_ms) {
    int start_time =timer_getMillis();
    int cur_time =start_time;
    if (wait_time_ms!=NULL) {

    }

    while (!data_received_flag )
              {

                  if (wait_time_ms!=NULL) {
                  cur_time =timer_getMillis();
                  if (cur_time - start_time >= wait_time_ms) {
                      return 0;
                  }
                  }
                  else {
                      continue;
                  }
              }
        data_received_flag=0;
        return  data_received;

    }

char getChar() {
    return char_input(NULL);

}

int getInt() {

    char c;
    int outint;
    /*
    do {
        c= getChar();

    }
    while (c!=0x0D);
    */
}
char is_input() {
    if (data_received_flag) {
        return 1;
}
    else
        return 0;
}
