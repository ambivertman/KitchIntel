#ifndef __CollectData_task_h__
#define __CollectData_task_h__

#include "header.h"

extern QueueHandle_t queue_data;

void CollectData_task(void *arg);

#endif // __CollectData_task_h__
