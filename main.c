/*
 * main.c
 *
 *  Created on: Sep 7, 2021
 *      Author: wclemms
 */

#include "adc.h"
#include "button.h"
#include "lcd.h"
#include "servo.h"
#include "sonar.h"
#include "Timer.h"
//#include "mission.h"
#include "open_interface.h"
#include "movement.h"
#include "uart.h"
#include "botconfig.h"
#include "scan.h"
#include "imu.h"
#include"scan.h"



///a basic IR calibration function
void muh_calibrate()
{
    int i = 0;
    double nTriesPerDist = 10;


    //SCAN at 8 different positions, five times
    ///I used these
    //48
    //40
    //32
    //28
    //24
    //22
    //18
    //12

    int pos_i = 8;
   // CyBOT_sendStr("Press any button to start scan\n\r");
    botprintf("IR\n\r");
     turn_servo_deg(90);

    while (pos_i)
    {

       char ch = getChar();

        pos_i--;
        double total = 0;
        double ping_total = 0;
        for (i = 0; i < (int) nTriesPerDist; i++)
        {
            //SCAN_(105, &temp);
           // adc_read(void)
            total += (double)adc_read();

        }
        total = total/10.0;

        botprintf("%Lf\n\r", total);



    }
    botprintf("Done scanning.\n\r");

}

////A more involved calibration functino that i never got accurate
void new_calibrate(oi_t * sensor) {
    int i=0;
    int max = 50;
    int nTriesPerDist=20;
    turn_servo_deg(90);

    while (i<max){
        double total = 0;
             double ping_total = 0;
             move_specific_distance(sensor,-1);
             timer_waitMillis(300);

             Ping pingy;
             int j;
             for (j = 0; j < (int) nTriesPerDist; j++)
             {
                 ping_total=0;
                 //SCAN_(105, &temp);
                // adc_read(void)
                 timer_waitMillis(40);
                 int poo = 0;
                 for (poo=0;poo<5;poo++) {
                     timer_waitMillis(5);

                     ping_total+=GetSonarDistance(&pingy);

                 }
                 ping_total=ping_total/5;
                 total = (double)adc_read();
                 botprintf("%lf,%lf\n\r",ping_total,total);


             }
         //    ping_total=ping_total/10;
           //  total = total/10.0;
             //botprintf("%lf,%lf\n\r",ping_total,total);


        i++;
    }


}

int main(void){
    //timer_init();
    timer_init();

    oi_t* interface = oi_alloc();
    oi_init(interface);
    ////reboot as a superstitious error prevention mechanism
    oi_close();
    oi_free(interface);
    interface = oi_alloc();
        oi_init(interface);

    adc_init();
    button_init();
    lcd_init();
    better_servo_init();
    uart_init(115200);
   //  SonarConfig();
    uart_interrupt_init();


   // botprintf("battery: %d out of %d\n\r",interface->batteryCharge,interface->batteryCapacity );
    lcd_printf("battery: %d out of %d\n",interface->batteryCharge,interface->batteryCapacity);
    //configure_wheels(interface);

    init_imu();

    int error=init_high_speed();
//    getChar();

  //  new_calibrate(interface);

        if (error) {
            botprintf("failed init highspeed\n\r");
            return;
}

    //TODO check if cliffSignal is greater than 2700 for white tape

//    while(1){
//        lcd_printf("FrontLeftSignal:\n%d\nFrontRightSignal:\n%d",interface->cliffFrontLeftSignal, interface->cliffFrontRightSignal);
//        oi_update(interface);
//    }


    ///servo calibration tests
    turn_servo_deg(0);

    ///start our manual mode
    manual_mode(interface);

        ///debugging/configuration commands loop
        while (1) {
            if (is_input()) {
                char c =  getChar();



                switch (c) {
                case '1':
                    botprintf("servo calibrate");
                    servo_calibration();
                    break;
                case '2':
                    //scan tests
                   // botprintf("scan tests");
                    new_calibrate(interface);
                    break;
                case '3':
                    ///test the cliff sensors functions
                    while (true){
                        oi_update(interface);
                        lcd_printf("cliffFrontLeftSignal: %d  cliff: %d  dirtDetect: %d",interface->cliffFrontLeftSignal, interface->cliffFrontLeft, interface->dirtDetect);
                        botprintf("cliffFrontLeftSignal: %d  cliff: %d  dirtDetect: %d\n\r",interface->cliffFrontLeftSignal, interface->cliffFrontLeft, interface->dirtDetect);
                    }

                    break;
                case '4':
                 //   test_scans_print_scans();
                   // test_object_shit(interface);
                    muh_calibrate();
                    break;

                case '5':
                    FindStartPostition(interface);
                    break;
                case 'g':
                    ;

                    while (1) {
                        signed short orientation[3];
                         get_orientation(orientation);
                    }

                    break;
                case 'Q':
                    //oi_shutoff_init();
                    oi_free(interface);
                    break;
                case 'M':
                    manual_mode(interface);
                    break;
                default:



                }

            }


        }



}
