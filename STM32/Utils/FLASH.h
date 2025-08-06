#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f10x.h"    
#include <stdbool.h>
#define ACCOUNT_ADRESS 0x0800F000 // 账号密码存储在这个地址

//把flash.c中的函数声明放在这里
void erasePage(uint32_t addr); // 擦除页
void changeFlashFor32(uint32_t addr, uint32_t data); // 写入32
void changeFlashFor16(uint32_t addr, uint16_t data); // 写入16
void readFlash(uint32_t addr, char *buffer, int len); // 读取数据
bool checkFlash(void); // 检查Flash是否被写入
void loadAccountFromFlash(char *wifi_account, char *wifi_passwd); // 从Flash加载账号密码    
void saveAccountToFlash(const char *wifi_account, const char *wifi_passwd); // 保存账号密码到Flash



#endif

