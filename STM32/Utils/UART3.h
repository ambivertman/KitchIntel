// UART3.h
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
void send_to_hc05(char *format, ...);

extern QueueHandle_t queue_hc05;

#endif
