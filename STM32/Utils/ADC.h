#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f10x.h"    

void AD_Init(void);
uint16_t AD_GetValue(uint8_t ADC_Channel);

#endif

