#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdint.h>

#include <ctype.h>
#include <unistd.h>
#include <time.h>

#if TTS_VERSION==300
#include "yt_tts_interface_300.h"

#define yt_tts_set_memory_buffer   yt_tts_set_memory_buffer_300
#define yt_tts_input_text_mbcs     yt_tts_input_text_mbcs_300
#define yt_tts_input_text_utf16    yt_tts_input_text_utf16_300
#define YT_DATE_YYYY_MM_DD         YT_DATE_YYYY_MM_DD_300
#define YT_TEXT_TYPE_DEFAULT       YT_TEXT_TYPE_DEFAULT_300
#define yt_tts_get_speech_frame    yt_tts_get_speech_frame_300
#define yt_tts_free_resource       yt_tts_free_resource_300
#define yt_tts_set_pitch           yt_tts_set_pitch_300
#define yt_tts_set_rate            yt_tts_set_rate_300
#define yt_tts_set_volume          yt_tts_set_volume_300
#define yt_tts_utf8_to_utf16       yt_tts_utf8_to_utf16_300
#elif TTS_VERSION==150
#include "yt_tts_interface_150.h"

#define yt_tts_set_memory_buffer   yt_tts_set_memory_buffer_150
#define yt_tts_input_text_mbcs     yt_tts_input_text_mbcs_150
#define yt_tts_input_text_utf16    yt_tts_input_text_utf16_150
#define YT_DATE_YYYY_MM_DD         YT_DATE_YYYY_MM_DD_150
#define YT_TEXT_TYPE_DEFAULT       YT_TEXT_TYPE_DEFAULT_150
#define yt_tts_get_speech_frame    yt_tts_get_speech_frame_150
#define yt_tts_free_resource       yt_tts_free_resource_150
#define yt_tts_set_pitch           yt_tts_set_pitch_150
#define yt_tts_set_rate            yt_tts_set_rate_150
#define yt_tts_set_volume          yt_tts_set_volume_150
#define yt_tts_utf8_to_utf16       yt_tts_utf8_to_utf16_150
#else
#error TTS_VERSION must be 300 or 150
#endif
#include "pcm_play.h"

#include "ipc_cmd.h"

static bool tts_init(void)
{
    char *pMemoryBufferForTTS = malloc(YT_TTS_MEM_SIZE_IN_BYTE);
    //STEP 1: initialize TTS engine
    bool ret = NULL != pMemoryBufferForTTS;
    if (ret) {
        #if TTS_VERSION==300
        char strArgOne[100],strArgTwo[100], strReserved[100] = "";

        yt_tts_set_memory_buffer(pMemoryBufferForTTS,YT_TTS_MEM_SIZE_IN_BYTE);

        strcpy(strArgOne,"/system/vendor/huaqin/yt_tts_data/mandarin/yt_tts_man_data_one.dat");
        strcpy(strArgTwo,"/system/vendor/huaqin/yt_tts_data/mandarin/yt_tts_man_data_two_01.dat");
        ret = yt_tts_initialize_300(strReserved,YT_LANG_ID_MANDARIN_300,strArgOne,YT_VOICE_ID_FEMALE_300,strArgTwo) == 0;
        #endif
        #if TTS_VERSION==150
        ret = yt_tts_initialize_150(pMemoryBufferForTTS, YT_TTS_MEM_SIZE_IN_BYTE) == 0;
        #endif
    } else {
        fprintf(stderr, "Not enough memory\r\n");
    }

    pcm_prepair();
    return ret;

}

#define SAMPLE_RATE 16000

#define BUZZER_MIN_FRQ 100
#define BUZZER_MAX_FRQ (SAMPLE_RATE / 2)
#define BUZZER_MIN_MSEC 70
#define BUZZER_MAX_MSEC 600

#define MAX_SAMPLE_COUNT (BUZZER_MAX_MSEC * SAMPLE_RATE / 1000)
static void buzzer_play(uint16_t freq, uint16_t msec, uint16_t volume)
{
    bool feed_ok = true;
    bool cancelled = false;
    if (freq < BUZZER_MIN_FRQ) {
        freq = BUZZER_MIN_FRQ;
    } else if (freq > BUZZER_MAX_FRQ) {
        freq = BUZZER_MAX_FRQ;
    }
    if (msec < BUZZER_MIN_MSEC) {
        msec = BUZZER_MIN_MSEC;
    } else if (msec > BUZZER_MAX_MSEC) {
        msec = BUZZER_MAX_MSEC;
    }
    int16_t sample[MAX_SAMPLE_COUNT];
    size_t sample_count = msec * SAMPLE_RATE / 1000;
    volume = volume * 7 / 10;
    for (unsigned n = 0; n < sample_count; n++) {
        sample[n] = (2 * n * freq / SAMPLE_RATE) % 2 == 0 ? -volume : +volume;
        if (n == 0 || sample[n] != sample[n - 1]) {
            // fprintf(stderr, "sample[%u] = %d\r\n", n, sample[n]);
        }
    }
    pcm_begin(msec);
    feed_ok = pcm_feed(sample, sample_count * 2);
    if (cancelled) {
        pcm_abort();
    }
    feed_ok = (!!feed_ok) & (!!pcm_end());
    const char *reply_msg;
    if (!feed_ok) {
        reply_msg = "ERR DEVICE";
    } else {
        reply_msg = cancelled ? "ERR USERCANCELLED" : "ERR OK";
    }
    send_ipc_reply(reply_msg, 0);
}

static bool tts_play(bool isGBK, char *buf)
{
    static int playing_count = 0;
    int len = strlen(buf);
    int nReturn;
    bool feed_ok = true;

    if (len > 4096 - 1) {
        len = 4096 - 1;
        buf[len] = 0;
    }
    if (isGBK) {
        nReturn = yt_tts_input_text_mbcs(buf, len,YT_DATE_YYYY_MM_DD,YT_TEXT_TYPE_DEFAULT);
    } else {
        unsigned short utf16Text[4096];
        unsigned int nTextLen;
        yt_tts_utf8_to_utf16(buf, len, utf16Text, &nTextLen);
        nReturn = yt_tts_input_text_utf16(utf16Text, nTextLen, YT_DATE_YYYY_MM_DD, YT_TEXT_TYPE_DEFAULT);
    }
    if (nReturn != 0) {
        fprintf(stderr, "start tts playing error\r\n");
        send_ipc_reply("ERR INIT", 0);
        return false;
    }
    pcm_begin(0);

    while(1) {
        short pSpeechFrame[400];
        unsigned int nSampleNumber;

        if (has_ipc_cmd_from_caller(0)) {
            // new command comes, abort this playing.
            break;
        }
        nReturn = yt_tts_get_speech_frame(pSpeechFrame,&nSampleNumber);
        if(nSampleNumber > 0) {
            feed_ok = pcm_feed(pSpeechFrame, nSampleNumber * 2);
            if (!feed_ok) {
                break;
            }
        }

        if(1 == nReturn) {
            // TODO: sentence complete.
            nSampleNumber = 62;
            memset(pSpeechFrame,0,nSampleNumber * 2);
             for(int j = 50; j <50; j++) {
                 feed_ok = pcm_feed(pSpeechFrame, nSampleNumber * 2);
                 if (!feed_ok) {
                     break;
                 }
             }
             if (!feed_ok) {
                 break;
             }
        }

        if(0 == nReturn) break; //complete the entire input text

    }

    if (nReturn) {
        pcm_abort();
    }
    pcm_end();

    const char *reply_msg;
    if (!feed_ok) {
        reply_msg = "ERR DEVICE";
    } else {
        reply_msg = nReturn == 0 ? "ERR OK" : "ERR USERCANCELLED";
    }
    send_ipc_reply(reply_msg, 0);

    playing_count++;
    if (playing_count >= 3) {
        int ret = execl("/system/bin/tts_service", "tts_service", NULL);
        perror("start tts service");
        fprintf(stderr, "\r\n");
        _exit(1);
    }
    return true;

}

static void tty_setting(uint16_t pitch, uint16_t rate, uint16_t volume)
{
    yt_tts_set_rate(rate);
    yt_tts_set_pitch(pitch);
    yt_tts_set_volume(volume);
}

// YT TTS part
static void tts_cmd_loop(void)
{

    uint16_t pitch = 105, rate = 105, volume = 32767;
    pid_t parent_pid = getppid();

    bool ret = tts_init();
    fprintf(stderr, "return value of tts init: %d\r\n", ret);
    if (!ret) {
        // return; // should NOT quit
    }
    tty_setting(pitch, rate, volume);

    for (;;) {
            char buf[4096];
            char cmd;
            memset(buf, 0, sizeof buf);
            int len = recv_ipc_cmd(buf, sizeof buf);
            if (len < 0) {
                perror("recv_msg_from maintask");
                fprintf(stderr, "\r\n");
                continue;
            }
            cmd = toupper(buf[0]);
            switch (cmd) {
            case 'Q':
                fprintf(stderr, "Quit Cmd received\r\n");
                return;

            case 'B':

                {
                    uint16_t freq = 0, msec = 0;
                    fprintf(stderr, "Beep Cmd received\r\n");
                    if (sscanf(buf + 1, "%hu%hu", &freq, &msec) == 2) {
                        fprintf(stderr, "Beepping in %u %u\r\n", freq, msec);
                        buzzer_play(freq, msec, volume);
                    } else {
                        fprintf(stderr, "Beepping argv error: %s\r\n", buf + 1);
                    }
                }
                break;

            case 'S':
                {
                    fprintf(stderr, "Setting Cmd received\r\n");
                    if (sscanf(buf + 1, "%hu%hu%hu", &pitch, &rate, &volume) == 3) {
                        fprintf(stderr, "Setting in %u %u %u\r\n", pitch, rate, volume);
                        tty_setting(pitch, rate, volume);
                    } else {
                        fprintf(stderr, "Setting argv error: %s\r\n", buf + 1);
                    }
                }
                break;

            case 'G':
            case 'U':
                {
                    bool isGBK = buf[0] == 'G';
                    fprintf(stderr, "ttf Playing Cmd received\r\n");
                    fprintf(stderr, "tts playing argv: %s %s\r\n", isGBK ? "GBK": "UTF8", buf + 1);
                    tts_play(isGBK, buf + 1);
                }
                break;

            case 'T':
                {
                fprintf(stderr, "STOP cmd received\r\n");
                }
                break;

            default:
                fprintf(stderr, "Unknown cmd: %s\r\n", buf);
                break;
            }
    }
}

static char *argv0;
void set_cmd_line_argv0(const char *cmd_line_argv0)
{
    if (argv0 == NULL || strlen(cmd_line_argv0) > strlen(argv0)) {
        return;
    }
    strcpy(argv0, cmd_line_argv0);
}


#if 0
time_t time(time_t *tloc)
{
    time_t ret = 1551603667;
    if (tloc) {
        *tloc = ret;
    }
    return ret;
}
#endif


int main(int argc, char *argv[])
{
    argv0 = argv[0];
    tts_cmd_loop();
    _exit(0);
    return 0;
}
