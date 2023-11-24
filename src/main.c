#include <stdio.h>

#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5

#include "RTE_Components.h"
#include CMSIS_device_header

#include "sign_verify.h"

void app_main (void *argument) {
    for (long long i = 0;; i++) {
        sign_verify_demo();
        printf("%lld\n\r", i);      
        osDelay(300);
    }
}

#define OWN_MEM_BLOCK_SIZE 8192
static unsigned char mem_block[OWN_MEM_BLOCK_SIZE];
static size_t mem_top = 0;
void *malloc(size_t n)
{
    size_t new_top = mem_top + ((n - 1) / 8 + 1)* 8;
    if (new_top > OWN_MEM_BLOCK_SIZE) {
        return 0;
    }
    void *result = mem_block + mem_top;
    mem_top = new_top;
    return result;
}

void free(void *p)
{
}

#define STACK_SIZE_OF_APP_THREAD 4096
__ALIGNED(8) static uint8_t stack_of_thread[STACK_SIZE_OF_APP_THREAD];

// task thread attribute
static const osThreadAttr_t thread_attr_app = {
    .name = "app_main",
    .priority = osPriorityRealtime4,
    .stack_mem  = stack_of_thread,
    .stack_size = sizeof(stack_of_thread),
};
int main (void) {

  // System Initialization
  SystemCoreClockUpdate();
  // ...
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadId_t id = osThreadNew(app_main, NULL, &thread_attr_app);
  if (osKernelGetState() == osKernelReady) {
    osKernelStart();                    // Start thread execution
  }

  while(1);
}
