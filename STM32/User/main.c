#include "stm32f10x.h"    
#include "freertos.h"
#include "task.h"
#include "queue.h"
#include "UART1.h" 
#include "UART2.h" 
#include "UART3.h" 
#include <string.h>

QueueSetHandle_t xQueueSet;
QueueHandle_t queue1, queue2;

void task1(void *arg) {
	char buf[60] = { 0 };
	char data = 0;

	while (1) {
		//AT指令通过串口输入经由USART1到STM32中,
		//STM32对收到的指令进行转发通过USART2到ESP01S
		//ESP01S会对发来的AT指令进行回应
		//所有的数据收发都在buf中,是否需要分离??可以建立一个消息队列集来判断是哪里发来的数据
		QueueSetMemberHandle_t set = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);
		//消息来自串口工具
		if (set == queue1) {
			BaseType_t ret = xQueueReceive(queue1, &data, 0);
			if (ret == pdTRUE) {
				buf[strlen(buf)] = data;
			}
			if (data == '\n') {
				//printf1("%s", buf);
				send_to_esp(buf);
				memset(buf, 0, sizeof(buf));
			}
		}

		//消息来自esp01s
		if (set == queue2) {
			BaseType_t ret = xQueueReceive(queue2, &data, 0);
			if (ret == pdTRUE) {
				buf[strlen(buf)] = data;
			}
			if (data == '\n') {
				printf1("%s", buf);
				// if (strstr(buf, "OK") != NULL) {
				// 	printf1("command 1!\r\n");
				// } else if (strstr(buf, "FAIL") != NULL) {
				// 	printf1("command 0!\r\n");
				// }
				memset(buf, 0, sizeof(buf));
			}
		}
	}

}


int main(void) {
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);

	USART1_Init();
	USART2_Init();

	queue1 = xQueueCreate(5, 1);
	queue2 = xQueueCreate(5, 1);

	// 5+5:表示创建一个最多可存储10+5个事件(所有队列最大值的总和)的队列集
	xQueueSet = xQueueCreateSet(5 + 5);

	// 将队列加入队列集
	xQueueAddToSet(queue1, xQueueSet);
	xQueueAddToSet(queue2, xQueueSet);

	xTaskCreate(task1, "Task1", 4 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	vTaskStartScheduler();

	while (1) {
	}
}


