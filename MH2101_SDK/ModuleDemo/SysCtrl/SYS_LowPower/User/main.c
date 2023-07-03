#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

typedef enum
{
	MENU_SEL_MAIN = 0,
	MENU_SEL_CPU_SLEEP,
	MENU_SEL_SLEEP,
	MENU_SEL_POWER_DOWN
}MENU_SEL;

void PrintMenu(MENU_SEL menu);
uint8_t GetCmd(void);

void UART_Configuration(void);
void SENSOR_Configuration(void);
void LOWPOWER_Configuration(void);

void CPU_Sleep_Test(void);
void Sleep_Test(void);
void PowerDown_Test(void);

void RTC_WakeUpTest(SleepMode_TypeDef sleepMode);
void Timer_CPUWakeUpTest(void);
void GPIO_WakeUpTest(SleepMode_TypeDef sleepMode);
void SENSOR_WakeUpTest(SleepMode_TypeDef sleepMode);

void ExtPins_Handle(void);

void CPUSleep_Prepare(void);
void CPUSleep_Recover(void);

void Sleep_Prepare(void);
void Sleep_Recover(void);

void PowerdownSleep_Prepare(void);
void PowerdownSleep_Recover(void);

void delay(void)
{
	uint32_t i = 0x2FFFFFF;
	while(i--);
}

int main(void)
{	
    SYSCTRL_PLLConfig(SYSCTRL_PLL_204MHz);
    SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
    SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
    SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);
//    QSPI_SetLatency(0);
    
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_ALL, ENABLE);
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_ALL, ENABLE);   
    
    UART_Configuration();
	
	printf("\nMegaHunt SCPU LowPower Demo V1.0.\n");

	PowerdownSleep_Recover();//POWERDOWN唤醒之后程序重新执行，但是寄存器值不会复位，需要调用此函数根据需求修改配置

	
	LOWPOWER_Configuration();
	while (1)
	{
		
	}
}

void PrintMenu(MENU_SEL menu)
{
	switch((uint32_t)menu)
	{
	case (uint32_t)MENU_SEL_MAIN: 
		printf("===============MAIN MENU================\n");
		printf("1 CPU Sleep Test\n");
		printf("2 Sleep Test\n");
		printf("3 Powerdown Test\n");
		printf("p Print Menu\n");
		printf("================MENU END================\n");
		break;
	
	case (uint32_t)MENU_SEL_CPU_SLEEP:
		printf("===============CPU SLEEP MENU================\n");
		printf("1 RTC WakeUp Test\n");
		printf("2 Timer WakeUp CPU Sleep Test\n");
		printf("3 Sensor WakeUp CPU Sleep Test\n");
		printf("4 GPIO(PC15) WakeUp CPU Sleep Test\n");
		printf("b Back\n");
		printf("p Print Menu\n");
		printf("===================MENU END==================\n");	
		break;
	
	case (uint32_t)MENU_SEL_SLEEP:
		printf("===============DEEP SLEEP MENU================\n");
		printf("1 RTC WakeUp Sleep Test\n");
		printf("2 Sensor WakeUp Sleep Test\n");
		printf("3 GPIO(PC15) WakeUp Sleep Test\n");
		printf("b Back\n");
		printf("p Print Menu\n");
		printf("===================MENU END===================\n");		
		break;
	
	case (uint32_t)MENU_SEL_POWER_DOWN:
		printf("===============DEEP SLEEP MENU================\n");
		printf("1 Sensor WakeUp Power Down Test\n");
	#ifdef BGA_121
		printf("2 GPIO(PF6) WakeUp Power Down Test\n");
	#endif
		printf("b Back\n");
		printf("p Print Menu\n");
		printf("===================MENU END===================\n");		
		break;
	
	default:
		break;
	}
}

uint8_t GetCmd(void)
{
	uint8_t tmp = 0;

	if (UART_GetLineStatus(UART0) & UART_LINE_STATUS_RX_RECVD)
	{
		tmp = UART_ReceiveData(UART0);
	}
	
	if (tmp)
	{
		printf("cmd is %c\n", tmp);
	}

	return tmp;
}

void NVIC_Configuration(uint32_t IRQChannel, FunctionalState NewState)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = IRQChannel;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = NewState;
	NVIC_Init(&NVIC_InitStructure);	
}

typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t Pin_Num;
    uint32_t Pin_status;
}TypeRampPinDef;

#define PIN_STATUS_HIGH     1
#define PIN_STATUS_LOW      0

TypeRampPinDef SpecialPin_List[] = 
{
	//SPI FLASH CS
    {GPIOB, GPIO_Pin_3, PIN_STATUS_HIGH},
	
    //SCI0_DET
    {GPIOA, GPIO_Pin_6, PIN_STATUS_LOW},
    
    //NFC_CS
    {GPIOB, GPIO_Pin_3, PIN_STATUS_HIGH},
    
    //SPI
    {GPIOB, GPIO_Pin_2, PIN_STATUS_LOW},
    {GPIOB, GPIO_Pin_4, PIN_STATUS_LOW},
    {GPIOB, GPIO_Pin_5, PIN_STATUS_LOW},
    
    //DAC
    {GPIOC, GPIO_Pin_1, PIN_STATUS_LOW},
    
    //CM_PD
    {GPIOC, GPIO_Pin_6, PIN_STATUS_HIGH},
    
    //LCD_CS
    {GPIOC, GPIO_Pin_14, PIN_STATUS_HIGH},
    
    //JTAG
    {GPIOC, GPIO_Pin_4, PIN_STATUS_LOW},
	
	
};

void ExtPins_Handle(void)
{
    uint32_t i;
	
	GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
    
    for (i = 0; i < (sizeof(SpecialPin_List)/sizeof(SpecialPin_List[0])); i++)
    {
        GPIO_InitStruct.GPIO_Pin = SpecialPin_List[i].Pin_Num;
        GPIO_Init(SpecialPin_List[i].GPIOx, &GPIO_InitStruct);
        
        if (SpecialPin_List[i].Pin_status == PIN_STATUS_LOW)
        {
            GPIO_ResetBits(SpecialPin_List[i].GPIOx, SpecialPin_List[i].Pin_Num);
        }
        if (SpecialPin_List[i].Pin_status == PIN_STATUS_HIGH)
        {
            GPIO_SetBits(SpecialPin_List[i].GPIOx, SpecialPin_List[i].Pin_Num);
        }
    }		
}

void CPUSleep_Prepare(void)
{
    uint32_t delay, u32Count;	
	
    /****************** for MSR ******************/		
    SYSCTRL->MSR_CR1 |= BIT(27);
    for (u32Count = 0; u32Count < 0x2000; u32Count++);

    /****************** for SCI ******************/			
    SYSCTRL->PHER_CTRL |= BIT(20);	
	
	/****************** for usb ******************/
	if((SYSCTRL->USBPHY_CR1&(1<<0))!= 0x01)
	SYSCTRL->USBPHY_CR1 |= 0x01;
	
    delay = 0xAFFFFF;
    while (delay--);	
	
	printf("ENTER CPU SLEEP\n");
}

void CPUSleep_Recover(void)
{
    /****************** for MSR ******************/			
    SYSCTRL->MSR_CR1 &= ~BIT(27);
 
	/****************** for usb ******************/
	SYSCTRL->USBPHY_CR1 &= (~(1<<0));
	
    /****************** for SCI ******************/			
    SYSCTRL->PHER_CTRL &= ~BIT(20);
	printf("EXIT CPU SLEEP\n");
}

void Sleep_Prepare(void)
{
    uint32_t delay, u32Count;	

	ExtPins_Handle();
	
    /****************** for MSR ******************/	
    SYSCTRL->MSR_CR1 |= BIT(27);
    for (u32Count = 0; u32Count < 0x2000; u32Count++);
	
	/****************** for SCI*****************/		
    SYSCTRL->PHER_CTRL |= BIT(20);
	/*************** for sensor **************/		
	SENSOR_ClearITPendingBit();
	
	ADC_StartCmd(DISABLE);
	ADC0->ADC_CR2 &= ~BIT(10);
	DAC_Cmd(DISABLE);
	
    /****************** for usb ******************/
	if((SYSCTRL->USBPHY_CR1&(1<<0))!= 0x01)
	SYSCTRL->USBPHY_CR1 |= 0x01;
	
	delay = 0xAFFFFF;
	while(delay--);
	
	/************** power down LDO25 *************/	
    SYSCTRL->LDO25_CR |= BIT(8) | BIT(9);
	
	//关闭所有唤醒源
    GPIO->WAKE_TYPE_EN &= ~(0x3F<<11);
    GPIO->WAKE_P0_EN = 0;
	GPIO->WAKE_P1_EN = 0;
	GPIO->WAKE_P2_EN = 0;
	
	printf("ENTER SLEEP\n");
}

void Sleep_Recover(void)
{
	
	/************** power down LDO25 *************/		
    SYSCTRL->LDO25_CR &= ~(BIT(8) | BIT(9));
	
	/****************** for MSR ******************/	
    SYSCTRL->MSR_CR1 &= ~BIT(27);
	
	/****************** for SCI*****************/	
	SYSCTRL->PHER_CTRL &=  ~BIT(20);

    /****************** for usb ******************/
	SYSCTRL->USBPHY_CR1 &= (~(1<<0));
}


void PowerdownSleep_Prepare(void)
{
    uint32_t  u32Count, delay;
	ExtPins_Handle();
    
    /****************** for MSR ******************/	
    SYSCTRL->MSR_CR1 |= BIT(27);
    for (u32Count = 0; u32Count < 0x2000; u32Count++);
	
    /****************** for SCI0 *****************/		
    SYSCTRL->PHER_CTRL |= BIT(20) | BIT(22);	
	
	/*************** for sensor **************/		
	SENSOR_ClearITPendingBit();
	
    /****************** for usb ******************/
	if((SYSCTRL->USBPHY_CR1&(1<<0))!= 0x01)
	SYSCTRL->USBPHY_CR1 |= 0x01;
	
	delay = 0xAFFFFF;
	while(delay--);
	
	ADC_StartCmd(DISABLE);
	ADC0->ADC_CR2 &= ~BIT(10);
	DAC_Cmd(DISABLE);
	
	GPIO_PullUpCmd(GPIOC, GPIO_Pin_0, DISABLE);
	GPIO_PullUpCmd(GPIOD, GPIO_Pin_0, DISABLE);
	
	
	/************** power down LDO25 *************/	
    SYSCTRL->LDO25_CR |= BIT(8) | BIT(9);
    
    //关闭所有唤醒源
    SYSCTRL->RST_FLAG &= ~(0x1F<<8);
}
void PowerdownSleep_Recover(void)
{
	//清除中断
	SENSOR_ClearITPendingBit();
	NVIC_ClearPendingIRQ(SENSOR_IRQn);
	
	/************** power down LDO25 *************/		
    SYSCTRL->LDO25_CR &= ~(BIT(8) | BIT(9));
	
	/****************** for MSR ******************/	
    SYSCTRL->MSR_CR1 &= ~BIT(27);
	
	/****************** for SCI*****************/	
	SYSCTRL->PHER_CTRL &=  ~(BIT(20)|BIT(22));

    /****************** for usb ******************/
	SYSCTRL->USBPHY_CR1 &= (~(1<<0));
    
}
void SENSOR_Configuration(void)
{   
	SENSOR_EXTInitTypeDef SENSOR_EXTInitStruct;
	memset(&SENSOR_EXTInitStruct, 0, sizeof(SENSOR_EXTInitStruct));

    SENSOR_EXTInitStruct.SENSOR_Port_Static = SENSOR_Port_S0 | SENSOR_Port_S1 | SENSOR_Port_S2 | SENSOR_Port_S3 | SENSOR_Port_S4 | SENSOR_Port_S5;
    SENSOR_EXTInitStruct.SENSOR_Port_Pull = SENSOR_Port_All;
	SENSOR_EXTInitStruct.SENSOR_GlitchEnable = ENABLE;
    SENSOR_EXTInitStruct.SENSOR_Static_Sample = SENSOR_DYNAMIC_SAMPLE_2;		

    SENSOR_EXTInitStruct.SENSOR_Port_Enable = ENABLE;	

    SENSOR_ClearITPendingBit();
	SENSOR_EXTCmd(DISABLE);
	while(SET == SENSOR_EXTIsRuning());
    
	SENSOR_EXTInit(&SENSOR_EXTInitStruct);
  	SENSOR_AttackRespMode(SENSOR_Interrupt);
	SENSOR_ANACmd(SENSOR_ANA_ALL,DISABLE);
	SENSOR_EXTCmd(ENABLE);
}


void LOWPOWER_Configuration(void)
{
	volatile uint8_t cmd = 0;
	
	PrintMenu(MENU_SEL_MAIN);
	while (1)
	{
		cmd = GetCmd();
		
		switch (cmd)
		{
		case '1':
			CPU_Sleep_Test();
			PrintMenu(MENU_SEL_MAIN);
			break;
		
		case '2':
			Sleep_Test();
			PrintMenu(MENU_SEL_MAIN);
			break;
		
		case '3':
			PowerDown_Test();
			PrintMenu(MENU_SEL_MAIN);
			break;
		
		case 'p':
			PrintMenu(MENU_SEL_MAIN);
			break;
		
		default:
			break;
		}
	}
}

void CPU_Sleep_Test(void)
{
	uint8_t cmd = 0;

	PrintMenu(MENU_SEL_CPU_SLEEP);
	while (1)
	{
		cmd = GetCmd();
		
		switch (cmd)
		{
		case '1':
			CPUSleep_Prepare();
			RTC_WakeUpTest(SleepMode_CpuOff);
			CPUSleep_Recover();
			PrintMenu(MENU_SEL_CPU_SLEEP);		
			break;
		
		case '2':
			CPUSleep_Prepare();
			Timer_CPUWakeUpTest();
			CPUSleep_Recover();
			PrintMenu(MENU_SEL_CPU_SLEEP);
			break;
		
		case '3':
			CPUSleep_Prepare();
			SENSOR_WakeUpTest(SleepMode_CpuOff);
			CPUSleep_Recover();
			PrintMenu(MENU_SEL_CPU_SLEEP);	
			break;
		
		case '4':
			CPUSleep_Prepare();
			GPIO_WakeUpTest(SleepMode_CpuOff);
			CPUSleep_Recover();
			PrintMenu(MENU_SEL_CPU_SLEEP);
			break;
		
		case 'b':
			return;
		
		case 'p':
			PrintMenu(MENU_SEL_CPU_SLEEP);
			break;	
		
		default:
			break;	
		}
	}
}

void Sleep_Test(void)
{  
	volatile uint8_t cmd = 0;

	PrintMenu(MENU_SEL_SLEEP);
	while (1)
	{
		cmd = GetCmd();
		
		switch (cmd)
		{
			case '1':
				Sleep_Prepare();
				RTC_WakeUpTest(SleepMode_Sleep);
				Sleep_Recover();
				PrintMenu(MENU_SEL_SLEEP);		
				break;
		
			case '2':
				Sleep_Prepare();
				SENSOR_WakeUpTest(SleepMode_Sleep);
				Sleep_Recover();
				PrintMenu(MENU_SEL_SLEEP);		
				break;
		
			case '3':
				Sleep_Prepare();
				GPIO_WakeUpTest(SleepMode_Sleep);
				Sleep_Recover();
				PrintMenu(MENU_SEL_SLEEP);
				break;
		
			case 'b':
				return;		
			
			case 'p':
				PrintMenu(MENU_SEL_SLEEP);
				break;	
			
			default:
				break;	
		}
	}
}


void PowerDown_Test(void)
{
	volatile uint8_t cmd = 0;

	PrintMenu(MENU_SEL_POWER_DOWN);
	while (1)
	{
		cmd = GetCmd();
		
		switch (cmd)
		{
	
		case '1':
			PowerdownSleep_Prepare();
			SENSOR_WakeUpTest(SleepMode_PowerDown);
			PrintMenu(MENU_SEL_POWER_DOWN);		
			break;
		#ifdef BGA_121
		case '2':
			PowerdownSleep_Prepare();
			GPIO_WakeUpTest(SleepMode_PowerDown);
			PrintMenu(MENU_SEL_POWER_DOWN);
			break;
		#endif
		
		case 'b':
			return;		
		
		case 'p':
			PrintMenu(MENU_SEL_POWER_DOWN);
			break;	
		
		default:
			break;	
		}
	}
}
void RTC_WakeUpTest(SleepMode_TypeDef sleepMode)
{
	NVIC_Configuration(RTC_IRQn, ENABLE);
	
	RTC_ResetCounter();     //rtc stop counting and counter cleared to 0
	RTC_SetRefRegister(0);
	RTC_SetAlarm(5);
	RTC_ITConfig(ENABLE);
	RTC_ClearITPendingBit();
	printf("RTC will trigger the interrput after 5s\n");	
    if (SleepMode_CpuOff == sleepMode)
    {
		SYSCTRL->CG_CTRL2 = 0;						//关闭所有门控时钟，只使能BPU
        SYSCTRL->CG_CTRL1 = SYSCTRL_APBPeriph_BPU; 
	
    }
    else if (SleepMode_Sleep == sleepMode)
    {
		SYSCRTRL_WakeUpCmd(SYSCTRL_WakeMode_RTC, ENABLE);
    }
	SYSCTRL_EnterSleep(sleepMode);
	
	if(SleepMode_CpuOff == sleepMode)
	{
		SYSCTRL->CG_CTRL2 = SYSCTRL_AHBPeriph_ALL;   
		SYSCTRL->CG_CTRL1 = SYSCTRL_APBPeriph_ALL;
		
		GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
	}
	NVIC_Configuration(RTC_IRQn, DISABLE);
	printf("RTC Wakeup SleepMode Out\n\n");	
}

void Timer_CPUWakeUpTest(void)
{
	TIM_InitTypeDef TIM_InitStructure;
	SYSCTRL_ClocksTypeDef c;

	SYSCTRL_GetClocksFreq(&c);

	TIM_InitStructure.TIMx = TIM_0;
	TIM_InitStructure.TIM_Period = c.PCLK_Frequency * 5;

	NVIC_ClearPendingIRQ(TIM0_0_IRQn);
	
	TIM_Init(TIMM0, &TIM_InitStructure);
	TIM_ITConfig(TIMM0, TIM_0, ENABLE);
	TIM_Cmd(TIMM0, TIM_0, ENABLE);
	NVIC_Configuration(TIM0_0_IRQn,ENABLE);
	
	SYSCTRL->CG_CTRL2 = 0;
    SYSCTRL->CG_CTRL1 = SYSCTRL_APBPeriph_TIMM0;  
	
	printf("SleepMode_CPUOff In\n");
	printf("Timers will trigger the interrput after 5s\n");
	SYSCTRL_EnterSleep(SleepMode_CpuOff);
	
	SYSCTRL->CG_CTRL2 = SYSCTRL_AHBPeriph_ALL;   
    SYSCTRL->CG_CTRL1 = SYSCTRL_APBPeriph_ALL;   
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);

	NVIC_Configuration(TIM0_0_IRQn, DISABLE);
	TIM_Cmd(TIMM0, TIM_0, DISABLE);	
	printf("SleepMode_CPUOff Out\n\n");
}

void SENSOR_WakeUpTest(SleepMode_TypeDef sleepMode)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	SENSOR_Configuration();
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	//清除中断
	SENSOR_ClearITPendingBit();
	NVIC_ClearPendingIRQ(SENSOR_IRQn);
	
	NVIC_Configuration(SENSOR_IRQn,ENABLE);
	printf("Please attack any pin of Tamper.\n");
	if (SleepMode_CpuOff == sleepMode)
    {
		SYSCTRL->CG_CTRL2 = 0;
		SYSCTRL->CG_CTRL1 = SYSCTRL_APBPeriph_BPU;    	
    }  
    else if (SleepMode_Sleep == sleepMode)
    {
		SYSCRTRL_WakeUpCmd(SYSCTRL_WakeMode_SENSOR, ENABLE);
    }
	else if(SleepMode_PowerDown == sleepMode)
	{
		SYSCTRL->RST_FLAG = (SYSCTRL->RST_FLAG & ~(0x1F << 8)) | BIT(12);	
	}

	SYSCTRL_EnterSleep(sleepMode);
	if(SleepMode_CpuOff == sleepMode)
	{
		SYSCTRL->CG_CTRL2 = SYSCTRL_AHBPeriph_ALL;   
		SYSCTRL->CG_CTRL1 = SYSCTRL_APBPeriph_ALL;
		
		GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
	}
	SENSOR_EXTCmd(DISABLE);
	while(SET == SENSOR_EXTIsRuning());
	SENSOR_EXTPortCmd(SENSOR_Port_All,DISABLE);
	SENSOR_ANACmd(SENSOR_ANA_ALL,DISABLE);
	
	NVIC_Configuration(SENSOR_IRQn, DISABLE);
	printf("SENSOR Wakeup SleepMode Out\n\n");	
}

void GPIO_WakeUpTest(SleepMode_TypeDef sleepMode)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	NVIC_Configuration(EXTI2_IRQn,ENABLE);
	
	//GPIO Configuration(PC15)

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	
	EXTI_LineConfig(EXTI_Line2, EXTI_PinSource15, EXTI_Trigger_Rising_Falling);

	if(SleepMode_CpuOff == sleepMode)
    {
		SYSCTRL->CG_CTRL2 = 0;
		SYSCTRL->CG_CTRL1 = SYSCTRL_APBPeriph_GPIO;   
	}
    else if (SleepMode_Sleep == sleepMode)
    {
		SYSCRTRL_WakeUpCmd(SYSCTRL_WakeMode_GPIO_Now, ENABLE);
		GPIO_WakeEvenConfig(GPIO_PortSourceGPIOC,GPIO_Pin_15,ENABLE);
   	
		printf("pull down PC15 wakeup\n");			
    }	
	#ifdef QFN_68
	else if(SleepMode_PowerDown == sleepMode)
	{
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		SYSCTRL->RST_FLAG = (SYSCTRL->RST_FLAG & ~(0x1F << 8)) | BIT(9);
	
		printf("pull down PA15 wakeup\n");			

	}	
   #endif
	SYSCTRL_EnterSleep(sleepMode);
	if (SleepMode_CpuOff == sleepMode)
	{
		SYSCTRL->CG_CTRL2 = SYSCTRL_AHBPeriph_ALL;   
		SYSCTRL->CG_CTRL1 = SYSCTRL_APBPeriph_ALL; 			
		
		GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
	}	
	EXTI_LineConfig(EXTI_Line2, EXTI_PinSource15, EXTI_Trigger_Off);
	NVIC_Configuration(EXTI2_IRQn, DISABLE);
	NVIC_Configuration(EXTI0_IRQn, DISABLE);
	
	printf("GPIO Wakeup SleepMode Out\n\n");	
}

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;

	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
	
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;

	UART_Init(UART0, &UART_InitStructure);	
}

int SER_PutChar (int ch)
{
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) ch);
	
	return ch;
}

int fputc(int c, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	if (c == '\n')
	{
		SER_PutChar('\r');
	}
	
	return (SER_PutChar(c));
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

