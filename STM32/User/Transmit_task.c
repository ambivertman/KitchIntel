#include "Transmit_task.h"


void Transmit_task_Init(void) {
    xTaskCreate(Transmit_task, "Transmit_task", 2 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}

void Transmit_task(void *arg) {
    printf1("Transmit_task start\r\n");
    //取得二值信号量就建立连接发送数据
    while (1) {
        char data_buf[30];
        if (xQueueReceive(queue_data, data_buf, portMAX_DELAY) == pdTRUE) {
            if (Connect_TCP() == false) {
                Init_wifi();
                continue;
            }
            vTaskDelay(200); // 等待ESP响应
            Enter_IO_Mode();
            vTaskDelay(200);
            printf1("Sending data...\r\n");
            printf1("%s", data_buf);
            Send_Data(data_buf);
            //send_to_esp(data_buf);
            memset(data_buf, 0, strlen(data_buf));
            vTaskDelay(200);
            Quit_IO_Mode();
            vTaskDelay(200);
            Disconnect_TCP();
            printf1("Transmit_task end\r\n");
        }
    }

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
        if (strstr(buf, "No") != NULL) {
            printf1("esp_response:%s\r\n", buf);
            memset(buf, 0, strlen(buf));
            return false;
        } else if (strstr(buf, "OK") != NULL) {
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
    vTaskDelay(1000); // 等待ESP响应
    printf1("delay done\r\n");


    //==================退出数据透传模式======================
    send_to_esp("+++");
    printf1("+++ command sended\r\n");
    vTaskDelay(2000); // 等待ESP响应
    printf1("Quit IO Mode\r\n");


    //==================关闭数据透传模式======================
    send_to_esp("AT+CIPMODE=0\r\n");
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
    printf1("CIP mdoe quit\r\n");
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

void Send_Data(char *data_buf) {
    char *HTTP_Header = "POST/HTTP/1.0\n\n";
    char HTTP_buf[60] = { 0 };
    sprintf(HTTP_buf, "%s%s", HTTP_Header, data_buf);
    send_to_esp(HTTP_buf);
}

