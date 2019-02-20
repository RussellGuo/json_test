#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>

#include "ipc_cmd.h"

static int mainfd = -1;
static pid_t subtask_pid = -1;

static void main_signal_handler(int sig)
{
    write(2, "main task be notified\r\n", 23);
}


bool RemoteTtsinit(void)
{
    bool ret;
    void tts_cmd_loop(void);
    signal(SIG_SUB_TO_MAINTASK, main_signal_handler);
    ret = start_subtask(tts_cmd_loop, &mainfd, &subtask_pid);
    return ret;
}

bool RemoteTtsclose(void)
{
    bool ret;
    ret = send_msg_to_remote(mainfd, "Q", 1, subtask_pid);
    stop_subtask(mainfd, subtask_pid);
    mainfd = subtask_pid = -1;
    return ret;
}

bool RemoteTtsStopPlaying(void)
{
    bool ret = send_msg_to_remote(mainfd, "", 0, subtask_pid);
    return ret;
}

bool RemoteTtsSetting(uint16_t pitch, uint16_t rate, uint16_t volume)
{
    char buf[80];
    int len = sprintf(buf, "S%u %u %u", pitch, rate, volume);
    bool ret = send_msg_to_remote(mainfd, buf, len, subtask_pid);
    return ret;
}

bool RemoteTtsPlay(bool isGBK, const char *txt)
{
    char buf[4096 + 1];
    int len = snprintf(buf, sizeof buf, "%s%s", isGBK ? "G" : "U", txt);
    bool ret = send_msg_to_remote(mainfd, buf, len, subtask_pid);
    return ret;
}

bool RemoteTtsBeep(uint16_t freq, uint16_t mesc)
{
    char buf[80];
    int len = sprintf(buf, "B%u %u", freq, mesc);
    bool ret = send_msg_to_remote(mainfd, buf, len, subtask_pid);
    return ret;
}

int getRemoteTtsIpcFd(void)
{
    return mainfd;
}

int RemoteTtsMsgFromService(void *msg, uint16_t max_msg_len)
{
    if (has_msg_from_remote(mainfd)) {
        int len = recv_msg_from_remote(mainfd, msg, max_msg_len);
        return len;
    } else {
        return 0;
    }
}

