#include <stdio.h>

#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5

#include "RTE_Components.h"
#include CMSIS_device_header

#include "sign_verify.h"

static bool sign_verify_demo(void);

void app_main (void *argument) {
    for (long long i = 0;; i++) {
        bool is_ok = sign_verify_demo();
        printf("%lld: signature veritification ret: %d\n\r", i, is_ok);      
        osDelay(300);
    }
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

static uint8_t pub_key_pem_string[] =
    "-----BEGIN PUBLIC KEY-----\r\n"
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxo1YWUCtl1RFCz9J9dq/\r\n"
    "mx7QFySWCQbnx8YmZfCdjKXLGiP5pJQ2++Ml1n6cUKasfgeJAF40tE68aI8hv1pW\r\n"
    "BTXFDW2BTnV0owdQQYGXqUBgOG/nBxNxlaqj6VynnXVq1IBZkVZ33VLFN2B8qbFf\r\n"
    "1gY/qzAprexOdagpZ9lMP5ksHbgS1EvohXFpEoBODBQnsZ9gFYsNbqEYMgMFFYfs\r\n"
    "Q6D/iKa9URqGfvNcMTfJmFv9sgOC4S6ZCsXyrJHZm+BRec6xy3zNY8JQtZ0sSDj8\r\n"
    "P5D2s0TYGSpPlBTe/PtLSTqzsozYePrMiPCxqFR1+G2pQPpTRv5Atq92e+HYjsJP\r\n"
    "JQIDAQAB\r\n"
    "-----END PUBLIC KEY-----\r\n"
;

static const uint8_t orig_data[] = { '1', '2', '3', '4' };
static const uint8_t sign_data[] = {
    0x0A, 0x8E, 0xD1, 0x81, 0x21, 0x78, 0x5F, 0xE2, 0xE6, 0xBD, 0xD9, 0x8C, 0x2D, 0xD0, 0x93, 0xE5,
    0x09, 0x03, 0x69, 0x1F, 0xBC, 0x04, 0xE7, 0x1B, 0x8C, 0x79, 0xDB, 0xFC, 0x21, 0xCC, 0x59, 0x8F,
    0x48, 0xD3, 0xB4, 0xCE, 0x19, 0xE7, 0xB7, 0x7A, 0x6B, 0x20, 0xF8, 0x9D, 0xFD, 0x37, 0x84, 0x60,
    0xA1, 0xF0, 0x69, 0x0C, 0xAE, 0x32, 0x5D, 0x5F, 0x7B, 0xCD, 0xB2, 0xC0, 0xF9, 0x33, 0x60, 0x8F,
    0x03, 0x5C, 0xFC, 0x91, 0xB5, 0x2E, 0xB4, 0x73, 0x65, 0x6D, 0x40, 0xE4, 0xA6, 0x75, 0x79, 0x1A,
    0x01, 0x98, 0x40, 0xEC, 0x97, 0xB9, 0xEC, 0x0C, 0x89, 0xAF, 0x7B, 0xFD, 0x49, 0xD1, 0xBB, 0xCB,
    0x6F, 0xA0, 0x1F, 0x7A, 0x37, 0x78, 0xB6, 0x0B, 0x0A, 0xD5, 0x07, 0xA1, 0x51, 0xA3, 0x6E, 0xF1,
    0xE5, 0x11, 0x64, 0x61, 0x8B, 0x38, 0xCA, 0x52, 0x8D, 0x0A, 0x38, 0xD6, 0xFB, 0x24, 0xD6, 0x5B,
    0x58, 0xF5, 0x00, 0xC0, 0x94, 0x04, 0x78, 0xAC, 0x97, 0x08, 0x5D, 0xFE, 0xBD, 0xD1, 0xC5, 0x4E,
    0x42, 0xCD, 0xE3, 0x0F, 0x62, 0xC6, 0xD5, 0xFE, 0x32, 0x7D, 0xD4, 0x72, 0xDE, 0xFD, 0x43, 0x63,
    0x4D, 0x24, 0xF4, 0x00, 0x44, 0x63, 0x61, 0x08, 0x8B, 0xA6, 0x23, 0x9C, 0x13, 0x81, 0x2D, 0x63,
    0x96, 0x1C, 0x6E, 0x6A, 0x76, 0xBE, 0x4B, 0xBB, 0xDA, 0x11, 0x5F, 0x4E, 0x85, 0x14, 0x12, 0xF4,
    0x40, 0x18, 0x9D, 0x59, 0xAA, 0x4E, 0x1A, 0x8B, 0xE7, 0x4A, 0x85, 0x77, 0x84, 0x5A, 0x9C, 0x30,
    0x07, 0xAC, 0x4E, 0x2C, 0xC0, 0x04, 0xE2, 0x8C, 0x24, 0xF7, 0x77, 0x5B, 0x5D, 0x35, 0x36, 0x7A,
    0x63, 0x40, 0x2B, 0x90, 0xBE, 0x3C, 0xEE, 0x89, 0xE3, 0x0C, 0x88, 0x92, 0xAB, 0x2D, 0x4F, 0x96,
    0x14, 0x5F, 0x9D, 0x03, 0xB7, 0xD1, 0x04, 0xD7, 0x37, 0xF5, 0x9C, 0xAB, 0xAC, 0x0A, 0xC4, 0x2D,
};

static bool sign_verify_demo(void)
{
    bool ret = sign_verify_sha512_rsa2048_pkcs1_padding(pub_key_pem_string, orig_data, sizeof orig_data, sign_data);
    return ret;
}

