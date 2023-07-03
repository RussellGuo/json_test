#include <stdio.h>
#include <stdint.h>
#include "hw_config.h"

void EXTI4_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line4);
	NVIC_ClearPendingIRQ(EXTI4_IRQn);
}

//蓝牙和mcu的连接引脚初始化
void bt_pin_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//< BT_RST
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOE, &GPIO_InitStruct);

	//< BT_WAKE_MCU
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
		
	//< MCU_WAKE_BT
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
	
		
	//< 下降沿触发中断
	EXTI_LineConfig(EXTI_Line4, EXTI_PinSource7, EXTI_Trigger_Falling);
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
}

void bt_power_on(void)
{
	GPIO_SetBits(GPIOE, GPIO_Pin_9);
}

void bt_power_off(void)
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_9);
}

void mcu_wake_bt(uint8_t state)
{
	if(state)
	{
		GPIO_SetBits(GPIOE, GPIO_Pin_8);
	}
	else
	{
		GPIO_ResetBits(GPIOE, GPIO_Pin_8);
	}
}



void hal_uart_set_baud (uint32_t baud)
{
    UART_InitTypeDef UART_InitStructure;
    UART_InitStructure.UART_BaudRate = baud;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;
    UART_Init(UART1, &UART_InitStructure);	
}

//初始化uart
void bt_uart_configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

	UART_FIFOInitTypeDef UART_FIFOInitStruct;
	
    //enable uart1 & dma
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART1, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART1, ENABLE);

    //UART1 GPIO PIN Remap
	GPIO_PinRemapConfig(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 , GPIO_Remap_3);
	
 	UART_StructInit(&UART_InitStructure);
	UART_FIFOStructInit(&UART_FIFOInitStruct);
    UART_InitStructure.UART_BaudRate = 115200;//115200;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;
	
	UART_FIFOInitStruct.FIFO_Enable = ENABLE;
    UART_FIFOInitStruct.FIFO_DMA_Mode = UART_FIFO_DMA_Mode_0;
    UART_FIFOInitStruct.FIFO_RX_Trigger = UART_FIFO_RX_Trigger_1_2_Full;
    UART_FIFOInitStruct.FIFO_TX_Trigger = UART_FIFO_TX_Trigger_1_4_Full;
    UART_FIFOInitStruct.FIFO_TX_TriggerIntEnable = DISABLE;

    UART_Init(UART1, &UART_InitStructure);	
    UART_FIFOInit(UART1, &UART_FIFOInitStruct);

    NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
	UART_ITConfig(UART1, UART_IT_RX_RECVD, ENABLE);
}

//初始化一个10ms的timer定时器
void bt_timer_configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_InitTypeDef TIM_InitStruct;
	SYSCTRL_ClocksTypeDef clocks;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	SYSCTRL_GetClocksFreq(&clocks);
	TIM_InitStruct.TIM_Period = (clocks.PCLK_Frequency/1000)*10;  // 10 ms
	TIM_InitStruct.TIMx = TIM_0;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	TIM_Cmd(TIMM0, TIM_InitStruct.TIMx, ENABLE);
}
