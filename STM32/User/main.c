#include "stm32f10x.h"    
#include "freertos.h"
#include "task.h"
#include "queue.h"
#include "UART1.h" 
#include "UART2.h" 
#include "UART3.h" 
#include <string.h>

//QueueSetHandle_t xQueueSet;
QueueHandle_t queue_usart, queue_esp01s, queue_hc05;

void task1(void *arg) {
	printf1("task1 start\r\n");
	char buf[100] = { 0 };
	char data = 0;
	BaseType_t ret = pdFALSE;
	//==========连接蓝牙获取wifi====
	int flag_count = 0;
	while (1) {
		//wifi密码从经由HC05又通过USART3传入,在这里逐字节组装
		printf1("1");
		ret = xQueueReceive(queue_hc05, &data, portMAX_DELAY);
		if (ret == pdTRUE) {
			buf[strlen(buf)] = data;
			if (data == '!') {
				flag_count++;
			}
			if (flag_count == 2) {
				break;
			}
		}
	}
	printf1("Blue teeth start\r\n");
	char wifi_account[10] = { 0 };
	char wifi_passwd[10] = { 0 };
	sscanf(buf, "!%[^=]=%[^!]!", wifi_account, wifi_passwd);
	memset(buf, 0, strlen(buf));


	//=========连接wifi=============
	//已获取账号密码, 开始向esp01s发送
	//拼接命令

	// sprintf(buf, "+CWJAP_DEF=\"%s\",\"%s\"\r\n", wifi_account, wifi_passwd);
	// printf1("wifi_buf:%s");
	// send_to_esp(buf);
	// memset(buf, 0, strlen(buf));
	// //接收esp01s的回复
	// while (1) {
	// 	ret = xQueueReceive(queue_esp01s, &data, portMAX_DELAY);
	// 	if (ret == pdTRUE) {
	// 		buf[strlen(buf)] = data;

	// 	}
	// 	if (strstr(buf, "OK") != NULL) {
	// 		printf("wifi_response:%s\r\n", buf);
	// 		memset(buf, 0, strlen(buf));
	// 		break;
	// 	}
	// }

	//=====================进行TCP连接
	//拼接TCP连接指令
	// send_to_esp("AT+CIPSTART=\"TCP\",\"47.115.220.165\",9013");
	// while (1) {
	// 	ret = xQueueReceive(queue_esp01s, &data, portMAX_DELAY);
	// 	if (ret == pdTRUE) {
	// 		buf[strlen(buf)] = data;

	// 	}
	// 	if (strstr(buf, "OK") != NULL) {
	// 		printf("wifi_response:%s\r\n", buf);
	// 		memset(buf, 0, strlen(buf));
	// 		break;
	// 	}
	// }


}



int main(void) {
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);

	USART1_Init();//用于pc串口工具
	USART2_Init();//用于esp01s
	USART2_Init();//用于hc05


	queue_hc05 = xQueueCreate(20, 1);
	queue_usart = xQueueCreate(70, 1);
	queue_esp01s = xQueueCreate(100, 1);

	// // 5+5:表示创建一个最多可存储50+50个事件(所有队列最大值的总和)的队列集
	// xQueueSet = xQueueCreateSet(50 + 150);

	// // 将队列加入队列集
	// xQueueAddToSet(queue1, xQueueSet);
	// xQueueAddToSet(queue2, xQueueSet);
	printf1("Queues created\r\n");
	xTaskCreate(task1, "Task1", 5 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	vTaskStartScheduler();

	while (1) {
	}
}


