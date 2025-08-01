// UART1.c
#include "UART1.h"
#include "UART2.h"

//用户和ESP01S连接
void USART2_Init(void) {
	// 开启时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// 初始化引脚
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PA2:USART2_TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3:USART2_RX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART配置
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStructure);

	// 启动USART2
	USART_Cmd(USART2, ENABLE);

	//配置中断源
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	//初始化NVIC设置
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 11;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_InitStruct);
}

void USART2_IRQHandler(void) {
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		char data = USART_ReceiveData(USART2);
		BaseType_t Is_Woken = pdTRUE;
		xQueueSendFromISR(queue_esp01s, &data, &Is_Woken);
	}
}

// 发送字节
void USART2_SendByte(uint8_t Byte) {
	USART_SendData(USART2, Byte);

	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void send_to_esp(char *format, ...) {
	// 加锁
	char strs[100];

	// 替换内容 -> 存储到strs
	va_list list;
	va_start(list, format);
	vsprintf(strs, format, list);
	va_end(list);

	// strs: 通过串口发走
	for (uint8_t i = 0; strs[i] != '\0'; i++) {
		USART2_SendByte(strs[i]);
	}
	// 解锁
}
