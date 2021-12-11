/*
 * adc.h
 *
 *  Created on: Oct 19, 2021
 *      Author: wclemms
 */

#ifndef ADC_H_
#define ADC_H_

#include "Timer.h"
#include <inc/tm4c123gh6pm.h>

///init IR
void adc_init(void);
///run ir
int adc_read(void);

#endif /* ADC_H_ */
