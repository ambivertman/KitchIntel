#include "Transmit_task.h"
#include "CollectData_task.h"
#include "Init_task.h"


//QueueSetHandle_t xQueueSet;
QueueHandle_t queue_esp01s, queue_hc05;
char buf[100] = { 0 };

void Init_task(void *arg) {

	if (Check_wifi_Connection() == false) {
		Init_wifi();
	}
	printf1("wifi already connected\r\n");
	// //创建数据传输任务和数据采集任务
	// Transmit_task_Init();
	// CollectData_task_Init();

	// //删除当前任务
	// vTaskDelete(NULL);
}


int main(void) {
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);

	USART1_Init();//用于pc串口工具
	USART2_Init();//用于esp01s
	USART3_Init();//用于hc05


	queue_hc05 = xQueueCreate(20, 1);
	queue_esp01s = xQueueCreate(100, 1);

	printf1("Queues created\r\n");
	xTaskCreate(Init_task, "Init_task", 2 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	vTaskStartScheduler();

	while (1) {
	}
}

