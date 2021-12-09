/*
 * imu.c
 *
 *  Created on: Dec 6, 2021
 *      Author: ndgartin
 */

#include "uart.h"
#include "imu.h"
#include "Timer.h"
///shift this 1 to the left and GET RID OF ZERO

/*
 * i2c
 * pin A6 is SCL1
 * pin A7 is SDA1
 * pin B6 and 7 are IMU I2C RESTN/GPIO2 and IMU I2C ADDR/GPIO1 but I have no 0xFACC-ing idea what that is for
 *
 *
 */
/*
 * confiuring IMU
 * PS1 PS0 == 0b00 for standard/fast i2c interface
 * com0 = sda send
 * com1 = scl recieve
 * Slave HIGH 0x29
 */
/*
 * configuring bot
 * enable i2c clcock page 348
 * enable gpio clock
 * alt function gpio
 * i2csda pin for open drain 676
 * PMCn fields in gpio pctl
 * init i2c master by writing i2cmr with 0x00000010
 * set clock speed:
 * (System clock / (2* SCL_LP + SCL_HP)*(SCL_CLK))-1
 * they use SCL_LP=6, SCL_HP=4
 *write clock speed to I2CMTPR
 *I2CMSA gets set with address of slave and transmit flag
 *data goes into I2CMDR
 *i2cmcs gets command (STOP,START,RUN)
 *poll I2CMCS BUSBSY bit until cleared
 *also has error bit
 *also
 */
static char imu_error_codes[11][100] = { "0 No error",
                                       "1 Peripheral initialization error",
                                       "2 System initialization error",
                                       "3 Self test result failed",
                                       "4 Register map value out of range",
                                       "5 Register map address out of range",
                                       "6 Register map write error",
                                       "7 BNO low power mode not available for selected operation mode",
                                       "8 Accelerometer power mode not available",
                                       "9 Fusion algorithm configuration error",
                                       "A Sensor configuration error"};

typedef struct imu_handle {
    int accX;
    int accY;
    int accZ;
    int gyroX;
    int gyroY;
    int gyroZ;

} imu_handle;


#define STOP_IMU I2C_MCS_STOP             //0x4
#define START_IMU I2C_MCS_START //0x2
#define TR_IMU I2C_MCS_RUN             // 0x1
#define ACK_IMU I2C_MCS_ACK             // 0x8



int get_imu_error() {
    char data;
    int error = get_byte(SYS_ERR, &data);
    botprintf("%s\n\r",imu_error_codes[data]);
    return error;
}
int wait_while_bus_busy(int milli){
    unsigned int count=timer_getMillis();
      unsigned int max_count=count+milli;
      while ((I2C1_MCS_R &  0x40) )//&& (count<max_count)) //while busy
          {
          count=timer_getMillis();
      }
      if (count>max_count) {
          return 1;
      }
      else {
          return 0;
      }
      }

int access_addr(char add) {
   // timer_waitMicros(5);
 //   timer_waitMicros(5);

    int error=0;
      I2C1_MSA_R = (DEFAULTI2C<<1); //address goes to bits 7-1, what sort of behavior is bit 0
    //  botprintf("should be the same: %X and %X and should be 0x52\n\r",I2C1_MSA_R,DEFAULTI2C<<1);

      I2C1_MDR_R = add; ///lololol data
      I2C1_MCS_R = 3;// (TR_IMU | START_IMU);//7 or 3 /// (STOP,START,RUN) ?!?!?!??
      ///bit 0 === master can t/r
      // bit 1 ---- generate START
      /// bit 2 ---- generate STOP
      ////bit 3 ----- ACK the controller does an ack automatically
      //////bit 4 highspeed enable bit


      //I2C1_MCS_R

   //   int counter=0;
      while (I2C1_MCS_R &  0x01) //while busy
          {
          //counter++;
      }
      error = I2C1_MCS_R & 0xE;
      if (error) {
          botprintf("Error in send\n\r");
          return error;
      }
      return 0;
}

int debugging_reset() {


}
int send_byte(char add, char data) {

        int error = access_addr(add);
        if (error) {
            botprintf("error: %d\n\r",error);
            return error;
        }
     //   I2C1_MSA_R = (DEFAULTI2C<<1);
       I2C1_MDR_R = data; /// data
       //I2C1_MCS_R &=0xFFFFFFE0;


       I2C1_MCS_R = 5;// (I2C_MCS_RUN | I2C_MCS_STOP); /// (STOP,START,RUN) ?!?!?!??

       //I2C1_MCS_R
    //   botprintf("waiting...");
      wait_while_bus_busy(100);
       error=(I2C1_MCS_R & 0xE);
       if (error) {
           botprintf("Error in send\n\r");
           return error;
       }

    ///set gpio to read
    ///
       return 0;

}


int get_bytes(char add, char buffer[], int countC) {
    int error = access_addr(add);
           if (error) {
               botprintf("error: %d\n\r",error);
               return error;
           }

           I2C1_MSA_R = (DEFAULTI2C<<1) | 0x1;
           int i=0;

               /*
                * #define STOP_IMU I2C_MCS_STOP             //0x4
#define START_IMU I2C_MCS_START //0x2
#define TR_IMU I2C_MCS_RUN             // 0x1
#define ACK_IMU I2C_MCS_ACK             // 0x8
                *
                */
           I2C1_MSA_R = (DEFAULTI2C<<1) | 0x1;

               I2C1_MCS_R = 0xB; //(I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_ACK);  ///(START | ACK| TR)
               while (I2C1_MCS_R &  0x1) //while busy
                                     {

                                      }
                                          buffer[0]=I2C1_MDR_R;

           for (i=1;i<(countC-1);i++){
               //I2C1_MCS_R
            //   botprintf("waiting...");
          // I2C1_MSA_R = (DEFAULTI2C<<1) | 0x1;
                     I2C1_MCS_R = (0x9); // RUN aknowledge
               while (I2C1_MCS_R &  0x1) //while busy
                   {

               }
               buffer[i]=I2C1_MDR_R;
           }
           //I2C1_MSA_R = (DEFAULTI2C<<1) | 0x1;
                               I2C1_MCS_R = (0x5); // RUN STOP
                         while (I2C1_MCS_R &  0x1) //while busy
                             {

                         }
                         buffer[countC-1]=I2C1_MDR_R;
                         wait_while_bus_busy(100);

                         return 0;



}


int get_byte(char add, char * data) {
    int error=0;
    error = access_addr(add);
   // timer_waitMicros(10);

    if (error) {
        botprintf("error: %d\n\r",error);
        return error;
    }
//    I2C1_MDR_R=0;

    I2C1_MSA_R =(( DEFAULTI2C<<1) | 0x1); ///recieve
   I2C1_MCS_R = (START_IMU |TR_IMU|STOP_IMU) ;

   wait_while_bus_busy(100);

   error=(I2C1_MCS_R & 0xE);
   if (error) {
       botprintf("Errord\n\r");
       return error;
   }
   *data=I2C1_MDR_R;

    return error;
}

/*
 * i2c
 * pin A6 is SCL1
 * pin A7 is SDA1
 * pin B6 and 7 are IMU I2C RESTN/GPIO2 and IMU I2C ADDR/GPIO1 but I have no 0xFACC-ing idea what that is for
 *
 *
 */
int init_imu() {
    ///set gpio to write
    ///GPIO A pins 6,7
    SYSCTL_RCGCGPIO_R |= 0x01;              // enable clock GPIOA (page 340)
    GPIO_PORTA_AFSEL_R &= 0x3F;
    GPIO_PORTA_AFSEL_R |= 0xC0;
    GPIO_PORTA_DEN_R   |= 0b11000000;

    GPIO_PORTA_PCTL_R  &= 0x00FFFFFF;
    GPIO_PORTA_PCTL_R |= 0x33000000;

    GPIO_PORTA_ODR_R |= 0x80; ///open drain. no idea what that is, but its got to be set. i2csda actually is only one that has to be set.
   // GPIO_PORTA_DIR_R   |=0xC0; //set as output
    //GPIO_PORTA_DIR_R   &=0xFFFFFF3F; // both out[ut. pretty sure we need sda to go back and forth between in and out for now im setting 6 to output and the other nothing. lol .
    //GPIO_PORTA_DIR_R |=0x000000C0; ///input
    ////setup i2c
    SYSCTL_RCGCI2C_R  |= SYSCTL_RCGCI2C_R1;
    I2C1_MCR_R = 0x00000010; ///initiate as master
    I2C1_MTPR_R = 0x7;///clock speed. 100kbhz is what is in datasheet so just using that. maximum is 400khz   fuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu
    //I2C1_MSA_R = (DEFAULTI2C<<1) | TRANSMIT_I2C; //address goes to bits 7-1, what sort of behavior is bit 0
    I2C1_MCLKOCNT_R = 0xFF; ///this is the timeout limit
    char test_data=0;


    timer_waitMillis(30);
//    debugging_reset();
        send_byte(SYS_TRIGGER, 0x10); ///reset the imu
        timer_waitMillis(30);

    get_imu_error();

unsigned int   error = get_byte(OPR_MODE, &test_data);

    botprintf("operation mode==data: %u\n\r",test_data);

    ///can only change values in config mode
    //7 ms to switch to operation mode. 19 ms to switch to configmode
    /*
    timer_waitMillis(30);
    send_byte(OPR_MODE, GYRO_ACCR_MODE);
       timer_waitMillis(30);
    int error = get_byte(OPR_MODE, &test_data);
    if (error) {
          botprintf("error: %d\n\r",error);
      }
    botprintf("orignal read value: %u\n\r",test_data);
     error = get_byte(POWER_MODE, &test_data);
     timer_waitMillis(30);

       if (error) {
             botprintf("error: %d\n\r",error);
         }
       botprintf("orignal read value: %u\n\r",test_data);
       timer_waitMillis(30);

        error = get_byte(ACC_X_L, &test_data);
          if (error) {
                botprintf("error: %d\n\r",error);
            }
          botprintf("orignal read value: %u\n\r",test_data);



    send_byte(OPR_MODE, GYRO_ACCR_MODE);
    timer_waitMillis(30);
     error = get_byte(OPR_MODE, &test_data);
    if (test_data!=GYRO_ACCR_MODE) {
        botprintf("Nope!!!! data: %u\n\r",test_data);
    }
    if (error) {
        botprintf("error: %d\n\r",error);
    }
    /*
     * OPR_MODE 0x3D  ///the defaualt value = config = 0x1C write to bits 3-0
#define GYRO_ACCR_MODE
     */

    return 0;
}



int init_high_speed() {
    int test_data;
   // while ((test_data!=M4G_MODE) ) {
    send_byte(OPR_MODE, NDOF_MODE);
        timer_waitMillis(400);
         test_data= get_byte(OPR_MODE, &test_data);

        if (test_data!=NDOF_MODE ){
            timer_waitMillis(400);
            get_imu_error();
        botprintf("Nope!!!! data: %u\n\r",test_data);
        }
    //}
return 0;

}
int get_gyro(char gyrodata[]) {

    int error = get_bytes(GYRO_X_L, gyrodata, 6);
    if (error) {
        botprintf("error in get gyro: %d",error);
        botprintf("%u,%u,%u\n\r",gyrodata[0],gyrodata[2],gyrodata[4]);

        return error;
    }
   // botprintf("%u,%u,%u\n\r",gyrodata[0],gyrodata[2],gyrodata[4]);

     signed short x=gyrodata[0]+ (((unsigned int) gyrodata[1])<<8);
     signed short y=gyrodata[2]+ (((unsigned int) gyrodata[3])<<8);
     signed short z=gyrodata[4]+ (((unsigned int) gyrodata[5])<<8);
    botprintf("%d,%d,%d\n\r",x,y,z);


    return error;
}

int get_rotation(signed short * data) {
    char charbuff[2];
    //timer_waitMillis(1);

    int error = get_bytes(HEADING_L, charbuff, 2);
           if (error) {
               botprintf("error in get gyro: %d",error);
               //botprintf("%u,%u,%u\n\r",charbuff[0],charbuff[2],charbuff[4]);

               return error;
           }
           *data=charbuff[0]+ (((signed short) charbuff[1])<<8);
}
int get_orientation(signed short buffer[]) {
    char charbuff[6];
    int error = get_bytes(HEADING_L, charbuff, 6);
       if (error) {
           botprintf("error in get gyro: %d",error);
           botprintf("%u,%u,%u\n\r",charbuff[0],charbuff[2],charbuff[4]);

           return error;
       }
      // botprintf("%u,%u,%u\n\r",gyrodata[0],gyrodata[2],gyrodata[4]);

        buffer[0]=charbuff[0]+ (((signed short) charbuff[1])<<8);
        buffer[1]=charbuff[2]+ (((signed short) charbuff[3])<<8);
        buffer[2]=charbuff[4]+ (((signed short) charbuff[5])<<8);


       return error;
}

int getAcc(signed short buffer[]) {
    char charbuff[6];
        int error = get_bytes(ACC_X_L, charbuff, 6);
        if (error) {
                  botprintf("error in get accel: %d",error);
                  botprintf("%u,%u,%u\n\r",charbuff[0],charbuff[2],charbuff[4]);

                  return error;
              }
             // botprintf("%u,%u,%u\n\r",gyrodata[0],gyrodata[2],gyrodata[4]);

               buffer[0]=charbuff[0]+ (((signed short) charbuff[1])<<8);
               buffer[1]=charbuff[2]+ (((signed short) charbuff[3])<<8);
               buffer[2]=charbuff[4]+ (((signed short) charbuff[5])<<8);

}
int getVel(signed short buffer[]) {
    char charbuff[6];
        int error = get_bytes(LINEAR_VEL, charbuff, 6);
        if (error) {
                  botprintf("error in get vel: %d",error);
                  botprintf("%u,%u,%u\n\r",charbuff[0],charbuff[2],charbuff[4]);

                  return error;
              }
             // botprintf("%u,%u,%u\n\r",gyrodata[0],gyrodata[2],gyrodata[4]);

               buffer[0]=charbuff[0]+ (((signed short) charbuff[1])<<8);
               buffer[1]=charbuff[2]+ (((signed short) charbuff[3])<<8);
               buffer[2]=charbuff[4]+ (((signed short) charbuff[5])<<8);

}



int parse_packets(char packets[]) {


}
/*
 * //*****************************************************************************
//
// I2C registers (I2C1)
//
//*****************************************************************************
#define I2C1_MSA_R              (*((volatile unsigned long *)0x40021000))
#define I2C1_MCS_R              (*((volatile unsigned long *)0x40021004))
#define I2C1_MDR_R              (*((volatile unsigned long *)0x40021008))
#define I2C1_MTPR_R             (*((volatile unsigned long *)0x4002100C))
#define I2C1_MIMR_R             (*((volatile unsigned long *)0x40021010))
#define I2C1_MRIS_R             (*((volatile unsigned long *)0x40021014))
#define I2C1_MMIS_R             (*((volatile unsigned long *)0x40021018))
#define I2C1_MICR_R             (*((volatile unsigned long *)0x4002101C))
#define I2C1_MCR_R              (*((volatile unsigned long *)0x40021020))
#define I2C1_MCLKOCNT_R         (*((volatile unsigned long *)0x40021024))
#define I2C1_MBMON_R            (*((volatile unsigned long *)0x4002102C))
#define I2C1_MCR2_R             (*((volatile unsigned long *)0x40021038))
#define I2C1_SOAR_R             (*((volatile unsigned long *)0x40021800))
#define I2C1_SCSR_R             (*((volatile unsigned long *)0x40021804))
#define I2C1_SDR_R              (*((volatile unsigned long *)0x40021808))
#define I2C1_SIMR_R             (*((volatile unsigned long *)0x4002180C))
#define I2C1_SRIS_R             (*((volatile unsigned long *)0x40021810))
#define I2C1_SMIS_R             (*((volatile unsigned long *)0x40021814))
#define I2C1_SICR_R             (*((volatile unsigned long *)0x40021818))
#define I2C1_SOAR2_R            (*((volatile unsigned long *)0x4002181C))
#define I2C1_SACKCTL_R          (*((volatile unsigned long *)0x40021820))
#define I2C1_PP_R               (*((volatile unsigned long *)0x40021FC0))
#define I2C1_PC_R               (*((volatile unsigned long *)0x40021FC4))
 */
