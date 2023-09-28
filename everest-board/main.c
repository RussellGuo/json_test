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
#include "uart_io_API.h"
#include "datagram_codec.h"
#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5

/*----------------------------------------------------------------------------
 *      Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/

int main (void) {

    osKernelInitialize();                 // Initialize CMSIS-RTOS
    init_uart_io_api();

    // System Initialization

    // ...
    osThreadNew(serial_datagram_receive_loop, NULL, NULL);    // Create application main thread
    if (osKernelGetState() == osKernelReady) {
        osKernelStart();                    // Start thread execution
    }

    while(1);
}
