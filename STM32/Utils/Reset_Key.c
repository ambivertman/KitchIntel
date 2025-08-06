#include "Reset_Key.h"
// 按键引脚定义
#define KEY_GPIO_PORT       GPIOB
#define KEY_GPIO_PIN        GPIO_Pin_6
#define KEY_PRESS_LEVEL     0
// 此函数与之前的示例完全相同
void ResetKey_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);
}



// 长按时间阈值 (单位：毫秒)
#define LONG_PRESS_MS       1000

// 按键扫描任务
void KeyScan_task(void *arg) {
    // 按键的状态
    typedef enum {
        STATE_IDLE,          // 空闲状态
        STATE_DEBOUNCE,      // 消抖状态
        STATE_PRESSED,       // 已按下状态
        STATE_LONG_PRESS_HOLD // 长按保持状态
    } KeyState_t;

    static KeyState_t s_key_state = STATE_IDLE;
    TickType_t press_start_time = 0;

    while (1) { // 任务主循环
        // 读取当前按键电平
        uint8_t key_is_pressed = (GPIO_ReadInputDataBit(KEY_GPIO_PORT, KEY_GPIO_PIN) == KEY_PRESS_LEVEL);

        switch (s_key_state) {
        case STATE_IDLE:
            if (key_is_pressed) {
                // 按键被按下，进入消抖状态
                s_key_state = STATE_DEBOUNCE;
            }
            break;

        case STATE_DEBOUNCE:
            if (key_is_pressed) {
                // 消抖完成，确认按下。记录当前时间，进入“已按下”状态
                press_start_time = xTaskGetTickCount();
                s_key_state = STATE_PRESSED;
            } else {
                // 抖动，返回空闲
                s_key_state = STATE_IDLE;
            }
            break;

        case STATE_PRESSED:
            if (key_is_pressed) {
                // 保持按下状态，检查是否达到长按时间
                if ((xTaskGetTickCount() - press_start_time) >= pdMS_TO_TICKS(LONG_PRESS_MS)) {
                    s_key_state = STATE_LONG_PRESS_HOLD;
                    //擦除flash
                    erasePage(ACCOUNT_ADRESS);
                    //重置
                    NVIC_SystemReset();
                }
            }
            break;

        case STATE_LONG_PRESS_HOLD:
            if (!key_is_pressed) {
                // 长按后松开，返回空闲状态
                s_key_state = STATE_IDLE;
            }
            // 在此状态不重复发送事件
            break;
        }

        // 每 20ms 扫描一次按键，这个延时也提供了消抖功能
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}


