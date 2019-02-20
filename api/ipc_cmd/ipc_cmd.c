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

void enable_ipc_signal(bool enable)
{
    sigset_t sigset, old_sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIG_MAIN_TO_SUBTASK);
    sigprocmask(enable ? SIG_UNBLOCK : SIG_BLOCK, &sigset, &old_sigset);
}

bool start_subtask(subtask_proc_t proc, int *ipc_fd, pid_t *pid)
{
    int pipe_fds[2];

    enable_ipc_signal(false);
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
    *pid = fork();
    if (*pid < 0) {
        perror("start_subtask fork");
        fprintf(stderr, "\r\n");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return false;
    } else if(*pid > 0) {
        // main process
        // main should not use pipe reading
        close(pipe_fds[0]);
        *ipc_fd = pipe_fds[1];
        return true;

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

void stop_subtask(int ipc_fd, pid_t pid)
{
    fprintf(stderr, "main process ending the subprocess...\r\n");
    // wait sub-process
    if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    }
    // close pipe
    close(ipc_fd);
}

bool send_msg_to_remote(int ipc_fd, const void *msg, uint16_t msg_len, pid_t sub_pid)
{
    ssize_t ret = send(ipc_fd, msg, msg_len, 0);
    if (ret < 0) {
        perror("send_msg_to_remote");
        fprintf(stderr, "\r\n");
    }
    if (ret == msg_len) {
        if (sub_pid > 0) {
            kill(sub_pid, ipc_fd != 0 ? SIG_MAIN_TO_SUBTASK : SIG_SUB_TO_MAINTASK);
        }
        return true;
    }
    return false;
}

int recv_msg_from_remote(int ipc_fd, void *msg, uint16_t max_msg_len)
{
    ssize_t ret = recv(ipc_fd, msg, max_msg_len, 0);
    return (int)ret;
}

bool has_msg_from_remote(int ipc_fd)
{
    int ret;
    struct pollfd p;
    p.fd = ipc_fd;
    p.events = POLL_IN;
    p.revents = 0;
    ret = poll(&p, 1, 0);
    return ret > 0;
}
