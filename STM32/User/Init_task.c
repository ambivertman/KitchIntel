#include "Init_task.h"


bool Check_wifi_Connection(void) {
    BaseType_t ret = pdFALSE;
    char data = 0;
    char buf[100] = { 0 };
    printf1("checking connection\r\n\r\n");
    send_to_esp("AT+CWJAP?\r\n");
    while (1) {
        ret = xQueueReceive(queue_esp01s, &data, portMAX_DELAY);
        if (ret == pdTRUE) {
            buf[strlen(buf)] = data;
        }
        if (strstr(buf, "No") != NULL && strstr(buf, "OK") != NULL) {
            printf1("wifi_response:%s\r\n", buf);
            return false;
        } else if (strstr(buf, "OK") != NULL) {
            printf1("wifi_response:%s\r\n", buf);
            return true;
        }
    }
}

void Init_wifi_bt(void) {
    char wifi_account[15] = { 0 };
    char wifi_passwd[15] = { 0 };

    Get_Account(wifi_account, wifi_passwd);
    while (1) {
        bool ret = Connect_wifi(wifi_account, wifi_passwd);
        if (ret == true) {
            break;
        } else {
            printf1("wrong account or password, please input again\r\n");
            memset(wifi_account, 0, strlen(wifi_account));
            memset(wifi_passwd, 0, strlen(wifi_passwd));
            Get_Account(wifi_account, wifi_passwd);
            continue;
        }

    }
    saveAccountToFlash(wifi_account, wifi_passwd);
    Return_Connection_Status(wifi_account);
}

void Get_Account(char *wifi_account, char *wifi_passwd) {
    printf1("Getting account\r\n\r\n");
    BaseType_t ret = pdFALSE;
    char data = 0;
    char buf[100] = { 0 };
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
    sscanf(buf, "!%[^=]=%[^!]!", wifi_account, wifi_passwd);
    printf1("Blue tooth task finished\r\n\r\n");
}

bool Connect_wifi(char *wifi_account, char *wifi_passwd) {
    printf1("Connecting wifi\r\n\r\n");
    BaseType_t ret = pdFALSE;
    char data = 0;
    char buf[100] = { 0 };
    //=========连接wifi=============
    //已获取账号密码, 开始向esp01s发送
    //拼接命令
    sprintf(buf, "AT+CWJAP_DEF=\"%s\",\"%s\"\r\n", wifi_account, wifi_passwd);
    send_to_esp(buf);
    memset(buf, 0, strlen(buf));
    //接收esp01s的回复
    while (1) {
        ret = xQueueReceive(queue_esp01s, &data, portMAX_DELAY);
        if (ret == pdTRUE) {
            buf[strlen(buf)] = data;
        }
        if (strstr(buf, "OK") != NULL) {
            printf1("wifi_response:%s\r\n\r\n", buf);
            memset(buf, 0, strlen(buf));
            return true;
        } else if (strstr(buf, "Fail") != NULL) {
            printf1("wifi_response:%s\r\n\r\n", buf);
            memset(buf, 0, strlen(buf));
            return false;
        }
    }
}

void Return_Connection_Status(char *wifi_account) {
    char buf[40] = { 0 };
    sprintf(buf, "{\"status\":0,\"wifi name\":\"%s\"}", wifi_account);
    send_to_hc05(buf);
}

void Init_wifi_flash(void) {
    char wifi_account[15] = { 0 };
    char wifi_passwd[15] = { 0 };
    if (checkFlash() == false) {
        printf1("Flash is empty, please input account and password via bluetooth\r\n");
        Init_wifi_bt();
    } else {
        loadAccountFromFlash(wifi_account, wifi_passwd);
        printf1("Account loaded from flash\r\n");
        if (Connect_wifi(wifi_account, wifi_passwd) == false) {
            printf1("Connect wifi failed, please input account and password via bluetooth\r\n");
            Init_wifi_bt();
        }
    }
    Return_Connection_Status(wifi_account);
}

