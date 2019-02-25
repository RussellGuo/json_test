#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>

#include "ipc_cmd.h"

static void tts_notifiy_callback(struct ipc_task_t *ipc_task, const uint8_t msg[], uint16_t len)
{
    fprintf(stderr, "tts_notifiy_callback: '%s'\r\n", msg);
}

static struct ipc_task_t tts_task = {
    -1,                   // fd
    0,                    // sub process ID
    tts_notifiy_callback, // will be call
};


bool RemoteTtsinit(void)
{
    bool ret;
    void tts_cmd_loop(void);
    ret = start_subtask(&tts_task, tts_cmd_loop);
    return ret;
}

bool RemoteTtsclose(void)
{
    bool ret;
    ret = send_ipc_cmd(&tts_task, "Q", 1);
    stop_subtask(&tts_task);
    return ret;
}

bool RemoteTtsStopPlaying(void)
{
    bool ret = send_ipc_cmd(&tts_task, "T", 1);
    return ret;
}

bool RemoteTtsSetting(uint16_t pitch, uint16_t rate, uint16_t volume)
{
    char buf[80];
    int len = sprintf(buf, "S%u %u %u", pitch, rate, volume);
    bool ret = send_ipc_cmd(&tts_task, buf, len);
    return ret;
}

bool RemoteTtsPlay(bool isGBK, const char *txt)
{
    char buf[4096 + 1];
    int len = snprintf(buf, sizeof buf, "%s%s", isGBK ? "G" : "U", txt);
    bool ret = send_ipc_cmd(&tts_task, buf, len);
    return ret;
}

bool RemoteTtsBeep(uint16_t freq, uint16_t mesc)
{
    char buf[80];
    int len = sprintf(buf, "B%u %u", freq, mesc);
    bool ret = send_ipc_cmd(&tts_task, buf, len);
    return ret;
}

