/************************ (C) COPYRIGHT Megahuntmicro *************************
 * @file                : mhscpu_sysctrl.c
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 21-October-2014
 * @brief               : This file provides all the SYSCTRL firmware functions
 *****************************************************************************/

/* Includes -----------------------------------------------------------------*/
#include "mhscpu_sysctrl.h"

/* Exported functions -------------------------------------------------------*/
/**
 * @brief  Enables or disables the AHB peripheral clock.
 * @param  SYSCTRL_AHBPeriph: specifies the AHB peripheral to gates its clock.
 *   For @b this parameter can be any combination
 *   of the following values:
 *     @arg SYSCTRL_AHBPeriph_ASYMC
 *     @arg SYSCTRL_AHBPeriph_DMA
 *     @arg SYSCTRL_AHBPeriph_USB
 *     @arg SYSCTRL_AHBPeriph_HSPI0
 *     @arg SYSCTRL_AHBPeriph_HSPI1 
 *     @arg SYSCTRL_AHBPeriph_QR
 *     @arg SYSCTRL_AHBPeriph_CMSPI 
 *     @arg SYSCTRL_AHBPeriph_OTP
 *     @arg SYSCTRL_AHBPeriph_GPU 
 *     @arg SYSCTRL_AHBPeriph_LCD
 *     @arg SYSCTRL_AHBPeriph_SYMC
 * @param  NewState: new state of the specified peripheral clock.
 *   This parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void SYSCTRL_AHBPeriphClockCmd(uint32_t SYSCTRL_AHBPeriph, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_SYSCTRL_AHB_PERIPH(SYSCTRL_AHBPeriph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
	{
        SYSCTRL->CG_CTRL2 |= SYSCTRL_AHBPeriph;
    }
    else
	{
        SYSCTRL->CG_CTRL2 &= ~SYSCTRL_AHBPeriph;
    }
}

/**
 * @brief  Enables or disables the APB peripheral clock.
 * @param  SYSCTRL_APBPeriph: specifies the APB peripheral to gates its clock.
 *   For @b this parameter can be any combination
 *   of the following values:
 *     @arg SYSCTRL_APBPeriph_TRNG
 *     @arg SYSCTRL_APBPeriph_ADC
 *     @arg SYSCTRL_APBPeriph_CRC
 *     @arg SYSCTRL_APBPeriph_KBD
 *     @arg SYSCTRL_APBPeriph_BPU
 *     @arg SYSCTRL_APBPeriph_DCMIS
 *     @arg SYSCTRL_APBPeriph_TIMM0
 *     @arg SYSCTRL_APBPeriph_GPIO
 *     @arg SYSCTRL_APBPeriph_I2C0
 *     @arg SYSCTRL_APBPeriph_SCI2
 *     @arg SYSCTRL_APBPeriph_SCI1
 *     @arg SYSCTRL_APBPeriph_SCI0
 *     @arg SYSCTRL_APBPeriph_SPI2
 *     @arg SYSCTRL_APBPeriph_SPI0
 *     @arg SYSCTRL_APBPeriph_UART3
 *     @arg SYSCTRL_APBPeriph_UART2
 *     @arg SYSCTRL_APBPeriph_UART1
 *     @arg SYSCTRL_APBPeriph_UART0
 * @param  NewState: new state of the specified peripheral clock.
 *   This parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void SYSCTRL_APBPeriphClockCmd(uint32_t SYSCTRL_APBPeriph, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_SYSCTRL_APB_PERIPH(SYSCTRL_APBPeriph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
	{
        SYSCTRL->CG_CTRL1 |= SYSCTRL_APBPeriph;
    }
    else
	{
        SYSCTRL->CG_CTRL1 &= ~SYSCTRL_APBPeriph;
    }
}

/**
 * @brief  Enables or disables the AHB peripheral reset.
 * @param  SYSCTRL_AHBPeriph: specifies the AHB peripheral to reset(CM3 reset/GLB Reset).
 *   For @b this parameter can be any combination
 *   of the following values:
 *     @arg SYSCTRL_GLB_RESET
 *     @arg SYSCTRL_CM3_RESET
 *     @arg SYSCTRL_DMA_RESET
 *     @arg SYSCTRL_USB_RESET
 *     @arg SYSCTRL_HSPI0_RESET
 *     @arg SYSCTRL_HSPI1_RESET
 *     @arg SYSCTRL_QR_RESET
 *     @arg SYSCTRL_CMSPI_RESET
 *     @arg SYSCTRL_LCD_RESET
 *     @arg SYSCTRL_CRYPT_RESET
 * @param  NewState: new state of the specified peripheral reset.
 *   This parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void SYSCTRL_AHBPeriphResetCmd(uint32_t SYSCTRL_AHBPeriph, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_SYSCTRL_AHB_PERIPH_RESET(SYSCTRL_AHBPeriph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    /* LOCK_R enable reset */
    SYSCTRL->LOCK_R &= ~(SYSCTRL_AHBPeriph & 0xF0000000);
    if (NewState != DISABLE) 
	{
        SYSCTRL->SOFT_RST2 |= SYSCTRL_AHBPeriph;
    }
    else
	{
        SYSCTRL->SOFT_RST2 &= ~SYSCTRL_AHBPeriph;
    }
	
    /* LOCK_R disable reset */
    SYSCTRL->LOCK_R |= (SYSCTRL_AHBPeriph & 0xF0000000);
}

/**
 * @brief  Enables or disables the APB peripheral reset.
 * @param  SYSCTRL_APBPeriph: specifies the APB peripheral to reset.
 *   For @b this parameter can be any combination
 *   of the following values:
 *     @arg SYSCTRL_APBPeriph_TRNG
 *     @arg SYSCTRL_APBPeriph_ADC
 *     @arg SYSCTRL_APBPeriph_CRC
 *     @arg SYSCTRL_APBPeriph_KBD
 *     @arg SYSCTRL_APBPeriph_DCMIS
 *     @arg SYSCTRL_APBPeriph_TIMM0
 *     @arg SYSCTRL_APBPeriph_GPIO
 *     @arg SYSCTRL_APBPeriph_I2C0
 *     @arg SYSCTRL_APBPeriph_SCI2
 *     @arg SYSCTRL_APBPeriph_SCI0
 *     @arg SYSCTRL_APBPeriph_SPI2
 *     @arg SYSCTRL_APBPeriph_SPI0
 *     @arg SYSCTRL_APBPeriph_UART3
 *     @arg SYSCTRL_APBPeriph_UART2
 *     @arg SYSCTRL_APBPeriph_UART1
 *     @arg SYSCTRL_APBPeriph_UART0
 * @param  NewState: new state of the specified peripheral reset.
 *   This parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void SYSCTRL_APBPeriphResetCmd(uint32_t SYSCTRL_APBPeriph, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_SYSCTRL_APB_PERIPH(SYSCTRL_APBPeriph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
	{
        SYSCTRL->SOFT_RST1 |= SYSCTRL_APBPeriph;
    }
    else
	{
        SYSCTRL->SOFT_RST1 &= ~SYSCTRL_APBPeriph;
    }
}

#if defined(__CC_ARM)
__STATIC_INLINE __ASM void SYSCTRL_Sleep(void)
{
    CPSID i;
    NOP;
    NOP;
    NOP;
    NOP;
    WFI;
    NOP;
    NOP;
    NOP;
    NOP;
    CPSIE i;
    BX    LR;
}
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__STATIC_FORCEINLINE void SYSCTRL_Sleep(void)
{
    __ASM volatile
	( //
        "CPSID i;"
        "NOP;"
        "NOP;"
        "NOP;"
        "NOP;"
        "WFI;"
        "NOP;"
        "NOP;"
        "NOP;"
        "NOP;"
        "CPSIE i;"
        "BX    LR;" //
    );
}
#elif defined(__GNUC__)
void SYSCTRL_Sleep(void)
{
    asm("CPSID i");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("wfi");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("CPSIE i");
    asm("BX LR");
}
#elif defined(__ICCARM__)
void SYSCTRL_Sleep(void)
{
    __ASM("CPSID i\n"
          "NOP\n"
          "NOP\n"
          "NOP\n"
          "NOP\n"
          "WFI\n"
          "NOP\n"
          "NOP\n"
          "NOP\n"
          "NOP\n"
          "CPSIE i\n"
          "BX LR");
}
#endif

/**
 * @brief  Enter low power mode
 * @param  SleepMode: specifies low power mode
 *   This parameter can be: SleepMode_CpuOff or SleepMode_DeepSleep.
 * @retval None
 */
void SYSCTRL_EnterSleep(SleepMode_TypeDef SleepMode)
{
    uint32_t rng;
    assert_param(IS_ALL_SLEEP_MODE(SleepMode));

    if (SleepMode == SleepMode_CpuOff) 
	{
        SYSCTRL->FREQ_SEL = (SYSCTRL->FREQ_SEL & ~(SYSCTRL_FREQ_SEL_POWERMODE_Mask)) | SYSCTRL_FREQ_SEL_POWERMODE_CPUOFF;
        while (!(SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_POWERMODE_CPUOFF))) {}
        SYSCTRL_Sleep();
    }
	else if (SleepMode == SleepMode_Sleep)
	{
        SYSCTRL->FREQ_SEL = (SYSCTRL->FREQ_SEL & ~(SYSCTRL_FREQ_SEL_POWERMODE_Mask)) | SYSCTRL_FREQ_SEL_POWERMODE_SLEEP;
        rng               = TRNG->RNG_ANA;
        TRNG->RNG_ANA     = rng | TRNG_RNG_AMA_PD_ALL_Mask;
        SYSCTRL_Sleep();
        TRNG->RNG_ANA = rng;		
    }
    else if (SleepMode == SleepMode_PowerDown)
	{
        SYSCTRL->FREQ_SEL = (SYSCTRL->FREQ_SEL & ~(SYSCTRL_FREQ_SEL_POWERMODE_Mask)) | SYSCTRL_FREQ_SEL_POWERMODE_POWERDOWN;
        SYSCTRL_Sleep();
    }
}

/**
 * @brief  Select System clock source
 * @param  SourceSelect System clock source value.
 * @retval None
 */
void SYSCTRL_SYSCLKSourceSelect(SYSCLK_SOURCE_TypeDef SourceSelect) 
{
    assert_param(IS_SYSCLK_SOURCE(SourceSelect));
	
    switch (SourceSelect)
	{
        case SELECT_EXT12M:
			SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_CLOCK_SOURCE_Mask)) | SYSCTRL_FREQ_SEL_CLOCK_SOURCE_EXT);		
            break;

        case SELECT_INC12M:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_CLOCK_SOURCE_Mask)) | SYSCTRL_FREQ_SEL_CLOCK_SOURCE_INC);
            break;
		
		case SELECT_EXTPA11:
			SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_CLOCK_SOURCE_Mask)) | SYSCTRL_FREQ_SEL_CLOCK_SOURCE_INC);		
			GPIO_PinRemapConfig(GPIOA, GPIO_Pin_11, GPIO_Remap_2);			
			SYSCTRL->ANA_CTRL |= BIT(2);
			SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_CLOCK_SOURCE_Mask)) | SYSCTRL_FREQ_SEL_CLOCK_SOURCE_EXT);		
			break;
		
		case SELECT_EXTPB7:
			SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_CLOCK_SOURCE_Mask)) | SYSCTRL_FREQ_SEL_CLOCK_SOURCE_INC);
			GPIO_PinRemapConfig(GPIOB, GPIO_Pin_7, GPIO_Remap_3);	
			SYSCTRL->ANA_CTRL |= BIT(2);
			SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_CLOCK_SOURCE_Mask)) | SYSCTRL_FREQ_SEL_CLOCK_SOURCE_EXT);
			break;
    }
}

/** @brief  Set System clock Freq
 * @param  SYSCLK_Freq System clock set value
 * @retval None
 */
void SYSCTRL_PLLConfig(SYSCTRL_PLL_TypeDef SYSCLK_Freq)
{
    assert_param(IS_PLL_FREQ(SYSCLK_Freq));

    switch ((uint32_t)SYSCLK_Freq)
	{
        case SYSCTRL_PLL_204MHz:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_XTAL_Mask)) | SYSCTRL_FREQ_SEL_XTAL_204Mhz);
            break;

        case SYSCTRL_PLL_192MHz:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_XTAL_Mask)) | SYSCTRL_FREQ_SEL_XTAL_192Mhz);
            break;

        case SYSCTRL_PLL_180MHz:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_XTAL_Mask)) | SYSCTRL_FREQ_SEL_XTAL_180Mhz);
            break;

        case SYSCTRL_PLL_168MHz:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_XTAL_Mask)) | SYSCTRL_FREQ_SEL_XTAL_168Mhz);
            break;

        case SYSCTRL_PLL_156MHz:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_XTAL_Mask)) | SYSCTRL_FREQ_SEL_XTAL_156Mhz);
            break;

        case SYSCTRL_PLL_144MHz:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_XTAL_Mask)) | SYSCTRL_FREQ_SEL_XTAL_144Mhz);
            break;

        case SYSCTRL_PLL_132MHz:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_XTAL_Mask)) | SYSCTRL_FREQ_SEL_XTAL_132Mhz);
            break;

        case SYSCTRL_PLL_120MHz:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_XTAL_Mask)) | SYSCTRL_FREQ_SEL_XTAL_120Mhz);
            break;

        case SYSCTRL_PLL_108MHz:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_XTAL_Mask)) | SYSCTRL_FREQ_SEL_XTAL_108Mhz);
            break;
    }
}

/**
 * @brief  Set System PLL Div
 * @param  PLL_Div Div value
 * @retval None
 */
void SYSCTRL_PLLDivConfig(uint32_t PLL_Div)
{
    assert_param(IS_GET_SYSCTRL_PLL_DIV(PLL_Div));

    switch (PLL_Div)
	{
        case SYSCTRL_PLL_Div_None:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_PLL_DIV_Mask)) | SYSCTRL_FREQ_SEL_PLL_DIV_1_0);
            break;

        case SYSCTRL_PLL_Div2:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_PLL_DIV_Mask)) | SYSCTRL_FREQ_SEL_PLL_DIV_1_2);
            break;

        case SYSCTRL_PLL_Div4:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_PLL_DIV_Mask)) | SYSCTRL_FREQ_SEL_PLL_DIV_1_4);
            break;
    }
}

/**
 * @brief  Set System HCLK Div
 * @param  HCLK_Div Div value
 * @retval None
 */
void SYSCTRL_HCLKConfig(uint32_t HCLK_Div)
{
    SYSCTRL_ClocksTypeDef Sysctrl_Clocks;

    assert_param(IS_GET_SYSCTRL_HCLK_DIV(HCLK_Div));

    SYSCTRL_GetClocksFreq(&Sysctrl_Clocks);
    switch (HCLK_Div)
	{
        case SYSCTRL_HCLK_Div2:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_HCLK_DIV_Mask)) | SYSCTRL_FREQ_SEL_HCLK_DIV_1_2);
            break;

        case SYSCTRL_HCLK_Div_None:
            if (Sysctrl_Clocks.CPU_Frequency <= 102000000)
			{
                SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_HCLK_DIV_Mask)) | SYSCTRL_FREQ_SEL_HCLK_DIV_1_0);
            }
            else
			{
                SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_HCLK_DIV_Mask)) | SYSCTRL_FREQ_SEL_HCLK_DIV_1_2);
            }
            break;
    }
}

/**
 * @brief  Set System PCLK Div
 * @param  PCLK_Div Div value
 * @retval None
 */
void SYSCTRL_PCLKConfig(uint32_t PCLK_Div)
{
    assert_param(IS_GET_SYSCTRL_PCLK_DIV(PCLK_Div));

    switch (PCLK_Div)
	{
        case SYSCTRL_PCLK_Div2:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_PCLK_DIV_Mask)) | SYSCTRL_FREQ_SEL_PCLK_DIV_1_2);
            break;

        case SYSCTRL_PCLK_Div4:
            SYSCTRL->FREQ_SEL = ((SYSCTRL->FREQ_SEL & (~SYSCTRL_FREQ_SEL_PCLK_DIV_Mask)) | SYSCTRL_FREQ_SEL_PCLK_DIV_1_4);
            break;
    }
}

/**
 * @brief Get the frequencies of different on chip clocks
 * @param SYSCTRL_Clocks: pointer to a SYSCTRL_ClocksTypeDef structure which will hold the clocks frequencies
 * @retval
 */
void SYSCTRL_GetClocksFreq(SYSCTRL_ClocksTypeDef* SYSCTRL_Clocks)
{
    /* PLLCLK */
    switch (SYSCTRL->FREQ_SEL & SYSCTRL_FREQ_SEL_XTAL_Mask)
	{
        case SYSCTRL_FREQ_SEL_XTAL_204Mhz:
            SYSCTRL_Clocks->PLL_Frequency = 204000000;
            break;
        case SYSCTRL_FREQ_SEL_XTAL_192Mhz:
            SYSCTRL_Clocks->PLL_Frequency = 192000000;
            break;
        case SYSCTRL_FREQ_SEL_XTAL_180Mhz:
            SYSCTRL_Clocks->PLL_Frequency = 180000000;
            break;
        case SYSCTRL_FREQ_SEL_XTAL_168Mhz:
            SYSCTRL_Clocks->PLL_Frequency = 168000000;
            break;
        case SYSCTRL_FREQ_SEL_XTAL_156Mhz:
            SYSCTRL_Clocks->PLL_Frequency = 156000000;
            break;
        case SYSCTRL_FREQ_SEL_XTAL_144Mhz:
            SYSCTRL_Clocks->PLL_Frequency = 144000000;
            break;
        case SYSCTRL_FREQ_SEL_XTAL_132Mhz:
            SYSCTRL_Clocks->PLL_Frequency = 132000000;
            break;
        case SYSCTRL_FREQ_SEL_XTAL_120Mhz:
            SYSCTRL_Clocks->PLL_Frequency = 120000000;
            break;
        case SYSCTRL_FREQ_SEL_XTAL_108Mhz:
            SYSCTRL_Clocks->PLL_Frequency = 108000000;
            break;
    }

    /* CPUCLK */
    switch (SYSCTRL->FREQ_SEL & SYSCTRL_FREQ_SEL_PLL_DIV_Mask)
	{
        case SYSCTRL_FREQ_SEL_PLL_DIV_1_0:
            SYSCTRL_Clocks->CPU_Frequency = SYSCTRL_Clocks->PLL_Frequency;
            break;
        case SYSCTRL_FREQ_SEL_PLL_DIV_1_2:
            SYSCTRL_Clocks->CPU_Frequency = SYSCTRL_Clocks->PLL_Frequency / 2;
            break;
        case SYSCTRL_FREQ_SEL_PLL_DIV_1_4:
            SYSCTRL_Clocks->CPU_Frequency = SYSCTRL_Clocks->PLL_Frequency / 4;
            break;
        default:
            break;
    }

    /* HCLK */
    switch (SYSCTRL->FREQ_SEL & SYSCTRL_FREQ_SEL_HCLK_DIV_Mask)
	{
        case SYSCTRL_FREQ_SEL_HCLK_DIV_1_0:
            SYSCTRL_Clocks->HCLK_Frequency = SYSCTRL_Clocks->CPU_Frequency;
            break;
        case SYSCTRL_FREQ_SEL_HCLK_DIV_1_2:
            SYSCTRL_Clocks->HCLK_Frequency = SYSCTRL_Clocks->CPU_Frequency / 2;
            break;
        default:
            SYSCTRL_Clocks->HCLK_Frequency = 0;
            break;
    }

    /* PCLK */
    switch (SYSCTRL->FREQ_SEL & SYSCTRL_FREQ_SEL_PCLK_DIV_Mask)
	{
        case SYSCTRL_FREQ_SEL_PCLK_DIV_1_2:
            SYSCTRL_Clocks->PCLK_Frequency = SYSCTRL_Clocks->HCLK_Frequency / 2;
            break;
        case SYSCTRL_FREQ_SEL_PCLK_DIV_1_4:
            SYSCTRL_Clocks->PCLK_Frequency = SYSCTRL_Clocks->HCLK_Frequency / 4;
            break;
        default:
            SYSCTRL_Clocks->PCLK_Frequency = 0;
            break;
    }
}

/**
 * @brief  Get reset flag
 * @param  None
 * @retval reset flag
 */
uint8_t SYSCTRL_GetResetFlag(void)
{
    return (SYSCTRL->RST_FLAG & 0x7);
}

/**
 * @brief  Clear reset flag
 * @param  ResetFlag: specify the reset flag
 *         This parameter can be one of the following values:T
 *      @arg SYSCTRL_RST_FLAG_SOFT_
 *      @arg SYSCTRL_RST_FLAG_POR_RESERESET
 *      @arg SYSCTRL_RST_FLAG_WDT_RESET
 * @retval None
 */
void SYSCTRL_ClearResetFlag(uint32_t ResetFlag)
{
    assert_param(IS_RST_FLAG(ResetFlag));

    SYSCTRL->RST_FLAG &= ~(ResetFlag);
}

/**
  * @brief  Enable or disables wakeup
  * @param  SYSCTRL_WakeModeTypeDef: 	SYSCTRL_WakeMode_GPIO_Now,
  *									 	SYSCTRL_WakeMode_GPIO_AfterGlitch,
  *   								 	SYSCTRL_WakeMode_KBD,
  *										SYSCTRL_WakeMode_RTC,
  *										SYSCTRL_WakeMode_MSR,
  *										SYSCTRL_WakeMode_SENSOR,
  *										SYSCTRL_WakeMode_AWD,
  *										SYSCTRL_WakeMode_CHG,
  *			NewState: 					ENABLE or DISABLE									
  * @retval None
  */
void SYSCRTRL_WakeUpCmd(SYSCTRL_WakeModeTypeDef WakeMode, FunctionalState NewState)
{	
	assert_param(IS_SYSCTRL_WAKE_MODE(WakeMode));
	
	if ((SYSCTRL_WakeMode_GPIO_Now == WakeMode) || (SYSCTRL_WakeMode_GPIO_AfterGlitch == WakeMode))
	{
		GPIO->WAKE_TYPE_EN = (GPIO->WAKE_TYPE_EN & ~0x1U) | WakeMode;
	}
	else
	{
		if (DISABLE != NewState)
		{
			GPIO->WAKE_TYPE_EN |= WakeMode;
		}
		else
		{
			GPIO->WAKE_TYPE_EN &= ~(WakeMode);
		}
	}
}

/**
  * @brief  Enable or disables powerdown mode wakeup
  * @param  SYSCTRL_PDWakeModeTypeDef: 	SYSCTRL_PDWakeMode_PF6,
  *									 	SYSCTRL_PDWakeMode_PA15,
  *   								 	SYSCTRL_PDWakeMode_PA14,
  *										SYSCTRL_PDWakeMode_SENSOR,
  *			NewState: 					ENABLE or DISABLE									
  * @retval None
  */

void SYSCRTRL_PDWakeUpCmd(SYSCTRL_PDWakeModeTypeDef WakeMode, FunctionalState NewState)
{
	assert_param(IS_SYSCTRL_PDWAKE_MODE(WakeMode));
	
	if (DISABLE != NewState)
	{
		SYSCTRL->RST_FLAG |= WakeMode;
	}
	else
	{
		SYSCTRL->RST_FLAG &= ~(WakeMode);
	}
}

/**
  * @brief  Calibrate 12M
  * @param  CalibrateMode : SYSCTRL_CALIBRATE_12M_USE_USB or SYSCTRL_CALIBRATE_12M_USE_XTA					
  * @retval none
  */
void SYSCTRL_Calibrate12M(SYSCTRL_CalibrateTypeDef CalibrateMode)
{
	assert_param(IS_SYSCTRL_CALIBRATE_MODE(CalibrateMode));
	
	if (CalibrateMode == SYSCTRL_CALIBRATE_12M_USE_USB)
	{
		SYSCTRL->CALIB_CSR |= BIT(10);
		while ((SYSCTRL->CALIB_CSR & BIT(10)));
	}
	
	if (CalibrateMode == SYSCTRL_CALIBRATE_12M_USE_XTA)
	{
		SYSCTRL->CALIB_CSR |= BIT(15);	
		SYSCTRL->CALIB_CSR |= BIT(8);
		while( SYSCTRL->CALIB_CSR & BIT(8));
	}
}


/**
  * @brief  get chip id
  * @param  none								
  * @retval chip id
  */
void SYSCTRL_GetChipSN(unsigned char *ChipSN)
{
	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_OTP , ENABLE);  
	memcpy(ChipSN, (uint32_t *)(SYSCTRL_CHIP_SN_ADDR), SYSCTRL_CHIP_SN_LEN);
}

/**
  * @brief  Soft Reset
  * @param  None
  * @retval None
  */
void SYSCTRL_SoftReset(void)
{
    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_GLB_RESET, ENABLE);
}

/**
  * @brief  Enter USB Download Mode
  * @param  None
  * @retval None
  */
void SYSCTRL_EnterUSBDownload(void)
{
	SYSCTRL->RSVD_POR |= SYSCTRL_USB_DOWNLOAD_FLAG;
	SYSCTRL_AHBPeriphResetCmd(SYSCTRL_GLB_RESET, ENABLE);
}

/**************************      (C) COPYRIGHT Megahunt    *****END OF FILE****/
