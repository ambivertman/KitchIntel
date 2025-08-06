#include "FLASH.h"

// addr: 页首地址
void erasePage(uint32_t addr) {
    FLASH_Unlock();					//解锁
    FLASH_ErasePage(addr);			//页擦除
    FLASH_Lock();					//加锁
}

// addr: 页首地址
// data: 存放在首地址的32位数据
void changeFlashFor32(uint32_t addr, uint32_t data) {
    FLASH_Unlock();							// 解锁
    FLASH_ProgramWord(addr, data);			// 32bits
    FLASH_Lock();							// 加锁
}

// addr: 页首地址
// data: 存放在首地址的16位数据
void changeFlashFor16(uint32_t addr, uint16_t data) {
    FLASH_Unlock();							// 解锁
    FLASH_ProgramHalfWord(addr, data);		// 16bits
    FLASH_Lock();							// 加锁
}

// 读取数据 读取len个字节的数据到buffer中
void readFlash(uint32_t addr, char *buffer, int len) {
    for (int i = 0; i < len; i++) {
        buffer[i] = *(__IO uint8_t *)(addr + i);
    }
}

//
bool checkFlash(void) {
    uint32_t addr = ACCOUNT_ADRESS; // 假设从Flash的起始地址开始检查
    uint32_t status = *((__IO uint32_t *)(addr));

    if (status == 0xFFFFFFFF) { // 检查是否为全1状态
        return false; // Flash未被写入
    } else {
        return true; // Flash已被写入
    }
}

void loadAccountFromFlash(char *wifi_account, char *wifi_passwd) {
    char buffer[30] = { 0 };
    readFlash(ACCOUNT_ADRESS + 4, buffer, sizeof(buffer) - 1); // 读取数据到buffer中
    sscanf(buffer, "!%[^=]=%[^!]!", wifi_account, wifi_passwd);
}

void saveAccountToFlash(const char *wifi_account, const char *wifi_passwd) {
    // 构造要写入的数据格式
    char buffer[30] = { 0 };
    snprintf(buffer, sizeof(buffer), "!%s=%s!", wifi_account, wifi_passwd);

    // 计算需要写入的字节数
    int len = strlen(buffer);

    // 擦除Flash页
    erasePage(ACCOUNT_ADRESS);

    //用长度标记是否使用
    changeFlashFor32(ACCOUNT_ADRESS, len);

    // 写入数据到Flash
    for (int i = 0; i < len; i += 4) {
        changeFlashFor32(ACCOUNT_ADRESS + 4 + i, *(int *)(buffer + i));
    }
}