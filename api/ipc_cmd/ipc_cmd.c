#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <poll.h>

#include <unistd.h>
#include <sys/wait.h>
#include "ipc_cmd.h"

#include <assert.h>

static void ipc_generic_callback(int id, uint64_t arg)
{
    uint8_t buf[4096 + 1];
    memset(buf, 0, sizeof (buf));
    ssize_t ret = recv(id, buf, sizeof (buf), 0);
    if (ret < 0) {
        perror("ipc_generic_callback");
        fprintf(stderr, "\r\n");
    } else {
        if (arg) {
            struct ipc_task_t *ipc_task = (struct ipc_task_t *)arg;
            subtask_notifiy_callback_t callback = ipc_task->subtask_notifiy_callback;
            if (callback) {
                callback(ipc_task, buf, ret);
            }
        }
    }
}

bool start_subtask(struct ipc_task_t *ipc_task, subtask_proc_t proc)
{
    int pipe_fds[2];

    ipc_task->ipc_fd = -1;
    ipc_task->pid = 0;
    // pipe
    if (socketpair(AF_UNIX, SOCK_DGRAM, 0, pipe_fds) < 0) {
        perror("start_subtask socketpair");
        fprintf(stderr, "\r\n");
        return false;
    }
    if (pipe_fds[1] <= 0) {
        fprintf(stderr, "OOPS! pipe write fd <= 0!!!\r\n");
        _exit(1);
    }

    // fork
    ipc_task->pid = fork();
    if (ipc_task->pid < 0) {
        perror("start_subtask fork");
        fprintf(stderr, "\r\n");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return false;
    } else if(ipc_task->pid > 0) {
        // main process
        // main should not use pipe reading
        close(pipe_fds[0]);
        ipc_task->ipc_fd = pipe_fds[1];
        bool ret = setPollEventFd(ipc_task->ipc_fd, ipc_generic_callback, (uint64_t)ipc_task, true);
        return ret;

    } else {
        // sub process
        // sub process should not use pipe writting
        close(pipe_fds[1]);

        // sub process always use 0 for IPC
        dup2(pipe_fds[0], 0);
        close(pipe_fds[0]);
        proc();
        close(0);
        _exit(0);
    }
}

void stop_subtask(struct ipc_task_t *ipc_task)
{
    fprintf(stderr, "main process ending the subprocess...\r\n");
    // wait sub-process
    if (ipc_task->pid > 0) {
        int status;
        waitpid(ipc_task->pid, &status, 0);
    }
    // close pipe
    close(ipc_task->ipc_fd);
    ipc_task->ipc_fd = -1;
    ipc_task->pid = 0;
}

bool send_ipc_cmd(struct ipc_task_t *ipc_task, const void *msg, uint16_t msg_len)
{
    ssize_t ret = send(ipc_task->ipc_fd, msg, msg_len, 0);
    if (ret < 0) {
        perror("send_ipc_cmd");
        fprintf(stderr, "\r\n");
    }
    if (ret == msg_len) {
        return true;
    }
    return false;
}

int recv_ipc_cmd(void *msg, uint16_t max_msg_len)
{
    ssize_t ret = recv(0, msg, max_msg_len, 0);
    return (int)ret;
}

bool has_ipc_cmd_from_caller(int msec)
{
    int ret;
    struct pollfd p;
    p.fd = 0;
    p.events = POLL_IN;
    p.revents = 0;
    ret = poll(&p, 1, msec);
    return ret > 0;
}

bool send_ipc_reply(const void *msg, uint16_t msg_len)
{
    ssize_t ret = send(0, msg, msg_len, 0);
    return ret == msg_len;
}
