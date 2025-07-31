#include "Delay.h"
volatile int Delay_done = 0;

void Delay_Init(void) {

    //开启外设时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    //初始化外设
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 1000;
    TIM_TimeBaseInitStruct.TIM_Prescaler = 71;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    //配置中断源
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    //初始化NVIC
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 11;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

    NVIC_Init(&NVIC_InitStruct);
}

//
void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        Delay_done = 1;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }

}

void Delay_ms(uint16_t ms) {
    while (ms--) {
        Delay_done = 0;
        TIM_Cmd(TIM2, ENABLE);  // 开启TIM2
        TIM_SetCounter(TIM2, 0);
        while (Delay_done != 1);
        TIM_Cmd(TIM2, DISABLE);
    }
}

