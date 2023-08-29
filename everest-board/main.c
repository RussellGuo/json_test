/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2019 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *      Name:    Blinky.c
 *      Purpose: RTX example program
 *
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include "mhscpu.h"
#include "uart_io_API.h"
#include "datagram_codec.h"
#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5

osThreadId_t tid_phaseA;                /* Thread id of thread: phase_a      */

/*----------------------------------------------------------------------------
 *      Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {

  tid_phaseA = osThreadNew(serial_datagram_receive_loop, NULL, NULL);

  osThreadFlagsSet(tid_phaseA, 0x0001);     /* set signal to phaseA thread   */

  osDelay(osWaitForever);
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = UART0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = UART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

int main (void) {
    SYSCTRL_ClocksTypeDef clk;

    SYSCTRL_PLLConfig(SYSCTRL_PLL_192MHz);
    SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div2);
    SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div_None);
    SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);

    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART3 | SYSCTRL_APBPeriph_GPIO, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART3, ENABLE);

    osKernelInitialize();                 // Initialize CMSIS-RTOS
    init_uart_io_api();
    NVIC_Configuration();

    // System Initialization
    SYSCTRL_GetClocksFreq(&clk);
    SystemCoreClock = clk.CPU_Frequency;
    printf("SystemCoreClock:%d\n", SystemCoreClock);

    // ...
    osThreadNew(app_main, NULL, NULL);    // Create application main thread
    if (osKernelGetState() == osKernelReady) {
        osKernelStart();                    // Start thread execution
    }

    while(1);
}
