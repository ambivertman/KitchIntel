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
	char USART_buf[100] = { 0 };
	char ESP_buf[100] = { 0 };
	char data = 0;

	while (1) {
		//AT指令通过串口输入经由USART1到STM32中,
		//STM32对收到的指令进行转发通过USART2到ESP01S
		//ESP01S会对发来的AT指令进行回应
		//所有的数据收发都在buf中,是否需要分离??可以建立一个消息队列集来判断是哪里发来的数据
		QueueSetMemberHandle_t set = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);
		//消息来自串口工具
		if (set == queue1) {
			while (xQueueReceive(queue1, &data, 200) == pdTRUE) {
				// 防止缓冲区溢出
				if (strlen(USART_buf) < sizeof(USART_buf) - 1) {
					USART_buf[strlen(USART_buf)] = data;
				} else {
					printf1("USART_buf overflow\r\n!");
				}
			}
			send_to_esp(USART_buf);
			memset(USART_buf, 0, sizeof(USART_buf));
		}

		//消息来自esp01s
		if (set == queue2) {
			//完整的消息体
			while (xQueueReceive(queue2, &data, 200) == pdTRUE) {
				// 防止缓冲区溢出
				if (strlen(ESP_buf) < sizeof(ESP_buf) - 1) {
					ESP_buf[strlen(ESP_buf)] = data;
				} else {
					printf1("esp_buf overflow\r\n!");
					memset(USART_buf, 0, sizeof(USART_buf));
				}
			}
			//最后一行的返回总是要等到下一次有消息才能打印
			//猜测可能是收到上一次的消息时正在打印,在打印的流程中下一条返回进来了
			//然后把消息存到了ESP_buf中,但是轮到最后一行输出的时候,消息队列集又陷入了阻塞
			//所以最后一条进入不了流程
			//但是按这种想法的话,按道理每次在输出的时候,中断是会把ESP_buf中是的内容搞乱的
			//现在的情况又只是卡住最后一条消息
			//总的问题应该是发的快,取得慢 该如何解决??

			//实际上可能是换行符的问题转义字符
			if (strstr(ESP_buf, "OK") != NULL) {
				printf1("%s", ESP_buf);
				memset(ESP_buf, 0, sizeof(ESP_buf));
			} else if (strstr(ESP_buf, "Fail") != NULL) {
				printf1("%s", ESP_buf);
				memset(ESP_buf, 0, sizeof(ESP_buf));
			} else if (strstr(ESP_buf, "ERROR") != NULL) {
				printf1("%s", ESP_buf);
				memset(ESP_buf, 0, sizeof(ESP_buf));
			} else if (strstr(ESP_buf, "busy") != NULL) {
				printf1("%s", ESP_buf);
				memset(ESP_buf, 0, sizeof(ESP_buf));
			}

		}
	}

}


int main(void) {
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);

	USART1_Init();
	USART2_Init();

	queue1 = xQueueCreate(50, 1);
	queue2 = xQueueCreate(100, 1);

	// 5+5:表示创建一个最多可存储50+50个事件(所有队列最大值的总和)的队列集
	xQueueSet = xQueueCreateSet(50 + 150);

	// 将队列加入队列集
	xQueueAddToSet(queue1, xQueueSet);
	xQueueAddToSet(queue2, xQueueSet);

	xTaskCreate(task1, "Task1", 5 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	vTaskStartScheduler();

	while (1) {
	}
}


