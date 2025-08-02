#include "stm32f10x.h"    
#include "freertos.h"
#include "task.h"
#include "queue.h"
#include "UART1.h" 
#include "UART2.h" 
#include "UART3.h" 
#include <string.h>
void Transmit_task(void *arg);
void Init_wifi(void);


//QueueSetHandle_t xQueueSet;
QueueHandle_t queue_usart, queue_esp01s, queue_hc05;
char buf[100] = { 0 };

void Init_task(void *arg) {
	send_to_esp("+++");
	vTaskDelay(3000);
	printf1("Init_task start\r\n");
	char data = 0;
	BaseType_t ret = pdFALSE;
	//===========判断wifi是否已经连接========
	int is_connected = 0;
	send_to_esp("AT+CWJAP?\r\n");
	while (1) {
		ret = xQueueReceive(queue_esp01s, &data, portMAX_DELAY);
		if (ret == pdTRUE) {
			buf[strlen(buf)] = data;
		}
		if (strstr(buf, "No") != NULL && strstr(buf, "OK") != NULL) {
			printf1("wifi_response:%s\r\n", buf);
			memset(buf, 0, strlen(buf));
			is_connected = 0;
			break;
		} else if (strstr(buf, "OK") != NULL) {
			printf1("wifi_response:%s\r\n", buf);
			memset(buf, 0, strlen(buf));
			is_connected = 1;
			break;
		}
	}
	if (is_connected == 0) {
		Init_wifi();
	}
	//=================创建数据传输任务=================
	xTaskCreate(Transmit_task, "Transmit_task", 2 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	//=================删除初始化任务=================
	vTaskDelete(NULL);
}

void Transmit_task(void *arg) {
	char data = 0;
	BaseType_t ret = pdFALSE;
	printf1("Transmit_task start\r\n");
	//=====================进行TCP连接=======================
	//拼接TCP连接指令
	send_to_esp("AT+CIPSTART=\"TCP\",\"124.222.249.185\",9006\r\n");
	printf1("TCP command sended\r\n");
	while (1) {
		ret = xQueueReceive(queue_esp01s, &data, portMAX_DELAY);
		if (ret == pdTRUE) {
			buf[strlen(buf)] = data;

		}
		if (strstr(buf, "OK") != NULL) {
			printf1("esp_response:%s\r\n", buf);
			memset(buf, 0, strlen(buf));
			break;
		}

	}

	//==================进入数据透传模式======================
	send_to_esp("AT+CIPMODE=1\r\n");
	printf1("CIP command sended\r\n");
	while (1) {
		ret = xQueueReceive(queue_esp01s, &data, portMAX_DELAY);
		if (ret == pdTRUE) {
			buf[strlen(buf)] = data;

		}
		if (strstr(buf, "OK") != NULL) {
			printf1("wifi_response:%s\r\n", buf);
			memset(buf, 0, strlen(buf));
			break;
		}
	}


	//===============发送数据======================
	while (1) {
		send_to_esp("AT+CIPSEND\r\n");
		printf1("CIPSEND command sended\r\n");
		while (1) {
			ret = xQueueReceive(queue_esp01s, &data, portMAX_DELAY);
			if (ret == pdTRUE) {
				buf[strlen(buf)] = data;

			}
			if (strstr(buf, "OK") != NULL) {
				printf1("wifi_response:%s\r\n", buf);
				memset(buf, 0, strlen(buf));
				break;
			}
		}
		send_to_esp("Hihi\n");
		vTaskDelay(3000);
	}
}



int main(void) {
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);

	USART1_Init();//用于pc串口工具
	USART2_Init();//用于esp01s
	USART3_Init();//用于hc05


	queue_hc05 = xQueueCreate(20, 1);
	queue_usart = xQueueCreate(70, 1);
	queue_esp01s = xQueueCreate(100, 1);

	printf1("Queues created\r\n");
	xTaskCreate(Init_task, "Init_task", 3 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	vTaskStartScheduler();

	while (1) {
	}
}

void Init_wifi(void) {
	char data = 0;
	BaseType_t ret = pdFALSE;
	//==========连接蓝牙获取wifi账号密码====
	int flag_count = 0;
	while (1) {
		//wifi密码从经由HC05又通过USART3传入,在这里逐字节组装
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
	char wifi_account[10] = { 0 };
	char wifi_passwd[10] = { 0 };
	sscanf(buf, "!%[^=]=%[^!]!", wifi_account, wifi_passwd);
	printf1("%s\r\n", buf);
	memset(buf, 0, strlen(buf));
	printf1("Blue tooth task finished\r\n");


	//=========连接wifi=============
	//已获取账号密码, 开始向esp01s发送
	//拼接命令

	sprintf(buf, "AT+CWJAP_DEF=\"%s\",\"%s\"\r\n", wifi_account, wifi_passwd);
	printf1("wifi_buf:%s", buf);
	send_to_esp(buf);
	memset(buf, 0, strlen(buf));
	//接收esp01s的回复
	while (1) {
		ret = xQueueReceive(queue_esp01s, &data, portMAX_DELAY);
		if (ret == pdTRUE) {
			buf[strlen(buf)] = data;
		}
		if (strstr(buf, "OK") != NULL) {
			printf1("wifi_response:%s\r\n", buf);
			memset(buf, 0, strlen(buf));
			break;
		}
	}
	//=============连接成功后使用蓝牙通知手机app=============
	sprintf(buf, "{\"status\":0,\"wifi name\":\"%s\"}", wifi_account);
	printf1("send to hc05 buf:%s\r\n", buf);
	send_to_hc05(buf);
	memset(buf, 0, strlen(buf));
	printf1("hc05 buf sended\r\n");
}
