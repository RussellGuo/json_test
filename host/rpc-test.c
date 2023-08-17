#include <stdio.h>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

//#include "semantic_api.h"
//#include "mcu-led-mode.h"
//#include "mcu-laser-mode.h"
//#include "mcu-flashlight-mode.h"
//#include "mcu-camera-mode.h"

#include "uart_io_api.h"

int main(void)
{
    //set_rpc_log_level(LOG_VERBOSE);

    bool ret;
    ret = init_uart_io_api();
    printf("ret %d for init_uart_io_api\n", ret);

    pid_t pid_of_recv = fork();
    if (pid_of_recv == 0) {
        serial_datagram_receive_loop(0);
        printf("UART is no longer OK, HALT\n");
        exit(1);
    }

    serial_datagram_send(123,0x08,NULL,0);

    sleep(5);
    kill(pid_of_recv, SIGKILL);

#ifndef WEXITED
#define WEXITED 4 /* Report dead child.  */
#endif
    waitpid(pid_of_recv, NULL, WEXITED);

    return 0;
}
