#include "stm32f10x.h"    
#include "freertos.h"
#include "task.h"
#include "queue.h"
#include "UART1.h" 
#include "UART2.h" 
#include "UART3.h" 
#include <string.h>

QueueHandle_t queue;

int flag_count = 0;
void task1(void *arg) {
	char buf[30] = { 0 };
	char data = 0;
	while (1) {
		//wifi密码从经由HC05又通过USART3传入,在这里逐字节组装
		BaseType_t ret = xQueueReceive(queue, &data, portMAX_DELAY);
		if (ret == pdTRUE) {
			buf[strlen(buf)] = data;
			if (data == '!') {
				flag_count++;
			}
		}
		//需要有一个结束的标记
		char wifi_account[10] = { 0 };
		char wifi_passwd[10] = { 0 };
		if (flag_count == 2) {
			printf1("succeed\r\n");
			sscanf(buf, "!%[^=]=%[^!]!", wifi_account, wifi_passwd);
			printf1("%s:%s\r\n", wifi_account, wifi_passwd);
			flag_count = 0;
		}
	}
}

int main(void) {
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);

	USART1_Init();
	USART3_Init();

	queue = xQueueCreate(5, 1);

	xTaskCreate(task1, "Task1", 4 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	vTaskStartScheduler();

	while (1) {
	}
}


