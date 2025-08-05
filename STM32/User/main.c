#include "Transmit_task.h"
#include "CollectData_task.h"
#include "Init_task.h"


//QueueSetHandle_t xQueueSet;
QueueHandle_t queue_esp01s, queue_hc05, queue_data;

void Init_task(void *arg) {
	vTaskDelay(1000);
	if (Check_wifi_Connection() == false) {
		Init_wifi();
	}
	printf1("wifi already connected\r\n");
	// //创建数据传输任务和数据采集任务
	xTaskCreate(CollectData_task, "CollectData_task", 1 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(Transmit_task, "Transmit_task", 4 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	//删除当前任务
	vTaskDelete(NULL);
}


int main(void) {
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);

	USART1_Init();//用于pc串口工具
	USART2_Init();//用于esp01s
	USART3_Init();//用于hc05
	Delay_Init();//初始化延时函数
	AD_Init(); // 初始化ADC


	queue_hc05 = xQueueCreate(20, 1);
	queue_esp01s = xQueueCreate(100, 1);
	queue_data = xQueueCreate(3, 30);

	printf1("Queues created\r\n");
	xTaskCreate(Init_task, "Init_task", 2 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	vTaskStartScheduler();

	while (1) {
	}
}

