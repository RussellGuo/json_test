#include <stdio.h>
#include <string.h>
#include "mhscpu.h"
#include "mhscpu_dcmi.h"
#include "mhscpu_dma.h"
#include "mhscpu_i2c.h"
#include "DecodeLib.h"
#include "uart.h"
#include "beep.h"
#include "ProjectConfig.h"

//定义内存池
unsigned char pool[DECODE_BUFF_SIZE] = {0};
extern void DCMI_NVICConfig(void);
extern void DMA_NVICConfig(void);
extern void Cameraclk_Configuration(void);
extern void CameraI2CGPIOConfig(void);
extern void DCMIGPIOInitialize(void);
extern void TimerInit(void);

int main(void)
{
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);

    DecodeInitTypeDef DecodeInitStruct;  //初始化结构体
    DecodeFlagTypeDef ret;               //解码库初始化状态
    
//   	SYSCTRL_SYSCLKSourceSelect(SELECT_EXT12M);
	SYSCTRL_PLLConfig(SYSCTRL_PLL_192MHz);
	SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);	//注意：pclk >= 48M

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO,ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_OTP, ENABLE);
        
	UART_Configuration();
    BEEP_Init();
    //摄像头管脚复用设置
    CameraI2CGPIOConfig();
    //使能摄像头输出时钟,必须初始化前调用
    Cameraclk_Configuration();
    //DCMI管脚复用，必须初始化前调用，根据实际使用情况配置
    DCMIGPIOInitialize();
    //解码库初始化 和 内存池初始化
    DecodeInitStruct.pool = pool;
    DecodeInitStruct.size = DECODE_BUFF_SIZE;
    DecodeInitStruct.CAM_PWDN_GPIOx = CAM_PWDN_GPIO;
    DecodeInitStruct.CAM_PWDN_GPIO_Pin = CAM_PWDN_GOIO_PIN;
    DecodeInitStruct.CAM_RST_GPIOx = CAM_RST_GPIO;
    DecodeInitStruct.CAM_RST_GPIO_Pin = CAM_RST_GOIO_PIN;
    DecodeInitStruct.CAM_I2Cx = I2C0;
    DecodeInitStruct.CAM_I2CClockSpeed = I2C_ClockSpeed_400KHz;
    DecodeInitStruct.SensorConfig = NULL;
    DecodeInitStruct.SensorCfgSize = 0;
    //GC6153虚拟管脚等设置
    DecodeInitStruct.CAMVirtualHS_GPIO = CAM_VIRTUAL_HS_GPIO;           /*DCMI 虚拟 HS 输入引脚*/
    DecodeInitStruct.CAMVirtualHS_Pin = CAM_VIRTUAL_HS_PIN;            /*DCMI 虚拟 HS 输入引脚Pin*/
    DecodeInitStruct.CAMVirtualVS_GPIO = CAM_VIRTUAL_VS_GPIO;           /*DCMI 虚拟 VS 输入引脚*/
    DecodeInitStruct.CAMVirtualVS_Pin = CAM_VIRTUAL_VS_PIN;            /*DCMI 虚拟 VS 输入引脚Pin*/
    DecodeInitStruct.CAMVirtualPCLK_GPIO = CAM_VIRTUAL_PCLK_GPIO;         /*DCMI 虚拟 PIXCLK 输入引脚*/
    DecodeInitStruct.CAMVirtualPCLK_Pin = CAM_VIRTUAL_PCLK_PIN;          /*DCMI 虚拟 PIXCLK 输入引脚Pin*/
    DecodeInitStruct.CAMPixCLK_GPIO = DCMI_PIXCLK_GPIO_PORT;               /*DCMI PIXCLK 输入引脚*/
    DecodeInitStruct.CAMPixCLK_Pin = DCMI_PIXCLK_GPIO_PIN;                /*DCMI PIXCLK 输入引脚Pin*/
    DecodeInitStruct.CAMData0_GPIO = DCMI_D0_GPIO_PORT;                /*DCMI Data0 输入引脚*/
    DecodeInitStruct.CAMData0_Pin = DCMI_D0_GPIO_PIN;                 /*DCMI Data0 输入引脚Pin*/
 
    ret = DecodeInit(&DecodeInitStruct);
    if(ret != DecodeInitSuccess)
    {
        switch(ret)
        {
            case DecodeInitCheckError:
                printf("解码库校验失败，请联系芯片厂商\r\n");
            break;
            case DecodeInitMemoryError:
                printf("给解码库的buff过小\r\n");
            break;
            case DecodeInitSensorError:
                printf("摄像头初始化失败\r\n");
            break;
            default:
                break;
        }
        while(1);
    }
    //NVIC 中断配置，注意调用顺序，必须先调用DecodeInit(),然后才能调用此函数
    if(GetGrayImageSize() == IMAGE640X336)
    {
        DCMI_NVICConfig();
    }
    else
    {
        DMA_NVICConfig();
        TimerInit();
    }
    
 	printf("Decode Demo V%d.%d.%d\n", (GetDecodeLibVerison() >> 16) & 0xff, (GetDecodeLibVerison() >> 8) & 0xff, GetDecodeLibVerison() & 0xff);
    
    if(DECODE_BUFF_SIZE == IMAGE8W_SINGLE_BUFF_MIN_SIZE || DECODE_BUFF_SIZE == IMAGE30W_SINGLE_BUFF_MIN_SIZE)
    {
        //单buff解码示例
        SingleBuffDecodeDemo();
    }
    else
    {
        //双buff解码示例
        DoubleBuffDecodeDemo();
    }
    
    //释放解码所需要的软硬件资源
    CloseDecode();
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_I2C0, DISABLE);   //I2C由于是开放配置，所以要用户进行关闭
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, DISABLE);    //为防止其他地方用到
}
//DCMI 中断设置函数
void DCMI_NVICConfig(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // DCMI 中断配置
    NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure); 
    
//    DCMI_ITConfig(DCMI_IT_VSYNC, ENABLE);
    DCMI_ITConfig(DCMI_IT_OVF, ENABLE);
//    DCMI_ITConfig(DCMI_IT_LINE, ENABLE);
    DCMI_ITConfig(DCMI_IT_FRAME, ENABLE);
    DCMI_ITConfig(DCMI_IT_ERR, ENABLE);

	DCMI_ClearITPendingBit(DCMI_IT_VSYNC);
	DCMI_ClearITPendingBit(DCMI_IT_OVF);
	DCMI_ClearITPendingBit(DCMI_IT_LINE);
	DCMI_ClearITPendingBit(DCMI_IT_FRAME);
	DCMI_ClearITPendingBit(DCMI_IT_ERR);
}

//DMA 中断设置函数
void DMA_NVICConfig(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // DMA 中断配置
    NVIC_InitStructure.NVIC_IRQChannel = DMA_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);
	DMA_ClearITPendingBit(DMA_Channel_0, DMA_IT_DMATransferComplete);
	DMA_ITConfig(DMA_Channel_0, DMA_IT_DMATransferComplete, ENABLE);
	
	DMA_ClearITPendingBit(DMA_Channel_0, DMA_IT_Error);
	DMA_ITConfig(DMA_Channel_0, DMA_IT_Error, ENABLE);
}

//I2C管脚复用
void CameraI2CGPIOConfig(void)
{
    SYSCTRL_APBPeriphClockCmd( SYSCTRL_APBPeriph_I2C0, ENABLE);
    I2C_DeInit(I2C0);
    
    GPIO_PinRemapConfig( SI2C_PORT, SI2C_SCL_PIN, SI2C_GPIO_REMAP);
    GPIO_PinRemapConfig( SI2C_PORT, SI2C_SDA_PIN, SI2C_GPIO_REMAP);
}

//DCMI管脚复用
void DCMIGPIOInitialize(void) 
{
	/* 控制信号  */
	GPIO_PinRemapConfig(DCMI_VSYNC_GPIO_PORT, DCMI_VSYNC_GPIO_PIN, DCMI_VSYNC_AF);
	GPIO_PinRemapConfig(DCMI_HSYNC_GPIO_PORT, DCMI_HSYNC_GPIO_PIN, DCMI_HSYNC_AF);
	GPIO_PinRemapConfig(DCMI_PIXCLK_GPIO_PORT, DCMI_PIXCLK_GPIO_PIN, DCMI_PIXCLK_AF);
	/* 数据信号 */
	GPIO_PinRemapConfig(DCMI_D0_GPIO_PORT, DCMI_D0_GPIO_PIN, DCMI_D0_AF);
	GPIO_PinRemapConfig(DCMI_D1_GPIO_PORT, DCMI_D1_GPIO_PIN, DCMI_D1_AF);
	GPIO_PinRemapConfig(DCMI_D2_GPIO_PORT, DCMI_D2_GPIO_PIN, DCMI_D2_AF);
	GPIO_PinRemapConfig(DCMI_D3_GPIO_PORT, DCMI_D3_GPIO_PIN, DCMI_D3_AF);
	GPIO_PinRemapConfig(DCMI_D4_GPIO_PORT, DCMI_D4_GPIO_PIN, DCMI_D4_AF);
	GPIO_PinRemapConfig(DCMI_D5_GPIO_PORT, DCMI_D5_GPIO_PIN, DCMI_D5_AF);
	GPIO_PinRemapConfig(DCMI_D6_GPIO_PORT, DCMI_D6_GPIO_PIN, DCMI_D6_AF);
	GPIO_PinRemapConfig(DCMI_D7_GPIO_PORT, DCMI_D7_GPIO_PIN, DCMI_D7_AF);
}

//输出时钟设置
void Cameraclk_Configuration(void)
{
#if 1
    uint32_t Period = 0;
    uint32_t PWM_HZ = 24000000;
    SYSCTRL_ClocksTypeDef clocks;
    TIM_PWMInitTypeDef TIM_PWMSetStruct;
    //使能tim时钟
    SYSCTRL_APBPeriphClockCmd( SYSCTRL_APBPeriph_TIMM0, ENABLE);
    //获取系统时钟
    SYSCTRL_GetClocksFreq(&clocks);

    //判断输出时钟是否满足输出24M要求，不满足则修改输出时钟
    if(clocks.PCLK_Frequency / 2 < PWM_HZ)
    {
        PWM_HZ = clocks.PCLK_Frequency / 2;
    }

	Period = clocks.PCLK_Frequency / PWM_HZ;                                                                                                                                                                               

	TIM_PWMSetStruct.TIM_LowLevelPeriod = (Period / 2 - 1);
	TIM_PWMSetStruct.TIM_HighLevelPeriod = (Period - TIM_PWMSetStruct.TIM_LowLevelPeriod - 2);
    TIM_PWMSetStruct.TIMx = CAM_XCK_TIM;
    
    TIM_PWMInit(TIMM0, &TIM_PWMSetStruct);
    //复用gpio
	GPIO_PinRemapConfig(CAM_XCK_GPIO, CAM_XCK_GPIO_PIN, GPIO_Remap_2);
    //使能gpio
	TIM_Cmd(TIMM0, CAM_XCK_TIM, ENABLE);
#else	
    /* If using XCK pin that support the "CLK_24M" multiplexing function, 
	 *  you can config IO REMAP to output 24M clock directly
	 */
	GPIO_PinRemapConfig(CAM_XCK_GPIO, CAM_XCK_GPIO_PIN, GPIO_Remap_3);
#endif	
}

/**
 * Timer模拟线程初始化
 *
 *
 */
void TimerInit(void)
{
    TIM_InitTypeDef TIM_InitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
//    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_TIMM0,ENABLE);
    TIM_InitStruct.TIM_Period = 200;

    TIM_InitStruct.TIMx = CAM_TASK_TIMER;
    TIM_Init(TIMM0, &TIM_InitStruct);


    TIM_Cmd(TIMM0, CAM_TASK_TIMER, DISABLE);
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannel = CAM_TASK_TIMER_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

    NVIC_Init(&NVIC_InitStructure);
  
    TIM_ClearITPendingBit(TIMM0, CAM_TASK_TIMER);
//	NVIC->ICPR[((uint32_t)(CAM_TASK_TIMER_IRQ) >> 5)] = (1 << ((uint32_t)(CAM_TASK_TIMER_IRQ) & 0x1F)); /* Clear pending interrupt */
    TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
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






