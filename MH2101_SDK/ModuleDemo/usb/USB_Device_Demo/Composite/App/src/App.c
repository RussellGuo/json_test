#include "App.h"

void SystemInit(void)
{
#if (__FPU_PRESENT) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
#endif

#if CONFIG_MHSCPU


#if CONFIG_MHSCPU_MH2101
    SYSCTRL_SYSCLKSourceSelect(SELECT_INC12M);
    SYSCTRL_PLLConfig(SYSCTRL_PLL_204MHz);
    SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
    SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div_None);
    SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);
#endif // CONFIG_MHSCPU_MH2101
#endif // CONFIG_MHSCPU

}

void GPIOSetup(void);
void UARTSetup(void);

int main(void)
{
    GPIOSetup();
    UARTSetup();
    USBSetup();

    printf("MegaHunt USB Device Demo Started!\n");

#if CONFIG_USB_DEVICE_MSD
    printf("MSC: Virtual Fat FS USB Disk.\n");
#endif // CONFIG_USB_DEVICE_MSD

#if CONFIG_USB_DEVICE_VCP
    printf("VCP: Send all received data from VCP Port.\n");
#endif // CONFIG_USB_DEVICE_VCP

#if CONFIG_USB_DEVICE_HID
    printf("HID: Loop send keys 'Hello World!!!'.\n");
#endif // CONFIG_USB_DEVICE_HID



    while (1)
    {
        USBLoop();
    }
}

void GPIOSetup(void)
{
#if CONFIG_MHSCPU


#if CONFIG_MHSCPU_MH2101
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_GPIO, ENABLE);
#endif // CONFIG_MHSCPU_MH2101
#endif // CONFIG_MHSCPU
}

void UARTSetup(void)
{

#if CONFIG_MHSCPU
    UART_InitTypeDef UART_InitStructure;


#if CONFIG_MHSCPU_MH2101
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);

    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
#endif // CONFIG_MHSCPU_MH2101

    UART_InitStructure.UART_BaudRate   = 115200;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits   = UART_StopBits_1;
    UART_InitStructure.UART_Parity     = UART_Parity_No;

    UART_Init(UART0, &UART_InitStructure);
#endif // CONFIG_MHSCPU
}

int fputc(int ch, FILE* f)
{
#if CONFIG_MHSCPU
    if (ch == '\n')
    {
        while (!UART_IsTXEmpty(UART0)) {}
        UART_SendData(UART0, (uint16_t)'\r');
    }
    while (!UART_IsTXEmpty(UART0)) {}
    UART_SendData(UART0, (uint8_t)ch);
#endif // CONFIG_MHSCPU

    return ch;
}

#ifdef USE_FULL_ASSERT
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
    while (1) {}
}
#endif
