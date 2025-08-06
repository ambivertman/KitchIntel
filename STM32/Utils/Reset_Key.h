#ifndef __RESET_KEY_H__
#define __RESET_KEY_H__
#include "stm32f10x.h"    
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FLASH.h"

// 定义按键事件枚举
typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SHORT_PRESS,
    KEY_EVENT_LONG_PRESS
} KeyEvent_t;

void KeyScan_task(void *arg);
void ResetKey_Init(void);

#endif

