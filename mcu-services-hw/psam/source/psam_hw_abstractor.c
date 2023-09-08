#include "psam_type.h"
#include "psam_pin_opt.h"
#include "psam_hw_abstractor.h"
#include "psam_config.h"
enum ICC_PIN {
    PIN_VCC = 1,
    PIN_RST,
    PIN_CLK,
    PIN_C4,
    PIN_GND,
    PIN_C6,
    PIN_IO,
    PIN_C8,
};

enum ICC_PIN_CFG {
    PIN_CFG_DIS = 0,
    PIN_CFG_EN = 1<<0,
    PIN_CFG_OUT = 1<<1,
    PIN_CFG_IN = 1<<2,
    PIN_CFG_INT = 1<<3,
};

enum {
    ETU_TIMER_DIS = 0,
    ETU_TIMER_1,
    ETU_TIMER_1P5,
    ETU_TIMER_xPy,
};

enum {
    GP_USE_DIS = 0,
    GP_USE_TSWT,
    GP_USE_ATRWT,
    GP_USE_WWT,
    GP_USE_CWT,
    GP_USE_BWT,
    GP_USE_GT,
    GP_USE_CGT,
    GP_USE_BGT,
};

extern unsigned char g_curslot;
extern sc_hw_abs_t g_sc_abs[SC_TERMINAL_NUM];
static volatile unsigned char g_psam_slot = SIMU1;
static volatile unsigned char g_gp_user = GP_USE_DIS;

MOD_LOCAL unsigned char sc_byte_format_convert(unsigned char byte)
{
    int i= 0;
    unsigned char convert = 0;

    for (i=0; i<8; i++) {
        if (!(byte & (1 << (7-i)))) {
            convert |= 1<<i;
        }
    }
    return convert;
}

static unsigned char sc_byte_parity(unsigned char byte)
{
    int i= 0;
    unsigned char parity = 0;

    for (i=0; i<8; i++) {
        if ((byte >> i) & 0x01)
            parity++;
    }
    return (parity%2);
}

static void sc_hw_abs_pin_write(sc_hw_abs_t *abs, int pin, int val)
{
        if (pin == PIN_VCC) {
            sc_simu_pinopt_power_setting(val);
        } else if (pin == PIN_RST) {
            sc_simu_pinopt_rst_setting(abs->status->terminal,val);
        } else if (pin == PIN_IO) {
            sc_simu_pinopt_io_setting(abs->status->terminal,val);
        }
}

static int sc_hw_abs_pin_read(sc_hw_abs_t *abs, int pin)
{
    (void)abs;
    if (pin == PIN_IO) {
        return sc_simu_pinopt_io_val(abs->status->terminal);
    }
    return 0;
}

#define SIMU_CLK_INIT_MASK      (1<<7)
#define SIMU1_CLK_RUNING_MASK   (1<<0)
#define SIMU2_CLK_RUNING_MASK   (1<<1)
static volatile unsigned char s_sc_simu_clk_sta = 0;

static void sc_hw_simu_clock_init(sc_hw_abs_t *abs)
{
    unsigned int pwm_low;
    unsigned int pwm_high;
    unsigned long apb_freq;

      TIM_PWMInitTypeDef tmr_config;

    (void) abs;
    if (s_sc_simu_clk_sta & SIMU_CLK_INIT_MASK) {
        return ;
    }

    apb_freq = get_apb_frequency();
    tmr_config.TIMx = SIMU_CLK_TMR_DEV;
    pwm_high = ((apb_freq/SIMU_CLK_HZ)/2 - 1);
    pwm_low = (((apb_freq/SIMU_CLK_HZ)+1)/2 - 1);
    TIM_ITConfig(TIMM0, SIMU_CLK_TMR_DEV, DISABLE);
    TIM_ClearITPendingBit(TIMM0, SIMU_CLK_TMR_DEV);
    TIM_PWMInit(TIMM0, &tmr_config);
    TIM_ModeConfig(TIMM0, SIMU_CLK_TMR_DEV, TIM_Mode_PWM);
    TIM_SetPWMPeriod(TIMM0, SIMU_CLK_TMR_DEV, pwm_low, pwm_high);
    s_sc_simu_clk_sta |= SIMU_CLK_INIT_MASK;
}

static void sc_hw_simu_clock_start(sc_hw_abs_t *abs)
{
    if (!(s_sc_simu_clk_sta & (SIMU1_CLK_RUNING_MASK | SIMU2_CLK_RUNING_MASK))) {
        TIM_Cmd(TIMM0, SIMU_CLK_TMR_DEV, ENABLE);
    }

    if (abs->status->terminal == SIMU1) {
        s_sc_simu_clk_sta |= SIMU1_CLK_RUNING_MASK;
    } else {
        s_sc_simu_clk_sta |= SIMU2_CLK_RUNING_MASK;
    }
    abs->status->clkstop = ICC_CLK_RUNING;
}

static void sc_hw_simu_clock_stop(sc_hw_abs_t *abs)
{
    if (abs->status->terminal == SIMU1) {
        s_sc_simu_clk_sta &= ~SIMU1_CLK_RUNING_MASK;
    } else {
        s_sc_simu_clk_sta &= ~SIMU2_CLK_RUNING_MASK;
    }
    abs->status->clkstop = ICC_CLK_STOP;

    if (!(s_sc_simu_clk_sta & (SIMU1_CLK_RUNING_MASK | SIMU2_CLK_RUNING_MASK))) {
        TIM_Cmd(TIMM0, SIMU_CLK_TMR_DEV, DISABLE);
    }
}

static void sc_hw_simu_clock_deinit(sc_hw_abs_t *abs)
{
    (void) abs;
    TIM_Cmd(TIMM0, SIMU_CLK_TMR_DEV, DISABLE);
    s_sc_simu_clk_sta &= ~SIMU_CLK_INIT_MASK;
}


#define SIMU1_VCC_RUNING_MASK   (1<<0)
#define SIMU2_VCC_RUNING_MASK   (1<<1)
static volatile unsigned char s_sc_simu_vcc_sta = 0;

static void sc_hw_simu_vcc_powon(sc_hw_abs_t *abs)
{
    if (abs->status->terminal == SIMU1) {
        s_sc_simu_vcc_sta |= SIMU1_VCC_RUNING_MASK;
    } else {
        s_sc_simu_vcc_sta |= SIMU2_VCC_RUNING_MASK;
    }
    sc_hw_abs_pin_write(abs, PIN_VCC, 1);
}

static void sc_hw_simu_vcc_powdown(sc_hw_abs_t *abs)
{
    if (abs->status->terminal == SIMU1) {
        s_sc_simu_vcc_sta &= ~SIMU1_VCC_RUNING_MASK;
    } else {
        s_sc_simu_vcc_sta &= ~SIMU2_VCC_RUNING_MASK;
    }
    if (!(s_sc_simu_vcc_sta & (SIMU1_VCC_RUNING_MASK | SIMU2_VCC_RUNING_MASK))) {
        sc_hw_abs_pin_write(abs, PIN_VCC, 0);
    }
}

enum ICC_BYTE_STEP {
    ICC_BYTE_STEP_START_DET = -1,       //rcv
    ICC_BYTE_STEP_START_BIT = -1,       //xmt
    ICC_BYTE_STEP_DATA_FIRST_BIT = 0,   //both
    ICC_BYTE_STEP_DATA_LAST_BIT = 7,    //both
    ICC_BYTE_STEP_PARITY_BIT = 8,       //both
    ICC_BYTE_STEP_STOP_BIT = 9,         //
    ICC_BYTE_STEP_SEND_NACK_BIT = ICC_BYTE_STEP_STOP_BIT,
    ICC_BYTE_STEP_RCV_NACK_BIT = 10,
    ICC_BYTE_STEP_SEND_NACK_END_BIT = ICC_BYTE_STEP_RCV_NACK_BIT,
    ICC_BYTE_STEP_END = 11,             //
};

enum {
    TRIGGER_DIS = 0,
    TRIGGER_FALLING,
};

static void sc_hw_simu_etu_tmr_isr(void)
{
    sc_hw_abs_t *abs = &g_sc_abs[g_curslot];

    if (abs->simu->etu_irq.thread) {
        abs->simu->etu_irq.thread(abs);
    }
    TIM_ClearITPendingBit(TIMM0, SIMU_ETU_TMR_DEV);
    NVIC_ClearPendingIRQ(SIMU_ETU_TMR_DEV_IRQn);
}

static void sc_hw_simu_etu_timer_init(sc_hw_abs_t *abs)
{
    TIM_InitTypeDef tmr_config;
    (void) abs;
    tmr_config.TIMx = SIMU_ETU_TMR_DEV;
    tmr_config.TIM_Period = 0;
    TIM_Init(TIMM0, &tmr_config);
    TIM_ITConfig(TIMM0, SIMU_ETU_TMR_DEV, ENABLE);
    TIM_ClearITPendingBit(TIMM0, SIMU_ETU_TMR_DEV);
    NVIC_ClearPendingIRQ(SIMU_ETU_TMR_DEV_IRQn);
      tim_isr_install(TIM0_GROUP5, sc_hw_simu_etu_tmr_isr);
}

static void sc_hw_simu_etu_timer_request(sc_hw_abs_t *abs, int type, int etu, sc_simu_cb_t callback)
{
    unsigned int apb_freq;
    unsigned int timer_clk_khz;
    unsigned int timer_cnt_per_etu;
    unsigned int fraction;

    if (type == ETU_TIMER_DIS) {
        TIM_Cmd(TIMM0, SIMU_ETU_TMR_DEV, DISABLE);
        TIM_ClearITPendingBit(TIMM0, SIMU_ETU_TMR_DEV);
        return ;
    }

    if (!abs->param->di) {
        return ;
    }
    //sc_debug("apb_freq=%d fi=%d, di=%d \n", abs->param->fi, abs->param->di);
    apb_freq = get_apb_frequency();
    timer_clk_khz = apb_freq/1000;
    timer_cnt_per_etu = timer_clk_khz*abs->param->fi/abs->param->di/SIMU_CLK_KHZ;

    abs->simu->etu_irq.completed = false;
    abs->simu->etu_irq.data = etu;
    abs->simu->etu_irq.thread = callback;

    if (type == ETU_TIMER_1) {
        fraction = 0x0;
        TIM_SetPeriod(TIMM0, SIMU_ETU_TMR_DEV, timer_cnt_per_etu);
        TIM_Cmd(TIMM0, SIMU_ETU_TMR_DEV, ENABLE);
        TIM_ClearITPendingBit(TIMM0, SIMU_ETU_TMR_DEV);
        NVIC_ClearPendingIRQ(SIMU_ETU_TMR_DEV_IRQn);
    } else if (type == ETU_TIMER_1P5) {
        if (abs->param->di == 1) {
            fraction = timer_cnt_per_etu*4/10;
        } else if (abs->param->di == 2) {
            fraction = timer_cnt_per_etu*3/10;
        } else if (abs->param->di == 4) {
            fraction = timer_cnt_per_etu*2/10;
        } else {
            fraction = timer_cnt_per_etu*2/10;
        }

        fraction = (timer_cnt_per_etu + fraction);
        TIM_SetPeriod(TIMM0, SIMU_ETU_TMR_DEV, fraction);
        TIM_Cmd(TIMM0, SIMU_ETU_TMR_DEV, ENABLE);
        TIM_ClearITPendingBit(TIMM0, SIMU_ETU_TMR_DEV);
        NVIC_ClearPendingIRQ(SIMU_ETU_TMR_DEV_IRQn);
        TIM_SetPeriod(TIMM0, SIMU_ETU_TMR_DEV, timer_cnt_per_etu);
    }
}

static void sc_hw_simu_etu_timer_deinit(sc_hw_abs_t *abs)
{
    (void)abs;
    TIM_Cmd(TIMM0, SIMU_ETU_TMR_DEV, DISABLE);
}

static void sc_hw_simu_etu_timer_wait_isr(sc_hw_abs_t *abs)
{
    if (abs->simu->etu_irq.data-- == 0) {
        abs->simu->etu_irq.completed = true;
    }
}

static int sc_hw_simu_etu_timer_wait_completed(sc_hw_abs_t *abs)
{
    int uiMaxLoops;

    uiMaxLoops = get_system_frequency();
    uiMaxLoops *= 5;
    while (!abs->simu->etu_irq.completed && uiMaxLoops-- > 0);

    if (uiMaxLoops > 0) {
        return SC_ERR_NONE;
    }
    return SC_ERR_TIME_VIOLATE;
}

static void sc_hw_simu_byte_transmit(sc_hw_abs_t *abs);
static void sc_hw_simu_byte_receive(sc_hw_abs_t *abs);

static void sc_hw_simu_retrans_gt(sc_hw_abs_t *abs)
{
    if (abs->simu->etu_irq.data == 0) {
        abs->simu->byte_step = ICC_BYTE_STEP_START_BIT;
        abs->simu->etu_irq.thread = sc_hw_simu_byte_transmit;
    } else {
        abs->simu->etu_irq.data--;
    }
}

static void sc_hw_simu_trans_wt(sc_hw_abs_t *abs)
{
    if (abs->simu->etu_irq.data == 0) {
        sc_errinfo("sc_hw_simu_trans_wt, wwt=%u, cwt=%u, bwt=%u\r\n", abs->param->wwt, abs->param->cwt, abs->param->bwt);
        abs->status->errno = SC_ERR_TIME_VIOLATE;
        abs->queue->fill(abs->queue, NULL, 0, abs->status->errno);
        sc_hw_simu_etu_timer_request(abs, ETU_TIMER_DIS, 0, NULL);
    } else {
        abs->simu->etu_irq.data--;
        if (abs->status->state == ICC_STATE_RESETING) {
            abs->param->atrwt--;
            if (abs->param->atrwt <= 0) {
                sc_errinfo("sc_hw_simu_trans_wt: atrwt \r\n");
                abs->status->errno = SC_ERR_TIME_VIOLATE;
                abs->queue->fill(abs->queue, NULL, 0, abs->status->errno);
                sc_hw_simu_etu_timer_request(abs, ETU_TIMER_DIS, 0, NULL);
            }
        }
    }
}

static void sc_hw_simu_gpio_isr(void)
{
    sc_hw_abs_t *abs = &g_sc_abs[g_curslot];

    if (abs->simu->io_irq.thread) {
        abs->simu->io_irq.thread(abs);
    }
}

static void sc_hw_simu_regester_io_irq(sc_hw_abs_t *abs, int en, void(*callback)(sc_hw_abs_t *))
{
    if (en) {
        abs->simu->byte_step = ICC_BYTE_STEP_START_DET;
        abs->simu->io_irq.thread = callback;
    }
    sc_simu_pinopt_io_trigger(abs->status->terminal ,sc_hw_simu_gpio_isr, en);
}

static void sc_hw_simu_found_start_bit_isr(sc_hw_abs_t *abs)
{
    sc_debug("F: start_bit_isr \r\n");
    if (abs->simu->byte_step == ICC_BYTE_STEP_START_DET) {
        sc_hw_simu_etu_timer_request(abs, ETU_TIMER_DIS, 0, NULL);
        sc_hw_simu_etu_timer_request(abs, ETU_TIMER_1P5, 1, sc_hw_simu_byte_receive);
        sc_hw_simu_regester_io_irq(abs, TRIGGER_DIS, NULL);
        abs->simu->byte_step = ICC_BYTE_STEP_DATA_FIRST_BIT;
        abs->simu->curbyte = 0x0;
    } else {
        abs->status->errno = SC_ERR_BYTE_STEP;
        abs->queue->fill(abs->queue, NULL, 0, abs->status->errno);
    }
}

/* hardware unreference */
static void sc_hw_simu_rcved_setting(sc_hw_abs_t *abs)
{
    sc_hw_abs_pin_write(abs, PIN_IO, 1);
    sc_hw_abs_pin_read(abs, PIN_IO);
    sc_hw_simu_regester_io_irq(abs, TRIGGER_DIS, NULL);
    sc_hw_simu_etu_timer_request(abs, ETU_TIMER_DIS, 0, NULL);
}

static int sc_hw_simu_init(sc_hw_abs_t *abs)
{
    if (abs->simu == NULL) {
        sc_debug("ERR NULL \r\n");
        return SC_ERR_NOINIT;
    }

    sc_hw_simu_etu_timer_init(abs);
    sc_hw_simu_etu_timer_request(abs, ETU_TIMER_DIS, 0, NULL);
    return SC_ERR_NONE;
}

static int sc_hw_simu_active(sc_hw_abs_t *abs, int type)
{
    if (type == ICC_RESET_COLD) {
        sc_simu_pinopt_volt_select(abs->cfg->volt);
        udelay(50);
        sc_hw_abs_pin_write(abs, PIN_RST, 0);
        //sc_hw_abs_pin_write(abs, PIN_VCC, 1);
        sc_hw_simu_vcc_powon(abs);
        udelay(10);
        sc_hw_abs_pin_write(abs, PIN_IO, 1);
        sc_hw_simu_clock_start(abs);
    } else {
        sc_hw_abs_pin_write(abs, PIN_RST, 0);
    }
    sc_hw_simu_etu_timer_request(abs, ETU_TIMER_1,
            DEF_RST_ETU*abs->param->di, sc_hw_simu_etu_timer_wait_isr);
    if (sc_hw_simu_etu_timer_wait_completed(abs) == SC_ERR_TIME_VIOLATE) {
        return SC_ERR_TIME_VIOLATE;
    }
    sc_hw_simu_etu_timer_request(abs, ETU_TIMER_DIS, 0, NULL);
    sc_hw_abs_pin_write(abs, PIN_RST, 1);

#ifndef SIMU_IO_INT_DEFECT
    sc_hw_simu_regester_io_irq(abs, TRIGGER_FALLING, sc_hw_simu_found_start_bit_isr);
#endif
    sc_hw_simu_etu_timer_request(abs, ETU_TIMER_1, abs->param->wwt, sc_hw_simu_trans_wt);

    return SC_ERR_NONE;
}

#ifdef SIMU_IO_INT_DEFECT
static int sc_hw_simu_wait_reveive(sc_hw_abs_t *abs)
{
    while (sc_hw_abs_pin_read(abs, PIN_IO) == 1) {
        if (abs->status->errno != SC_ERR_NONE) {
            return abs->status->errno;
        }
    }

    while (sc_hw_abs_pin_read(abs, PIN_IO) == 1) {
        if (abs->status->errno != SC_ERR_NONE) {
            return abs->status->errno;
        }
    }

    abs->simu->byte_step = ICC_BYTE_STEP_START_DET;
    sc_hw_simu_found_start_bit_isr(abs);

    while (abs->simu->byte_step != ICC_BYTE_STEP_END) {
        if (abs->status->errno != SC_ERR_NONE) {
            return abs->status->errno;
        }
    }
    return SC_ERR_NONE;
}
#endif

static void sc_hw_simu_byte_transmit(sc_hw_abs_t *abs)
{
    unsigned char ch = 0;
    sc_hw_simu_t *simu = abs->simu;

    //sc_debug("byte_transmit, step=%d\r\n", simu->byte_step);
    if (simu->byte_step == ICC_BYTE_STEP_START_BIT) {
        sc_hw_abs_pin_write(abs, PIN_IO, 0);
        simu->byte_step++;
    } else if (simu->byte_step >= ICC_BYTE_STEP_DATA_FIRST_BIT
                && simu->byte_step <= ICC_BYTE_STEP_DATA_LAST_BIT) {
        if (simu->curbyte & (1 << simu->byte_step)) {
            sc_hw_abs_pin_write(abs, PIN_IO, 1);
        } else {
            sc_hw_abs_pin_write(abs, PIN_IO, 0);
        }
        simu->byte_step++;
    } else if (simu->byte_step == ICC_BYTE_STEP_PARITY_BIT) {
        sc_hw_abs_pin_write(abs, PIN_IO, simu->parity);
        simu->byte_step++;
    } else if (simu->byte_step == ICC_BYTE_STEP_STOP_BIT) {
        sc_hw_abs_pin_write(abs, PIN_IO, 1);
        if (abs->status->protocol == ICC_PROTOCOL_T0) {
            simu->byte_step = ICC_BYTE_STEP_RCV_NACK_BIT;
        } else {
            simu->byte_step = ICC_BYTE_STEP_END;
        }
    } else if (simu->byte_step == ICC_BYTE_STEP_RCV_NACK_BIT) {
        ch = sc_hw_abs_pin_read(abs, PIN_IO);
        if (ch == 0) {
            /*Get NACK, Wait 3 etu, then retransmit */
            abs->status->retrans_error_cnt--;
            if (abs->status->retrans_error_cnt > 0) {
                sc_hw_simu_etu_timer_request(abs, ETU_TIMER_1, 3, sc_hw_simu_retrans_gt);
            } else {
                sc_hw_simu_etu_timer_request(abs, ETU_TIMER_DIS, 0, NULL);
                abs->status->errno = SC_ERR_NACK_EXCEED;
                abs->queue->fill(abs->queue, NULL, 0, abs->status->errno);
                simu->byte_step = ICC_BYTE_STEP_END;
            }
        } else {
            simu->byte_step = ICC_BYTE_STEP_END;
            sc_hw_simu_etu_timer_request(abs, ETU_TIMER_DIS, 0, NULL);
        }
    }
}

static void sc_hw_simu_byte_receive(sc_hw_abs_t *abs)
{
    int errno = SC_ERR_NONE;
    unsigned char ch = 0;
    sc_hw_simu_t *simu = abs->simu;

    if ((simu->byte_step >= ICC_BYTE_STEP_DATA_FIRST_BIT)
        && (simu->byte_step <= ICC_BYTE_STEP_DATA_LAST_BIT)) {
        ch = sc_hw_abs_pin_read(abs, PIN_IO);
        simu->curbyte |= (ch << simu->byte_step);
        simu->byte_step++;
    } else if (abs->simu->byte_step == ICC_BYTE_STEP_PARITY_BIT) {
        ch = sc_hw_abs_pin_read(abs, PIN_IO);
        if (abs->param->format == ICC_FORMAT_CONV) {
            ch = ((~ch) & 0x01);
            simu->curbyte = sc_byte_format_convert(simu->curbyte);
        }
        simu->parity = sc_byte_parity(simu->curbyte);
        if (ch != simu->parity) {
            sc_debug("\nSC_ERR_BYTE_PARITY, curbyte=%02x, ch=%d, parity=%d, format=%d, abs->status->protocol=%d \n",
                    simu->curbyte, ch, simu->parity, abs->param->format, abs->status->protocol);
            if (abs->status->protocol == ICC_PROTOCOL_T1) {
                errno = SC_ERR_BYTE_PARITY;
                sc_hw_simu_etu_timer_request(abs, ETU_TIMER_DIS, 0, NULL);
            } else {
                abs->status->retrans_error_cnt--;
                if (abs->status->retrans_error_cnt == 0) {
                    errno = SC_ERR_NACK_EXCEED;
                    sc_hw_simu_etu_timer_request(abs, ETU_TIMER_DIS, 0, NULL);
                }
            }
        } else if (abs->status->protocol == ICC_PROTOCOL_T0) {
            abs->status->retrans_error_cnt = T0_BYTE_RETRANS_MAX;
        }
        simu->byte_step++;
    } else if (simu->byte_step == ICC_BYTE_STEP_STOP_BIT) {
        // Also ICC_BYTE_STEP_SEND_NACK_BIT
        ch = sc_hw_abs_pin_read(abs, PIN_IO);
        if (ch == 0) {
            errno = SC_ERR_NO_STOP_BIT;
            sc_debug("\nSC_ERR_NO_STOP_BIT,burbyte=%02x, format=%d, abs->status->protocol=%d \n",
                    simu->curbyte, abs->param->format, abs->status->protocol);
            sc_hw_simu_etu_timer_request(abs, ETU_TIMER_DIS, 0, NULL);
        }
        if (abs->status->protocol == ICC_PROTOCOL_T0) {
            if (abs->status->retrans_error_cnt < T0_BYTE_RETRANS_MAX) {
                // Send NACK
                sc_hw_abs_pin_write(abs, PIN_IO, 0);
                sc_hw_simu_etu_timer_request(abs, ETU_TIMER_1P5, 0, sc_hw_simu_byte_receive);
                simu->byte_step = ICC_BYTE_STEP_SEND_NACK_END_BIT;
            } else {
                simu->byte_step = ICC_BYTE_STEP_END;
            }
        } else {
            simu->byte_step = ICC_BYTE_STEP_END;
        }
    } else if (simu->byte_step == ICC_BYTE_STEP_SEND_NACK_END_BIT) {
        sc_hw_abs_pin_write(abs, PIN_IO, 1);
        sc_hw_simu_etu_timer_request(abs, ETU_TIMER_DIS, 0, NULL);
        sc_hw_simu_regester_io_irq(abs, TRIGGER_FALLING, sc_hw_simu_found_start_bit_isr);
    }

    if (errno != SC_ERR_NONE) {
        abs->status->errno = errno;
        abs->queue->fill(abs->queue, NULL, 0, abs->status->errno);
    } else if (simu->byte_step == ICC_BYTE_STEP_END) {
#ifndef SIMU_IO_INT_DEFECT
        sc_hw_simu_regester_io_irq(abs, TRIGGER_FALLING, sc_hw_simu_found_start_bit_isr);
#endif
        ch = simu->curbyte;
        sc_debug("R:%02X", ch);
        abs->queue->fill(abs->queue, &ch, 1, abs->status->errno);
        if (abs->status->protocol == ICC_PROTOCOL_T1) {
            sc_hw_simu_etu_timer_request(abs, ETU_TIMER_1, abs->param->cwt, sc_hw_simu_trans_wt);
        } else {
            sc_hw_simu_etu_timer_request(abs, ETU_TIMER_1, abs->param->wwt, sc_hw_simu_trans_wt);
        }
    }
}

static int sc_hw_simu_packet_transmit(sc_hw_abs_t *abs)
{
    int length;
    unsigned char ch = 0;
    sc_hw_simu_t *simu = abs->simu;

    //sc_debug("sc_hw_simu_packet_transmit \r\n");
    sc_hw_simu_regester_io_irq(abs, TRIGGER_DIS, NULL);
    if (abs->status->protocol == ICC_PROTOCOL_T0) {
        /* T=0 Uart7816 no BGT inserted */
        if(abs->cfg->spec == ICC_SPEC_EMV)
            udelay(6*1000*abs->param->fi/abs->param->di/SIMU_CLK_KHZ);
        else
            udelay((abs->param->bgt-6)*1000*abs->param->fi/abs->param->di/SIMU_CLK_KHZ);
    } else {
        udelay(abs->param->bgt*1000*abs->param->fi/abs->param->di/SIMU_CLK_KHZ);
    }

    sc_if_debug((abs->param->di == 1), "TX: ");
    abs->param->lrc = 0x0;
    for ( ; ; ) {
        if (abs->status->errno != SC_ERR_NONE)
            return SC_ERR_FAIL;
        length = abs->queue->tunnel(abs->queue, &ch, 1);
        if (length > 0) {
            simu->curbyte = ch;
            if (abs->status->protocol == ICC_PROTOCOL_T1) {
                abs->param->lrc ^= simu->curbyte;
            } else {
                abs->status->retrans_error_cnt = T0_BYTE_RETRANS_MAX;
            }
            sc_if_debug((abs->param->di == 1), "%02X ", ch);
            simu->parity = sc_byte_parity(simu->curbyte);
            if (abs->param->format == ICC_FORMAT_CONV) {
                simu->parity = ((~simu->parity) & 0x01);
                simu->curbyte = sc_byte_format_convert(simu->curbyte);
            }
            simu->byte_step = ICC_BYTE_STEP_START_BIT;
            sc_hw_simu_etu_timer_request(abs, ETU_TIMER_1, 1, sc_hw_simu_byte_transmit);
            while ((simu->byte_step < ICC_BYTE_STEP_END) && (abs->status->errno == SC_ERR_NONE));

            if (abs->queue->tunnel(abs->queue, NULL, 0)) {
                udelay((abs->param->gt-10)*1000*abs->param->fi/abs->param->di/PSAM_CLK_KHZ);
                sc_hw_simu_etu_timer_request(abs, ETU_TIMER_1, 1, sc_hw_simu_etu_timer_wait_isr);
                if (sc_hw_simu_etu_timer_wait_completed(abs) == SC_ERR_TIME_VIOLATE) {
                    return SC_ERR_TIME_VIOLATE;
                }
            }
        } else {
            break;
        }
    }

    if (abs->status->protocol == ICC_PROTOCOL_T1) {
        udelay((abs->param->gt-10)*1000*abs->param->fi/abs->param->di/SIMU_CLK_KHZ);
        simu->curbyte = abs->param->lrc;
        simu->parity = sc_byte_parity(simu->curbyte);
        if (abs->param->format == ICC_FORMAT_CONV) {
            simu->curbyte = sc_byte_format_convert(simu->curbyte);
            simu->parity = ((!simu->parity) & 0x01);
        }
        sc_if_debug((abs->param->di == 1), "%02X ", simu->curbyte);
        simu->byte_step = ICC_BYTE_STEP_START_BIT;
        sc_hw_simu_etu_timer_request(abs, ETU_TIMER_1, 1, sc_hw_simu_byte_transmit);
        while ((simu->byte_step < ICC_BYTE_STEP_END) && (abs->status->errno == SC_ERR_NONE));
    }
    sc_if_debug((abs->param->di == 1), "\r\n");

    sc_hw_simu_regester_io_irq(abs, TRIGGER_FALLING, sc_hw_simu_found_start_bit_isr);
    if (abs->status->protocol == ICC_PROTOCOL_T1) {
        sc_hw_simu_etu_timer_request(abs, ETU_TIMER_1, abs->param->bwt, sc_hw_simu_trans_wt);
    } else {
        sc_hw_simu_etu_timer_request(abs, ETU_TIMER_1, abs->param->wwt, sc_hw_simu_trans_wt);
    }

    return SC_ERR_NONE;
}


static int sc_hw_simu_disactive(sc_hw_abs_t *abs)
{
    DelayMs(10);
    abs->status->state = ICC_STATE_POWEROFF;
    sc_hw_simu_regester_io_irq(abs, TRIGGER_DIS, NULL);
    sc_hw_simu_etu_timer_request(abs, ETU_TIMER_DIS, 0, NULL);
    sc_hw_abs_pin_write(abs, PIN_RST, 0);
    udelay(5);
    sc_hw_simu_clock_stop(abs);
    sc_hw_abs_pin_write(abs, PIN_IO, 0);
    udelay(2);
    sc_hw_simu_vcc_powdown(abs);
    //sc_hw_abs_pin_write(abs, PIN_VCC, 0);
    sc_debug("simu disactive\r\n");
    return SC_ERR_NONE;
}

MOD_LOCAL int sc_hw_card_status(sc_hw_abs_t *abs)
{
    abs->status->present = ICC_CARD_PRESENT;

    return abs->status->present;
}

MOD_LOCAL int sc_hw_abs_init(sc_hw_abs_t *abs)
{

    if(abs->status->terminal == SIMU1) {
        sc_simu_pinopt_init();
        sc_hw_simu_init(abs);
        sc_hw_simu_clock_init(abs);
       // Consider for PSAM resume from sleep mode.
        sc_hw_abs_channel_select(g_psam_slot);

    } else if (abs->status->terminal == SIMU2) {
        // Done by SIMU_PSAM1
    } else {
        return SC_ERR_PARAM;
    }
    return SC_ERR_NONE;
}


MOD_LOCAL int sc_hw_abs_detect(sc_hw_abs_t *abs)
{
    if((abs->status->terminal == SIMU1)
            || (abs->status->terminal == SIMU2)) {
        return SC_ERR_NONE;
    } else {
        return SC_ERR_PARAM;
    }
}

MOD_LOCAL int sc_hw_abs_channel_select(int slot)
{
    if (slot < SC_TERMINAL_NUM) {
        if (((slot == SIMU1) || (slot == SIMU2)) && (g_psam_slot != slot)) {
            sc_hw_simu_disactive(&g_sc_abs[g_psam_slot]);
        }
        sc_pinopt_channel_select(slot);
        g_psam_slot = slot;
        return SC_ERR_NONE;
    } else {
        return SC_ERR_PARAM;
    }
}

#ifdef SIMU_IO_INT_DEFECT
MOD_LOCAL int sc_hw_abs_wait_reveive(sc_hw_abs_t *abs)
{
    if (abs->status->terminal == CTRL1) {
        return SC_ERR_NONE;
    } else if (abs->status->terminal == CTRL2) {
        return SC_ERR_NONE;
#ifdef SC_SIMU_ENABLE
    } else if ((abs->status->terminal == SIMU1)
            || (abs->status->terminal == SIMU2)) {
        return sc_hw_simu_wait_reveive(abs);
#endif
    } else {
        return SC_ERR_PARAM;
    }
}
#endif

MOD_LOCAL int sc_hw_abs_active(sc_hw_abs_t *abs, int type)
{
    if((abs->status->terminal == SIMU1)
            || (abs->status->terminal == SIMU2)) {
        return sc_hw_simu_active(abs, type);
    } else {
        return SC_ERR_PARAM;
    }
}

MOD_LOCAL int sc_hw_abs_rcved_setting(sc_hw_abs_t *abs)
{
    if((abs->status->terminal == SIMU1)
            || (abs->status->terminal == SIMU2)) {
        sc_hw_simu_rcved_setting(abs);
        return SC_ERR_NONE;
    } else {
        return SC_ERR_PARAM;
    }
}

MOD_LOCAL int sc_hw_abs_packet_transmit(sc_hw_abs_t *abs)
{
    if((abs->status->terminal == SIMU1)
            || (abs->status->terminal == SIMU2)) {
        return sc_hw_simu_packet_transmit(abs);
    } else {
        return SC_ERR_PARAM;
    }
}

MOD_LOCAL int sc_hw_wait_xmt_complete(sc_hw_abs_t *abs)
{
    if((abs->status->terminal == SIMU1)
            || (abs->status->terminal == SIMU2)) {
        return SC_ERR_NONE;
    } else {
        return SC_ERR_PARAM;
    }
}

MOD_LOCAL int sc_hw_abs_disactive(sc_hw_abs_t *abs)
{
    if((abs->status->terminal == SIMU1)
            || (abs->status->terminal == SIMU2)) {
        return sc_hw_simu_disactive(abs);
    } else {
        return SC_ERR_PARAM;
    }
}

MOD_LOCAL int sc_hw_abs_deinit(sc_hw_abs_t *abs)
{
    if(abs->status->terminal == SIMU1) {
#if (SC_SIMU_NUM == 1)
        sc_hw_simu_clock_deinit(abs);
        sc_hw_simu_etu_timer_deinit(abs);
        sc_simu_pinopt_deinit();
#else
        // Implement by SIMUPSAM2
#endif
        return SC_ERR_NONE;
    } else if (abs->status->terminal == SIMU2) {
#if (SC_SIMU_NUM == 2)
        sc_hw_simu_clock_deinit(abs);
        sc_hw_simu_etu_timer_deinit(abs);
        sc_simu_pinopt_deinit();
#else
        // Not Implement or Implement by SIMUPSAM3
#endif
        return SC_ERR_NONE;
    } else {
        return SC_ERR_PARAM;
    }
}
