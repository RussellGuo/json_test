#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>

#include <unistd.h>

#include "ipc_cmd.h"
#include "tts_api.h"

tts_callback_t tts_callback;
static void tts_notifiy_callback(struct ipc_task_t *ipc_task, const uint8_t msg[], uint16_t len)
{
    tts_playing_result_t result = tts_playing_other;
    const char *info = (const char *)msg;
    if (strcmp(info, "ERR OK") == 0) {
        result = tts_playing_normal_finished;
    } else if (strcmp(info, "ERR INIT") == 0) {
        result = tts_playing_initiation_failed;
    } else if (strcmp(info, "ERR USERCANCELLED") == 0) {
        result = tts_playing_user_cancelled;
    } else if (strcmp(info, "ERR DEVICE") == 0) {
        result = tts_playing_device_failed;
    } else {
        result = tts_playing_other;
    }

    if (tts_callback) {
        tts_callback(result, info);
    }
}

static struct ipc_task_t tts_task = {
    -1,                   // fd
    0,                    // sub process ID
    tts_notifiy_callback, // will be call
};


static void start_tts_service(void)
{
    int ret = execl("/system/bin/tts_service", "/system/bin/tts_service", NULL);
    perror("start tts service");
    fprintf(stderr, "\r\n");
    _exit(1);
}

bool RemoteTtsinit(tts_callback_t callback)
{
    bool ret;
    ret = start_subtask(&tts_task, start_tts_service);
    if (ret) {
        tts_callback = callback;
    }
    return ret;
}

bool RemoteTtsclose(void)
{
    bool ret;
    ret = send_ipc_cmd(&tts_task, "Q", 1);
    stop_subtask(&tts_task);
    tts_callback = NULL;
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

