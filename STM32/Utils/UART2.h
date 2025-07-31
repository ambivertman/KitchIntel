// UART1.h
#ifndef __UART2_H__
#define __UART2_H__

// STM32外设库头文件
#include "stm32f10x.h" 
#include <stdio.h>
#include <stdarg.h>
#include "freertos.h"
#include "queue.h"

void USART2_Init(void);
void USART2_SendByte(uint8_t Byte);
void send_to_esp(char *format, ...);

extern QueueHandle_t queue2;


#endif
