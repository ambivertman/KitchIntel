#include "CollectData_task.h"

void CollectData_task(void *arg) {
    printf1("CollectData_task start\r\n");
    //数据采集任务的具体实现
    while (1) {
        char data_buf[30] = { 0 };
        printf1("Collecting data...\r\n");
        char light_data[15] = "\"Light\":123\n";
        char temp_data[15] = "\"Temp\":25\n";
        sprintf(data_buf, "%s%s", light_data, temp_data);
        xQueueSend(queue_data, &data_buf, portMAX_DELAY);
        memset(data_buf, 0, sizeof(data_buf));
        printf1("CollectData_task end\r\n");
        vTaskDelay(50000);
    }
}

