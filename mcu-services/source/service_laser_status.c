/*
 * service_laser.c
 *
 * Implementation of MCU laser service module
 * MCU laser service module, this module will run autonomously, SaCP can configure
 *  the mode and mode parameters at any time. Refer to the documentation
 * "SaIP communication protocol between the host and the MCU" for the configuration method.
 *
 *  Created on: Dec 7, 2020
 *      Author: Guo Qiang
 */

#include "service_laser.h"
#include "cmsis_os2.h"
#include "gd32e10x.h"

#include "semantic_api.h"
#include "stdio.h"
#include "uart_io_api.h"

uint16_t ch2_fre;
uint16_t ch3_fre;
uint16_t ch2_ccnumber = 0;
uint16_t ch2_readvalue1 = 0;
uint16_t ch2_readvalue2 = 0;

uint16_t ch3_ccnumber = 0;
uint16_t ch3_readvalue1 = 0;
uint16_t ch3_readvalue2 = 0;

uint32_t ch2_count;
uint32_t ch3_count;
bool laser_flag = false;

static void gpio_configuration(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /*configure as alternate function*/
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
}

static void nvic_configuration(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(TIMER3_IRQn, 1, 1);
}

static void timer3_config(void)
{
    /* TIMER3 configuration: input capture mode -------------------
    the external signal is connected to TIMER3 CH2 pin(PB8)
    the rising edge is used as active edge
    the TIMER3 CH2CV is used to compute the frequency value
    ------------------------------------------------------------ */
    timer_ic_parameter_struct timer_icinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER3);

    timer_deinit(TIMER3);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER2 configuration */
    timer_initpara.prescaler         = 119;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 65535;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER3, &timer_initpara);

    /* TIMER3  configuration */
    /* initialize TIMER channel input parameter struct */
    timer_channel_input_struct_para_init(&timer_icinitpara);
    /* TIMER2 CH0 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_RISING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x0;
    timer_input_capture_config(TIMER3,TIMER_CH_2,&timer_icinitpara);
    timer_input_capture_config(TIMER3,TIMER_CH_3,&timer_icinitpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER3);
    /* clear channel 0 interrupt bit */
    timer_interrupt_flag_clear(TIMER3,TIMER_INT_FLAG_CH2);
    timer_interrupt_flag_clear(TIMER3,TIMER_INT_FLAG_CH3);
    /* channel 0 interrupt enable */
    timer_interrupt_enable(TIMER3,TIMER_INT_CH2);
    timer_interrupt_enable(TIMER3,TIMER_INT_CH3);

    /* TIMER2 counter enable */
    timer_enable(TIMER3);
}

static void timer3_init(void){
    gpio_configuration();
    nvic_configuration();
    timer3_config();
}

// service main loop
__NO_RETURN static void laser_status_thread(void *argument)
{
    (void)argument;
     timer3_init();
    for(;;) {
        if((ch2_fre == 10000) && (ch3_fre == 10000)){
            ch2_fre = 0;
            ch3_fre = 0;
            laser_flag = false;
        }else{
            if(!laser_flag){
                turn_laser(false);
                laser_flag = true;
            }
        }
   //     printf("ch2_fre %d,ch2_count %d\n",ch2_fre,ch2_count);
   //      printf("ch3_fre %d,ch3_count %d\n",ch3_fre,ch3_count);
        osDelay(200);
    }
}

// stack for the thread
#if !defined(STACK_SIZE_OF_SERVICE_LASER_STATUS_THREAD)
#define STACK_SIZE_OF_SERVICE_LASER_STATUS_THREAD 1024
#endif

__ALIGNED(8) static uint8_t stack_of_thread[STACK_SIZE_OF_SERVICE_LASER_STATUS_THREAD];

// attributes for the thread
static const osThreadAttr_t ThreadAttr_Laser_status = {
    .name = "Laser_status_Thread",
    .stack_mem  = stack_of_thread,
    .stack_size = sizeof(stack_of_thread),
};


static osThreadId_t tid_laser_status;

// start the laser service, should be invoked at the beginning.
// Return value:
//   true means success, otherwise failure
bool init_thread_of_service_laser_status(void)
{
    // just create a main loop thread.
    tid_laser_status = osThreadNew(laser_status_thread, NULL, &ThreadAttr_Laser_status);
    return tid_laser_status !=NULL;
}

/**
  * @brief  This function handles TIMER2 interrupt request.
  * @param  None
  * @retval None
  */
void TIMER3_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER3, TIMER_INT_FLAG_CH2)){
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_CH2);

        if(0 == ch2_ccnumber){
            /* read channel 0 capture value */
            ch2_readvalue1 = timer_channel_capture_value_register_read(TIMER3, TIMER_CH_2);
            ch2_ccnumber = 1;
        }else if(1 == ch2_ccnumber){
            /* read channel 0 capture value */
            ch2_readvalue2 = timer_channel_capture_value_register_read(TIMER3, TIMER_CH_2);

            if(ch2_readvalue2 > ch2_readvalue1){
                ch2_count = (ch2_readvalue2 - ch2_readvalue1);
            }else{
                ch2_count = ((0xFFFFU - ch2_readvalue1) + ch2_readvalue2);
            }

            ch2_fre = 1000000U / ch2_count;
            ch2_ccnumber = 0;
        }
    }

    if(SET == timer_interrupt_flag_get(TIMER3, TIMER_INT_FLAG_CH3)){
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_CH3);

        if(0 == ch3_ccnumber){
            /* read channel 0 capture value */
            ch3_readvalue1 = timer_channel_capture_value_register_read(TIMER3, TIMER_CH_3);
            ch3_ccnumber = 1;
        }else if(1 == ch3_ccnumber){
            /* read channel 0 capture value */
            ch3_readvalue2 = timer_channel_capture_value_register_read(TIMER3, TIMER_CH_3);

            if(ch3_readvalue2 > ch3_readvalue1){
                ch3_count = (ch3_readvalue2 - ch3_readvalue1);
            }else{
                ch3_count = ((0xFFFFU - ch3_readvalue1) + ch3_readvalue2);
            }

            ch3_fre = 1000000U / ch3_count;
            ch3_ccnumber = 0;
        }
    }
}
