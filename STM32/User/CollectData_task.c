#include "CollectData_task.h"

void CollectData_task(void *arg) {
    printf1("CollectData_task start\r\n");
    //数据采集任务的具体实现
    while (1) {
        char data_buf[30] = { 0 };
        printf1("Collecting data...\r\n");
        int light_value = AD_GetValue(ADC_Channel_1); // 假设使用ADC_Channel_1采集光照数据
        int temp_value = AD_GetValue(ADC_Channel_4); // 假设使用ADC_Channel_4采集温度数据
        sprintf(data_buf, "Light is %d\nTmp is %d\n", light_value, temp_value);
        xQueueSend(queue_data, &data_buf, portMAX_DELAY);
        printf1("CollectData_task end\r\n");
        vTaskDelay(50000);
    }
}

