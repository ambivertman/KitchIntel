#include "Transmit_task.h"


void Transmit_task_Init(void) {
    xTaskCreate(Transmit_task, "Transmit_task", 2 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}

void Transmit_task(void *arg) {
    printf1("Transmit_task start\r\n");
    //取得二值信号量就建立连接发送数据
    Connect_TCP();
    Enter_IO_Mode();
    //Send_Data(buf);
    printf1("Sending data...\r\n");
    send_to_esp("Hello from STM32\r\n");
    Quit_IO_Mode();
    Disconnect_TCP();
    printf1("Transmit_task end\r\n");
}

bool Connect_TCP(void) {
    BaseType_t ret = pdFALSE;
    char data = 0;
    send_to_esp("AT+CIPSTART=\"TCP\",\"47.115.220.165\",9013\r\n");
    printf1("TCP command sended\r\n");
    while (1) {
        ret = xQueueReceive(queue_esp01s, &data, portMAX_DELAY);
        if (ret == pdTRUE) {
            buf[strlen(buf)] = data;
        }
        if (strstr(buf, "OK") != NULL) {
            printf1("esp_response:%s\r\n", buf);
            memset(buf, 0, strlen(buf));
            return true;
        }
    }
}

void Enter_IO_Mode(void) {
    BaseType_t ret = pdFALSE;
    char data = 0;
    //==================开启数据透传模式======================
    send_to_esp("AT+CIPMODE=1\r\n");
    printf1("enter CIP mode\r\n");
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
    //==================进入数据透传模式======================
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
}

void Quit_IO_Mode(void) {
    BaseType_t ret = pdFALSE;
    char data = 0;
    printf1("Quitting IO mode\r\n");
    vTaskDelay(500);
    printf1("delay done\r\n");


    //==================退出数据透传模式======================
    send_to_esp("+++");
    printf1("+++ command sended\r\n");
    vTaskDelay(1000);
    printf1("Quit IO Mode\r\n");


    //==================关闭数据透传模式======================
    send_to_esp("AT+CIPCLOSE\r\n");
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
    printf1("CIPCLOSE command sended\r\n");
}

void Disconnect_TCP(void) {
    BaseType_t ret = pdFALSE;
    char data = 0;
    send_to_esp("AT+CIPCLOSE\r\n");
    printf1("CIPCLOSE command sended\r\n");
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
    printf1("TCP connection closed\r\n");
}

