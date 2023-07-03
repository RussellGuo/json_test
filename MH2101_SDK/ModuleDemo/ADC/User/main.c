#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define ADC_BUFFER_SIZE     2000            //采样点数
#define WINDOWN_WIDTH   	80  
#define VOLTAGE_SELECT 		1
typedef struct
{
    uint16_t X_Axis;
    uint16_t Y_Axis;
}Type_TableDataDef;

typedef struct
{
    Type_TableDataDef *Table;
    uint16_t TableSize;
    uint16_t Frequency_MAX;
    uint16_t Frequency_MIN;
}Type_HistogramDef;

uint16_t ADC_Buffer[ADC_BUFFER_SIZE];
ADC_InitTypeDef ADC_InitStruct;

uint8_t Flag;
void Delay(void);
void NVIC_Configuration(void);
void UART_Configuration(void);
void ADC_Configuration(void);
void AnalyzeData(Type_HistogramDef *Histogram, uint16_t *buf, uint16_t BufLen);
uint8_t AddX_AxisData(Type_HistogramDef *Histogram, uint16_t X_data);
void PrintHistogram(Type_HistogramDef *Histogram);
void Sequence(Type_HistogramDef *Histogram);

int main(void)
{
    uint32_t i, u32_PointNumber = 200;
    Type_HistogramDef Histogram;
    
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_ADC | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_ADC, ENABLE);
        
    UART_Configuration();
    NVIC_Configuration();
	ADC_Configuration();       
    ADC_StartCmd(ENABLE);
    ADC_BuffCmd(ENABLE);
#if(VOLTAGE_SELECT == 1)
	ADC_DivResistorCmd(ENABLE);
	Flag = 1;
#elif(VOLTAGE_SELECT == 0)
	ADC_DivResistorCmd(DISABLE);
	Flag = 0;
#endif
    Delay();
	printf("MegaHunt SCPU ADC Demo V1.0.\n");
    printf("Start work.\n");
    
    for(i = 0; i < u32_PointNumber; i++)
    {
        ADC_Buffer[i] = ADC_GetResult();
    }
	
	for(i = 0; i < u32_PointNumber; i++)
	{
		if(ADC_InitStruct.ADC_Channel == ADC_CHANNEL_CHARGE_VBAT)
		{
			printf("ADC Value is :\t%d \t %.3f \tV \r\n", ADC_Buffer[i], ((float)ADC_CalVoltage(ADC_Buffer[i], 1700) * 11 / 4)/1000);
		}
		else
		{
			if(Flag == 0)
			{
				printf("ADC Value is :\t%d \t %.3f \tV \r\n", ADC_Buffer[i], ((float)ADC_CalVoltage(ADC_Buffer[i], 1700))/1000);
			}
			else
			{
				printf("ADC Value is :\t%d \t %.3f \tV \r\n", ADC_Buffer[i], ((float)ADC_CalVoltage(ADC_Buffer[i], 1700) * 2)/1000);
			}
		}
    }
    AnalyzeData(&Histogram, ADC_Buffer, u32_PointNumber);
    
	while(1) 
    {
    }
}
void Delay(void)
{
    uint32_t number = 0x5Fffff;
    while(number--);
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = ADC0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
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

void ADC_Configuration(void)
{
	GPIO_FuncConfig(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5, GPIO_FUNC_ANA);
    GPIO_PullUpCmd(GPIOC,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5,DISABLE);
    
	GPIO_FuncConfig(GPIOE, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 , GPIO_FUNC_ANA);
    GPIO_PullUpCmd(GPIOE,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15,DISABLE);
	
	GPIO_FuncConfig(GPIOD, GPIO_Pin_10 , GPIO_FUNC_ANA);
    GPIO_PullUpCmd(GPIOD,GPIO_Pin_10,DISABLE);
	
    ADC_InitStruct.ADC_Channel = ADC_CHANNEL_2;
    ADC_InitStruct.ADC_SampSpeed = ADC_SpeedPrescaler_8;
    ADC_InitStruct.ADC_IRQ_EN = DISABLE;
    ADC_InitStruct.ADC_FIFO_EN = DISABLE;
    ADC_Init(&ADC_InitStruct);
}

//计算方差,均值,最大最小值,统计频率分布
void AnalyzeData(Type_HistogramDef *Histogram, uint16_t *buf, uint16_t BufLen)
{
    uint16_t i, max, min;
    double Sum ,f16Average;
    uint16_t RemainTableSize, ret;
    Sum = 0;
    max = 0;
    min = 4096;
    
    for (i = 0; i < BufLen; i++)
    {
        Sum += buf[i];
        if (buf[i] > max)   max = buf[i];
        if (buf[i] < min)   min = buf[i];

        if (RemainTableSize > (ret = AddX_AxisData(Histogram, buf[i])))       //添加数据到频率统计表
        {
            RemainTableSize = ret;
        }
    }
    f16Average = Sum / BufLen;

    Sum = 0;
    for (i = 0; i < BufLen; i++)            //计算方差
    {
        Sum += ( (double)buf[i] - f16Average) * ( (double)buf[i] - f16Average);
    }
    
    printf("+-----------------------+\r\n");
    printf("* 采样数:\t  %6d\r\n", BufLen);
    printf("* 最大值:\t  %6d\r\n", max);
    printf("* 最小值:\t  %6d\r\n", min);
    printf("* 电压浮动:\t%6.0fmV\r\n", (float)(max - min) * 1700 / 4096);
    printf("* 均值:\t\t  %6.2f\r\n", f16Average);
    printf("* 方差:\t\t  %6.4f\r\n", Sum / BufLen);
    printf("+-----------------------+\r\n");
    printf("*频率分布\t 数据表剩余容量 %d/%d %s", RemainTableSize, Histogram->TableSize, (RemainTableSize > 0)?(" "):("数据表容量不足,部分数据未显示!"));
    
    PrintHistogram(Histogram);
}

//添加频率统计数据
uint8_t AddX_AxisData(Type_HistogramDef *Histogram, uint16_t X_data)
{
    uint16_t i, isFindX_Data = 0;

    for (i = 0; i < Histogram->TableSize && (0 != Histogram->Table[i].Y_Axis); i++)
    {
        if (X_data == Histogram->Table[i].X_Axis)
        {
            Histogram->Table[i].Y_Axis ++;
            isFindX_Data = 1;
            break;
        }
    }
    if (!isFindX_Data)
    {
        Histogram->Table[i].X_Axis = X_data;
        Histogram->Table[i].Y_Axis ++;
    }
    if (Histogram->Table[i].Y_Axis > Histogram->Frequency_MAX)  Histogram->Frequency_MAX = Histogram->Table[i].Y_Axis;
    if (Histogram->Table[i].Y_Axis < Histogram->Frequency_MIN)  Histogram->Frequency_MIN = Histogram->Table[i].Y_Axis;
    
    return Histogram->TableSize - i - 1;    //返回剩余空间
}

//输出频率分布表
void PrintHistogram(Type_HistogramDef *Histogram)
{
    uint16_t i, j;
    float UnitLength = (float)WINDOWN_WIDTH / (float)(Histogram->Frequency_MAX - Histogram->Frequency_MIN); //缩放至窗口范围

    Sequence(Histogram);    //对频率进行排序
    printf("\r\n+");
    for (i = 0; i < WINDOWN_WIDTH + 15; i++)
    {
        printf("-");
    }
    printf("\r\n");
    for (i = 0; i < Histogram->TableSize; i++)
    {
        if (0UL == Histogram->Table[i].Y_Axis)  //数据打印结束
        {
            break;
        }
        else
        {
            printf("*%d\t| ", Histogram->Table[i].X_Axis);
            for (j = 0; j < (UnitLength * Histogram->Table[i].Y_Axis + 0.5); j++)
            {
                printf("o");
            }
            printf("  %d\r\n", Histogram->Table[i].Y_Axis);
        }
    }
    printf("+");
    for (i = 0; i < WINDOWN_WIDTH + 15; i++)
    {
        printf("-");
    }
    printf("\r\n");
}

//直接插入排序,对频率分布进行排序显示
void Sequence(Type_HistogramDef *Histogram)
{
    int16_t i,j;
    Type_TableDataDef TableTmp;
    
    for(j = 1; j < Histogram->TableSize && 0 != Histogram->Table[j].Y_Axis ; j++) //直接插入排序
    {
        TableTmp = Histogram->Table[j];
        i = j - 1;
        //注:两个条件顺序不可调换
        while(i >= 0 && Histogram->Table[i].X_Axis > TableTmp.X_Axis)     //从大到小: < ;从小到大: >
        {                                                                 //Histogram->Table[i].Y_Axis > TableTmp.Y_Axis以频率值进行排序
            Histogram->Table[i + 1] = Histogram->Table[i];
            i--;
        }
        Histogram->Table[i + 1] = TableTmp;
    }
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
        printf("%s %s %d \r\n", __func__, file, line);
	}
}
#endif
