#ifndef __SC_PIN_OPT__
#define __SC_PIN_OPT__

#include "base.h"
#include "psam_config.h"
#include "hal_gpio_int.h"
#define SC_SIMU_ENABLE

#ifdef SC_SIMU_ENABLE
#define SIMU_VCC_DEV         GPIOE   //PSAM_EN
#define SIMU_VCC_PIN         0

#define SIMU1_CLK_DEV        GPIOE   //PSAM1_CLK
#define SIMU1_CLK_PIN        10

#define SIMU2_CLK_DEV        GPIOC   //PSAM2_CLK
#define SIMU2_CLK_PIN        9

#define SIMU1_RST_DEV       GPIOE   //PSAM1_RST
#define SIMU1_RST_PIN       11

#define SIMU1_IO_DEV        GPIOE   //PSAM1_IO
#define SIMU1_IO_PORT       nGPE
#define SIMU1_IO_PIN        12

#define SIMU2_RST_DEV       GPIOD   //PSAM2_RST
#define SIMU2_RST_PIN       6

#define SIMU2_IO_DEV        GPIOC   //PSAM2_IO
#define SIMU2_IO_PORT       nGPC
#define SIMU2_IO_PIN        0

#else
#error "sc_pin_opt SC_SIMU_ENABLE undefined products"
#endif

typedef callback_isr_cb GPIO_CallBackType;

MOD_LOCAL void sc_pinopt_channel_select(int slot);

#ifdef SC_SIMU_ENABLE
MOD_LOCAL void sc_simu_pinopt_init(void);
MOD_LOCAL void sc_simu_pinopt_volt_select(int volt);
MOD_LOCAL void sc_simu_pinopt_power_setting(int val);
MOD_LOCAL void sc_simu_pinopt_rst_setting(unsigned int terminal, int val);
MOD_LOCAL void sc_simu_pinopt_io_trigger(unsigned int terminal, GPIO_CallBackType func, int enable);
MOD_LOCAL void sc_simu_pinopt_io_setting(unsigned int terminal, int val);
MOD_LOCAL int sc_simu_pinopt_io_val(unsigned int terminal);
MOD_LOCAL void sc_simu_pinopt_deinit(void);
#endif
#endif
