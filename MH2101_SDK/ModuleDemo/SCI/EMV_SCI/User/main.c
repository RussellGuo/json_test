#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "mhscpu_sci.h"
#include "uart.h"
#include "test_emv.h"
#include "emv_hard.h"

void NVIC_Configuration(void);

#define GROUPA      0x1
#define GROUPB      0x2
#define GROUPE      0x3

#define SCI2_PIN    GROUPB
int main(int argc, char *argv[])
{
	//EMV4.3 must set 192MHz
    SYSCTRL_PLLConfig(SYSCTRL_PLL_192MHz);
    SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div2);
    SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div_None);
    SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);
    
    
    //Enable clock SCI0, SCI1, SCI2, UART0, and GPIO.
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_SCI0 | SYSCTRL_APBPeriph_SCI2 | SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO | SYSCTRL_APBPeriph_TIMM0, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_SCI0 | SYSCTRL_APBPeriph_SCI2 | SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
    
    //card detect
    emv_hard_CardDetectSelect(SCI_CHANNEL_0, SCI_CardDetectLowEffective);
    //Choose active level(Low level active).
    emv_hard_VCCENSelect(SCI_CHANNEL_0, SCI_VCCENLowEffective);
    
    //card detect
    emv_hard_CardDetectSelect(SCI_CHANNEL_2, SCI_CardDetectLowEffective);
    //Choose active level(Low level active).
    emv_hard_VCCENSelect(SCI_CHANNEL_2, SCI_VCCENLowEffective);
    
    uart_Config(115200);
    
    //DET: PA[6], VCCEN: PA[7], CLK: PA[8], RSTN: PA[9], IO: PA[10]
    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, GPIO_Remap_0);
    GPIO_PullUpCmd(GPIOA, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, ENABLE);

#if SCI2_PIN == GROUPA
    //PA11: DET, PA12: VCCEN, PA13: CLK, PA14: RSTN, PA15: IO
    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_Remap_0);
#elif SCI2_PIN == GROUPE
    //DET: PE[8], VCCEN: PE[9], CLK: PE[10], RSTN: PE[11], IO: PE[12]
    GPIO_PinRemapConfig(GPIOE, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12, GPIO_Remap_0);
#elif SCI2_PIN == GROUPB    
    //PB[7]: DET, PB[8]: VCCEN, PB[9]: CLK, PB[10]: RSTN, PB[11]: IO
    GPIO_PinRemapConfig(GPIOB, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11, GPIO_Remap_2);
#endif

    SCI_ConfigEMV(BIT(SCI_CHANNEL_0), 5000000);
    NVIC_Configuration();
    
    loop_back(SCI_CHANNEL_0);
    return 0;
}


void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = SCI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = SCI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = UART0_IRQn;
    NVIC_Init(&NVIC_InitStructure);
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
     ex: PRINT("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small DBG_PRINT (option LD Linker->Libraries->Small DBG_PRINT
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
  #define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  #define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    
    if (ch == '\n')
    {
        while(0 != uart_SendChar('\r'));
    }
    while(0 != uart_SendChar((uint8_t) ch));

    return ch;
}

GETCHAR_PROTOTYPE
{
    int32_t s32Recv;

    while (-1 == (s32Recv = uart_RecvChar()));

    return s32Recv;
}





