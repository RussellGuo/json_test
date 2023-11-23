#include <stdio.h>

#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5

#include "RTE_Components.h"
#include CMSIS_device_header

void app_main (void *argument) {
    for (long long i = 0;; i++) {
        printf("%lld\n\r", i);      
        osDelay(300);
    }
}

int main (void) {

  // System Initialization
  SystemCoreClockUpdate();
  // ...
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, NULL);    // Create application main thread
  if (osKernelGetState() == osKernelReady) {
    osKernelStart();                    // Start thread execution
  }

  while(1);
}
