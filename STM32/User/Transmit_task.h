#ifndef __Transmit_task_h__
#define __Transmit_task_h__

#include "header.h"
extern char buf[100];

void Transmit_task(void *arg);
void Transmit_task_Init(void);
bool Connect_TCP(void);
void Enter_IO_Mode(void);
void Quit_IO_Mode(void);
void Disconnect_TCP(void);


#endif // __Transmit_task_h__

