// UART1.h
#ifndef __UART3_H__
#define __UART3_H__

// STM32外设库头文件
#include "stm32f10x.h" 
#include <stdio.h>
#include <stdarg.h>
#include "freertos.h"
#include "queue.h"

void USART3_Init(void);
void USART3_SendByte(uint8_t Byte);
void printf3(char *format, ...);

extern QueueHandle_t queue1;


#endif
