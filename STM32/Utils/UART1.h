// UART1.h
#ifndef __UART1_H__
#define __UART1_H__

// STM32外设库头文件
#include "stm32f10x.h" 
#include <stdio.h>
#include <stdarg.h>
#include "freertos.h"
#include "queue.h"

void USART1_Init(void);
void USART1_SendByte(uint8_t Byte);
void printf1(char* format, ...);

extern QueueHandle_t queue;


#endif
