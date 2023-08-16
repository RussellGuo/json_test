#include <stdio.h>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

#include "semantic_api.h"
#include "mcu-led-mode.h"
#include "mcu-laser-mode.h"
#include "mcu-flashlight-mode.h"
#include "mcu-camera-mode.h"

#include "uart_io_api.h"

int main(void)
{
    set_rpc_log_level(LOG_VERBOSE);

    bool ret;
    ret = init_uart_io_api();
    printf("ret %d for init_uart_io_api\n", ret);

    pid_t pid_of_recv = fork();
    if (pid_of_recv == 0) {
        serial_datagram_receive_loop(0);
        printf("UART is no longer OK, HALT\n");
        exit(1);
    }

    struct timeval timeval = {0};
    gettimeofday(&timeval, NULL);
    uint32_t seq = (uint32_t)( (timeval.tv_sec % 3600)* 1000 + timeval.tv_usec / 1000);

    ret = GetPsnFromEeprom(++seq);
    rpc_log(LOG_DEBUG, "ret %d for GetPsnFromEeprom %u", ret, seq);

    ret = ReqFwVersion(++seq);
    rpc_log(LOG_DEBUG, "ret %d for ReqFwVersion %u", ret, seq);

    ret = ReqRunInfo(++seq);
    rpc_log(LOG_DEBUG, "ret %d for ReqRunInfo %u", ret, seq);

    for (unsigned int i = 0; i < 6; i++) {
        unsigned mode_parameter = 1 << i;
        ret = SetLedConfig(LED_MODE_FACTORY_TEST, mode_parameter, ++seq);
        rpc_log(LOG_DEBUG, "ret %d for SetLedConfig mode %u %u", ret, mode_parameter, seq);
        sleep(1);
    }
    ret = SetLedConfig(MCU_MODULE_OFF, 0, ++seq);
    rpc_log(LOG_DEBUG, "ret %d for SetLedConfig OFF %u", ret, seq);

    ret = SetLaserConfig(LASER_MODE_FACTORY_TEST, 0, ++seq);
    rpc_log(LOG_DEBUG, "ret %d for SetLaserConfig %u", ret, seq);
    sleep(1);
    ret = SetLaserConfig(MCU_MODULE_OFF, 0, ++seq);
    rpc_log(LOG_DEBUG, "ret %d for SetLaserConfig OFF %u", ret, seq);

    ret = SetFlashlightConfig(FLASHLIGHT_MODE_FACTORY_TEST, 0, ++seq);
    rpc_log(LOG_DEBUG, "ret %d for SetFlashlightConfig %u", ret, seq);
    sleep(1);

    ret = SetFlashlightConfig(MCU_MODULE_OFF, 0, ++seq);
    rpc_log(LOG_DEBUG, "ret %d for SetFlashlightConfig OFF %u", ret, seq);

    ret = ConnectivityTest(++seq);
    rpc_log(LOG_DEBUG, "ret %d for StartFactoryTest %u", ret, seq);

    ret = SavePsnIntoEeprom("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrst", ++seq);
    rpc_log(LOG_DEBUG, "ret %d for SavePsnIntoEeprom %u", ret, seq);

    ret = SetCameraConfig(CAMERA_MODE_SECONDARY, ++seq);
    rpc_log(LOG_DEBUG, "ret %d for SetCameraConfig %u", ret, seq);


    sleep(5);
    kill(pid_of_recv, SIGKILL);

#ifndef WEXITED
#define WEXITED 4 /* Report dead child.  */
#endif
    waitpid(pid_of_recv, NULL, WEXITED);

    return 0;
}
