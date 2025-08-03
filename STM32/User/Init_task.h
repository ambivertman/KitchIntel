#ifndef __Init_task_h__
#define __Init_task_h__
#include "header.h"

extern char buf[100];

void Init_wifi(void);
bool Check_wifi_Connection(void);
void Get_Account(char *wifi_account, char *wifi_passwd);
bool Connect_wifi(char *wifi_account, char *wifi_passwd);
void Return_Connection_Status(char *wifi_account);

#endif 

