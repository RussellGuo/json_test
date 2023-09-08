#include "psam_pin_opt.h"
#include "psam_hw_abstractor.h"

static unsigned char gucSciMode;

MOD_LOCAL void sc_pinopt_channel_select(int slot)
{
#ifdef SC_SIMU_ENABLE
#ifdef SIMU_SLOT_DEV
    if(slot == SIMU1) {
        GPIO_ResetBits(SIMU_SLOT_DEV, BIT(SIMU_SLOT_PIN));
    } else if (slot == SIMU2) {
        GPIO_SetBits(SIMU_SLOT_DEV, BIT(SIMU_SLOT_PIN));
#endif
#endif
}

MOD_LOCAL void sc_simu_pinopt_init(void)
{
    GPIO_InitTypeDef gpio_conf;

    gpio_conf.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_conf.GPIO_Remap = GPIO_Remap_1;

#ifdef SIMU_SLOT_DEV
    gpio_conf.GPIO_Pin = BIT(SIMU_SLOT_PIN);
    GPIO_Init(SIMU_SLOT_DEV, &gpio_conf);
#endif

#if defined(SIMU_VCCSEL0_DEV) && defined(SIMU_VCCSEL1_DEV)
    gpio_conf.GPIO_Pin = BIT(SIMU_VCCSEL0_PIN);
    GPIO_Init(SIMU_VCCSEL0_DEV, &gpio_conf);

    gpio_conf.GPIO_Pin = BIT(SIMU_VCCSEL1_PIN);
    GPIO_Init(SIMU_VCCSEL1_DEV, &gpio_conf);
#endif

    gpio_conf.GPIO_Pin = BIT(SIMU_VCC_PIN);
    GPIO_Init(SIMU_VCC_DEV, &gpio_conf);

#if 1//def SIMU1_RST_DEV
    gpio_conf.GPIO_Pin = BIT(SIMU1_RST_PIN);
    GPIO_Init(SIMU1_RST_DEV, &gpio_conf);
#endif
#if 1//def SIMU2_RST_DEV
    gpio_conf.GPIO_Pin = BIT(SIMU2_RST_PIN);
    GPIO_Init(SIMU2_RST_DEV, &gpio_conf);
#endif

#if 1//def SIMU1_IO_DEV
    gpio_conf.GPIO_Mode = GPIO_Mode_Out_OD_PU;
    gpio_conf.GPIO_Remap = GPIO_Remap_1;
    gpio_conf.GPIO_Pin = BIT(SIMU1_IO_PIN);
    GPIO_Init(SIMU1_IO_DEV, &gpio_conf);
#endif
#if 1//def SIMU2_IO_DEV
    gpio_conf.GPIO_Mode = GPIO_Mode_Out_OD_PU;
    gpio_conf.GPIO_Remap = GPIO_Remap_1;
    gpio_conf.GPIO_Pin = BIT(SIMU2_IO_PIN);
    GPIO_Init(SIMU2_IO_DEV, &gpio_conf);
#endif

    gpio_conf.GPIO_Remap = GPIO_Remap_2;
    gpio_conf.GPIO_Pin = BIT(SIMU1_CLK_PIN);
    GPIO_Init(SIMU1_CLK_DEV, &gpio_conf);
    GPIO_PullUpCmd(SIMU1_CLK_DEV, BIT(SIMU1_CLK_PIN), DISABLE);

    gpio_conf.GPIO_Remap = GPIO_Remap_2;
    gpio_conf.GPIO_Pin = BIT(SIMU2_CLK_PIN);
    GPIO_Init(SIMU2_CLK_DEV, &gpio_conf);
    GPIO_PullUpCmd(SIMU2_CLK_DEV, BIT(SIMU2_CLK_PIN), DISABLE);

#ifdef SIMU_SLOT_DEV
    GPIO_WriteBit(SIMU_SLOT_DEV, BIT(SIMU_SLOT_PIN));
#endif
#if defined(SIMU_VCCSEL0_DEV) && defined(SIMU_VCCSEL1_DEV)
    GPIO_ResetBits(SIMU_VCCSEL0_DEV, BIT(SIMU_VCCSEL0_PIN));
    GPIO_ResetBits(SIMU_VCCSEL1_DEV, BIT(SIMU_VCCSEL1_PIN));
#endif
#if defined(DPA_CHECK) && defined(SIMU_VCCSEL0_DEV)
    GPIO_SetBits(SIMU_VCCSEL0_DEV, BIT(SIMU_VCCSEL0_PIN));
#endif

    GPIO_ResetBits(SIMU_VCC_DEV, BIT(SIMU_VCC_PIN));
#if 1//def SIMU1_RST_DEV
    GPIO_ResetBits(SIMU1_RST_DEV, BIT(SIMU1_RST_PIN));
#endif
#if 1//def SIMU2_RST_DEV
    GPIO_ResetBits(SIMU2_RST_DEV, BIT(SIMU2_RST_PIN));
#endif
#if 1 //def SIMU1_IO_DEV
    GPIO_ResetBits(SIMU1_IO_DEV, BIT(SIMU1_IO_PIN));
#endif
#if 1//def SIMU2_IO_DEV
    GPIO_ResetBits(SIMU2_IO_DEV, BIT(SIMU2_IO_PIN));
#endif
}

MOD_LOCAL void sc_simu_pinopt_volt_select(int volt)
{
#if defined(SIMU_VCCSEL0_DEV) && defined(SIMU_VCCSEL1_DEV)
    if (volt == ICC_VOLT_5V) {
        GPIO_ResetBits(SIMU_VCCSEL0_DEV, BIT(SIMU_VCCSEL0_PIN));
        GPIO_SetBits(SIMU_VCCSEL1_DEV, BIT(SIMU_VCCSEL1_PIN));
    } else if (volt == ICC_VOLT_1P8V) {
        GPIO_SetBits(SIMU_VCCSEL0_DEV, BIT(SIMU_VCCSEL0_PIN));
        GPIO_SetBits(SIMU_VCCSEL1_DEV, BIT(SIMU_VCCSEL1_PIN));
    } else if (volt == ICC_VOLT_3P3V) {
        GPIO_SetBits(SIMU_VCCSEL0_DEV, BIT(SIMU_VCCSEL0_PIN));
        GPIO_ResetBits(SIMU_VCCSEL1_DEV, BIT(SIMU_VCCSEL1_PIN));
    } else {

    }
#else
    (void)volt;
#endif
}

MOD_LOCAL void sc_simu_pinopt_power_setting(int val)
{
    if(val)
    {
        GPIO_SetBits(SIMU_VCC_DEV, BIT(SIMU_VCC_PIN));
    }else{
        GPIO_ResetBits(SIMU_VCC_DEV, BIT(SIMU_VCC_PIN));
    }
}

MOD_LOCAL void sc_simu_pinopt_rst_setting(unsigned int terminal, int val)
{
    if (terminal==SIMU1) {
#if 1//def SIMU1_RST_DEV
        if(val)
        {
            GPIO_SetBits(SIMU1_RST_DEV, BIT(SIMU1_RST_PIN));
        }else{
            GPIO_ResetBits(SIMU1_RST_DEV, BIT(SIMU1_RST_PIN));
        }
#endif
    } else {
#if 1//def SIMU2_RST_DEV
        if(val)
        {
            GPIO_SetBits(SIMU2_RST_DEV, BIT(SIMU2_RST_PIN));
        }else{
            GPIO_ResetBits(SIMU2_RST_DEV, BIT(SIMU2_RST_PIN));
        }
#endif
    }
}

MOD_LOCAL void sc_simu_pinopt_io_trigger(unsigned int terminal, GPIO_CallBackType func, int enable)
{
    if (terminal==SIMU1) {
#if 1//def SIMU1_IO_PORT
        if (enable) {
            if (func != NULL) {
                gpio_isr_install(SIMU1_IO_PORT, SIMU1_IO_PIN, func);
            }
            gpio_enable_interrupt(SIMU1_IO_PORT, SIMU1_IO_PIN, GPIO_INT_TRIGGER_FALLING);
            sc_simu_pinopt_io_val(terminal);
        } else {
            gpio_disable_interrupt(SIMU1_IO_PORT, SIMU1_IO_PIN);
        }
#endif
    } else {
#if 1//def SIMU2_IO_PORT
        if (enable) {
            if (func != NULL) {
                gpio_isr_install(SIMU2_IO_PORT, SIMU2_IO_PIN, func);
            }
            gpio_enable_interrupt(SIMU2_IO_PORT, SIMU2_IO_PIN, GPIO_INT_TRIGGER_FALLING);
            sc_simu_pinopt_io_val(terminal);
        } else {
            gpio_disable_interrupt(SIMU2_IO_PORT, SIMU2_IO_PIN);
        }
#endif
    }
}

MOD_LOCAL void sc_simu_pinopt_io_setting(unsigned int terminal, int val)
{
    if (terminal==SIMU1) {
#if 1//def SIMU1_IO_DEV
        if(val)
        {
            GPIO_SetBits(SIMU1_IO_DEV, BIT(SIMU1_IO_PIN));
        }else{
            GPIO_ResetBits(SIMU1_IO_DEV, BIT(SIMU1_IO_PIN));
        }
#endif
    } else {
#if 1//def SIMU2_IO_DEV
        if(val)
        {
            GPIO_SetBits(SIMU2_IO_DEV, BIT(SIMU2_IO_PIN));
        }else{
            GPIO_ResetBits(SIMU2_IO_DEV, BIT(SIMU2_IO_PIN));
        }
#endif
    }
}

MOD_LOCAL int sc_simu_pinopt_io_val(unsigned int terminal)
{
    if (terminal==SIMU1) {
#if 1//def SIMU1_IO_DEV
        return GPIO_ReadInputDataBit(SIMU1_IO_DEV, BIT(SIMU1_IO_PIN));
#else
        return 1;
#endif
    } else {
#if 1//def SIMU2_IO_DEV
        return GPIO_ReadInputDataBit(SIMU2_IO_DEV, BIT(SIMU2_IO_PIN));
#else
        return 1;
#endif
    }
}

MOD_LOCAL void sc_simu_pinopt_deinit(void)
{
    GPIO_InitTypeDef gpio_conf;

    gpio_conf.GPIO_Remap = GPIO_Remap_1;
    gpio_conf.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_conf.GPIO_Pin = BIT(SIMU_VCC_PIN);
    GPIO_Init(SIMU_VCC_DEV, &gpio_conf);
    GPIO_ResetBits(SIMU_VCC_DEV, BIT(SIMU_VCC_PIN));

    gpio_conf.GPIO_Mode = GPIO_Mode_Out_PP;

#ifdef SIMU_SLOT_DEV
    gpio_conf.GPIO_Pin = BIT(SIMU_SLOT_PIN);
    GPIO_Init(SIMU_SLOT_DEV, &gpio_conf);
    GPIO_ResetBits(SIMU_SLOT_DEV, BIT(SIMU_SLOT_PIN));
#endif

#if defined(SIMU_VCCSEL0_DEV) && defined(SIMU_VCCSEL1_DEV)
    gpio_conf.GPIO_Pin = BIT(SIMU_VCCSEL0_PIN);
    GPIO_Init(SIMU_VCCSEL0_DEV, &gpio_conf);
    GPIO_ResetBits(SIMU_VCCSEL0_DEV, BIT(SIMU_VCCSEL0_PIN));

    gpio_conf.GPIO_Pin = BIT(SIMU_VCCSEL1_PIN);
    GPIO_Init(SIMU_VCCSEL1_DEV, &gpio_conf);
    GPIO_ResetBits(SIMU_VCCSEL1_DEV, BIT(SIMU_VCCSEL1_PIN));
#endif


#if 1//def SIMU1_RST_DEV
    gpio_conf.GPIO_Pin = BIT(SIMU1_RST_PIN);
    GPIO_Init(SIMU1_RST_DEV, &gpio_conf);
    GPIO_ResetBits(SIMU1_RST_DEV, BIT(SIMU1_RST_PIN));
#endif
#if 1//def SIMU2_RST_DEV
    gpio_conf.GPIO_Pin = BIT(SIMU2_RST_PIN);
    GPIO_Init(SIMU2_RST_DEV, &gpio_conf);
    GPIO_ResetBits(SIMU2_RST_DEV, BIT(SIMU2_RST_PIN));
#endif

    gpio_conf.GPIO_Pin = BIT(SIMU1_CLK_PIN);
    GPIO_Init(SIMU1_CLK_DEV, &gpio_conf);
    GPIO_ResetBits(SIMU1_CLK_DEV, BIT(SIMU1_CLK_PIN));

    gpio_conf.GPIO_Pin = BIT(SIMU2_CLK_PIN);
    GPIO_Init(SIMU2_CLK_DEV, &gpio_conf);
    GPIO_ResetBits(SIMU1_CLK_DEV, BIT(SIMU2_CLK_PIN));

#if 1//def SIMU1_IO_DEV
    gpio_conf.GPIO_Pin = BIT(SIMU1_IO_PIN);
    GPIO_Init(SIMU1_IO_DEV, &gpio_conf);
    GPIO_ResetBits(SIMU1_IO_DEV, BIT(SIMU1_IO_PIN));
#endif
#if 1//def SIMU2_IO_DEV
    gpio_conf.GPIO_Pin = BIT(SIMU2_IO_PIN);
    GPIO_Init(SIMU2_IO_DEV, &gpio_conf);
    GPIO_ResetBits(SIMU2_IO_DEV, BIT(SIMU2_IO_PIN));
#endif
}





