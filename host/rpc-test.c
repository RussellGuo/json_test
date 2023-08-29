#include <stdio.h>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include "uart_io_api.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "remote_message.pb.h"
#include "datagram_codec.h"

int main(void)
{
    //set_rpc_log_level(LOG_VERBOSE);
    //¹¹ÔìÒ»¸öto mcu±àÂëµÄÀý×Ó
    bool ret;
    login_req login_req_obj = {"caomeng", "123456"};
    //login_req_obj.username = "caomeng";
    //login_req_obj.password = "123456";

    to_mcu to_mcu_obj = to_mcu_init_zero;
    to_mcu_obj.seq = 1;
    to_mcu_obj.which_req = to_mcu_login_tag;
    to_mcu_obj.req.login = login_req_obj;

    unsigned char login_req_buf[600] = {0};

    pb_ostream_t out_stream = pb_ostream_from_buffer(login_req_buf, sizeof(login_req_buf));

    bool status = pb_encode(&out_stream, to_mcu_fields, &to_mcu_obj);

    ret = init_uart_io_api();
    printf("ret %d for init_uart_io_api\n", ret);

    pid_t pid_of_recv = fork();
    if (pid_of_recv == 0) {
        serial_datagram_receive_loop(0);
        printf("UART is no longer OK, HALT\n");
        exit(1);
    }

    send_datagram(login_req_buf, out_stream.bytes_written);

    sleep(5);
    kill(pid_of_recv, SIGKILL);

#ifndef WEXITED
#define WEXITED 4 /* Report dead child.  */
#endif
    waitpid(pid_of_recv, NULL, WEXITED);

    return 0;
}
