;/**************************************************************************//**
; * @file     startup_mhscpu.s
; * @brief    CMSIS Cortex-M ARMv7-M based Core Device Startup File for
; *           Device mhscpu
; * @version  V5.4.0
; * @date     12. December 2018
; ******************************************************************************/
;/*
; * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */

;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------


;<h> Stack Configuration
;  <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;</h>

Stack_Size      EQU      0x00008000

                AREA     STACK, NOINIT, READWRITE, ALIGN=3
__stack_limit
Stack_Mem       SPACE    Stack_Size
__initial_sp


;<h> Heap Configuration
;  <o> Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
;</h>

Heap_Size       EQU      0x00008000

                IF       Heap_Size != 0                      ; Heap is provided
                AREA     HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE    Heap_Size
__heap_limit
                ENDIF


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA     RESET, DATA, READONLY
                EXPORT   __Vectors
                EXPORT   __Vectors_End
                EXPORT   __Vectors_Size

__Vectors       DCD      __initial_sp                        ;     Top of Stack
                DCD      Reset_Handler                       ;     Reset Handler
                DCD      NMI_Handler                         ; -14 NMI Handler
                DCD      HardFault_Handler                   ; -13 Hard Fault Handler
                DCD      MemManage_Handler                   ; -12 MPU Fault Handler
                DCD      BusFault_Handler                    ; -11 Bus Fault Handler
                DCD      UsageFault_Handler                  ; -10 Usage Fault Handler
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      0                                   ;     Reserved
                DCD      SVC_Handler                         ;  -5 SVCall Handler
                DCD      DebugMon_Handler                    ;  -4 Debug Monitor Handler
                DCD      0                                   ;     Reserved
                DCD      PendSV_Handler                      ;  -2 PendSV Handler
                DCD      SysTick_Handler                     ;  -1 SysTick Handler

                ; Interrupts
                DCD      DMA0_IRQHandler
                DCD      USB_IRQHandler
                DCD      CHARGE_IRQHandler
                DCD      LCD_IRQHandler
                DCD      SCI0_IRQHandler
                DCD      UART0_IRQHandler
                DCD      UART1_IRQHandler
                DCD      SPI0_IRQHandler
                DCD      CRYPT0_IRQHandler
                DCD      TIM0_0_IRQHandler
                DCD      TIM0_1_IRQHandler
                DCD      TIM0_2_IRQHandler
                DCD      TIM0_3_IRQHandler
                DCD      EXTI0_IRQHandler
                DCD      EXTI1_IRQHandler
                DCD      EXTI2_IRQHandler
                DCD      RTC_IRQHandler
                DCD      SENSOR_IRQHandler
                DCD      TRNG_IRQHandler
                DCD      ADC0_IRQHandler
                DCD      SSC_IRQHandler
                DCD      TIM0_4_IRQHandler
                DCD      TIM0_5_IRQHandler
                DCD      KEYBOARD_IRQHandler
                DCD      MSR_IRQHandler
                DCD      EXTI3_IRQHandler
                DCD      HSPI1_IRQHandler
                DCD      SPI2_IRQHandler
                DCD      0
                DCD      SCI2_IRQHandler
                DCD      0
                DCD      0
                DCD      UART2_IRQHandler
                DCD      UART3_IRQHandler
                DCD      0
                DCD      QSPI_IRQHandler
                DCD      I2C0_IRQHandler
                DCD      EXTI4_IRQHandler
                DCD      EXTI5_IRQHandler
                DCD      TIM0_6_IRQHandler
                DCD      TIM0_7_IRQHandler
                DCD      0
                DCD      DCMI_IRQHandler
                DCD      0
                DCD      0
                DCD      0
                DCD      QR_IRQHandler
                DCD      GPU_IRQHandler
                DCD      0
                DCD      AWD_IRQHandler
                DCD      DAC_IRQHandler
                DCD      HSPI0_IRQHandler
                DCD      ASYMC_IRQHandler
                DCD      CMSPI_IRQHandler
                DCD      CMSPIDMA_IRQHandler
                SPACE    ((224 - 55) * 4)                    ; Interrupts 55 .. 224 are left out
__Vectors_End
__Vectors_Size  EQU      __Vectors_End - __Vectors


                AREA     |.text|, CODE, READONLY

; Reset Handler

Reset_Handler   PROC
                EXPORT   Reset_Handler             [WEAK]
                IMPORT   SystemInit
                IMPORT   __main

                LDR      R0, =__Vectors
                LDR      R1, =0xE000ED08
                STR      R0, [R1]
                LDR      R0, [R0]
                MOV      SP, R0

                LDR      R0, =SystemInit
                BLX      R0
                LDR      R0, =__main
                BX       R0
                ENDP


; Macro to define default exception/interrupt handlers.
; Default handler are weak symbols with an endless loop.
; They can be overwritten by real handlers.
                MACRO
                Set_Default_Handler  $Handler_Name
$Handler_Name   PROC
                EXPORT   $Handler_Name             [WEAK]
                B        .
                ENDP
                MEND


; Default exception/interrupt handler

                Set_Default_Handler  NMI_Handler
                Set_Default_Handler  HardFault_Handler
                Set_Default_Handler  MemManage_Handler
                Set_Default_Handler  BusFault_Handler
                Set_Default_Handler  UsageFault_Handler
                Set_Default_Handler  SVC_Handler
                Set_Default_Handler  DebugMon_Handler
                Set_Default_Handler  PendSV_Handler
                Set_Default_Handler  SysTick_Handler

                Set_Default_Handler  DMA0_IRQHandler
                Set_Default_Handler  USB_IRQHandler
                Set_Default_Handler  CHARGE_IRQHandler
                Set_Default_Handler  LCD_IRQHandler
                Set_Default_Handler  SCI0_IRQHandler
                Set_Default_Handler  UART0_IRQHandler
                Set_Default_Handler  UART1_IRQHandler
                Set_Default_Handler  SPI0_IRQHandler
                Set_Default_Handler  CRYPT0_IRQHandler
                Set_Default_Handler  TIM0_0_IRQHandler
                Set_Default_Handler  TIM0_1_IRQHandler
                Set_Default_Handler  TIM0_2_IRQHandler
                Set_Default_Handler  TIM0_3_IRQHandler
                Set_Default_Handler  EXTI0_IRQHandler
                Set_Default_Handler  EXTI1_IRQHandler
                Set_Default_Handler  EXTI2_IRQHandler
                Set_Default_Handler  RTC_IRQHandler
                Set_Default_Handler  SENSOR_IRQHandler
                Set_Default_Handler  TRNG_IRQHandler
                Set_Default_Handler  ADC0_IRQHandler
                Set_Default_Handler  SSC_IRQHandler
                Set_Default_Handler  TIM0_4_IRQHandler
                Set_Default_Handler  TIM0_5_IRQHandler
                Set_Default_Handler  KEYBOARD_IRQHandler
                Set_Default_Handler  MSR_IRQHandler
                Set_Default_Handler  EXTI3_IRQHandler
                Set_Default_Handler  HSPI1_IRQHandler
                Set_Default_Handler  SPI2_IRQHandler
                Set_Default_Handler  SCI2_IRQHandler
                Set_Default_Handler  UART2_IRQHandler
                Set_Default_Handler  UART3_IRQHandler
                Set_Default_Handler  QSPI_IRQHandler
                Set_Default_Handler  I2C0_IRQHandler
                Set_Default_Handler  EXTI4_IRQHandler
                Set_Default_Handler  EXTI5_IRQHandler
                Set_Default_Handler  TIM0_6_IRQHandler
                Set_Default_Handler  TIM0_7_IRQHandler
                Set_Default_Handler  DCMI_IRQHandler
                Set_Default_Handler  QR_IRQHandler
                Set_Default_Handler  GPU_IRQHandler
                Set_Default_Handler  AWD_IRQHandler
                Set_Default_Handler  DAC_IRQHandler
                Set_Default_Handler  HSPI0_IRQHandler
                Set_Default_Handler  ASYMC_IRQHandler
                Set_Default_Handler  CMSPI_IRQHandler
                Set_Default_Handler  CMSPIDMA_IRQHandler

                ALIGN


; User setup Stack & Heap

                IF       :LNOT::DEF:__MICROLIB
                IMPORT   __use_two_region_memory
                ENDIF

                EXPORT   __stack_limit
                EXPORT   __initial_sp
                IF       Heap_Size != 0                      ; Heap is provided
                EXPORT   __heap_base
                EXPORT   __heap_limit
                ENDIF

                END
