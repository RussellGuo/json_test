#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <utils/Log.h>

#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>

#include "aud_player.h"
#include <media/mediaplayer.h>
#include <media/MediaPlayerInterface.h>
#include <media/stagefright/foundation/ABase.h>
#include <media/stagefright/foundation/AString.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include <system/audio.h>
#include <system/audio_policy.h>
#include <pthread.h>
#include <sched.h>
#include <utils/String8.h>
#include <utils/KeyedVector.h>


#define LOG_DEBUG 0

//#if LOG_DEBUG>=1
//#undef ALOGE

#define LOGD(...)  printf("hal_test: " __VA_ARGS__), printf("  \n")
#define LOGE(...) LOGD(__VA_ARGS__)
#define ALOGD(...)  printf("hal_test: " __VA_ARGS__), printf("  \n")
#define ALOGE(...) LOGD(__VA_ARGS__)
#define ALOGI(...) LOGD(__VA_ARGS__)
//#define ALOGE_IF(...)
//#endif


namespace android {

typedef struct t_playerTrack
{
    char flagStart;
    sp<MediaPlayer> pMediaPlayer;
    //sp<PlayerListener> pListener;
    pthread_mutex_t lock;
}PLAYER_TRACK;

PLAYER_TRACK *pPlayerInfo=NULL;

extern "C"
{
    playerhandle  player_open(char* path);
	int player_prepare(playerhandle handle);
	int player_start(playerhandle handle);
	int player_pause(playerhandle handle);
	int player_stop(playerhandle handle);
	int player_delete(playerhandle handle);

	int player_set_volume(char *dev, int l_vol, int r_vol);
	int player_set_volume(char *dev, int l_vol, int r_vol);
	int player_set_device(playerhandle pPlayerInfo, char *device);
}

playerhandle  player_open(char *path)
{
    status_t status;
   
    pPlayerInfo = (PLAYER_TRACK *)malloc(sizeof(PLAYER_TRACK ));
    if(!pPlayerInfo) {
        return NULL;
    }
    else {
        memset(pPlayerInfo, 0, sizeof(PLAYER_TRACK));
    }
    pthread_mutex_init(&pPlayerInfo->lock, NULL);

    android::ProcessState::self()->startThreadPool();

    const KeyedVector<String8, String8> kheader;

    //kheader.add(String8("key1"), String8("value1"));

    pPlayerInfo->pMediaPlayer = new MediaPlayer();
    //pPlayerInfo->pListener = new PlayerListener();
    //pPlayerInfo->pMediaPlayer->setListener(pPlayerInfo->pListener);

    //int fd = open(path,O_RDONLY);
    int fd = open(path, O_RDONLY);
    pPlayerInfo->pMediaPlayer->setDataSource(fd,0,0x7fffffffL);

    //pPlayerInfo->pMediaPlayer->setDataSource("http://47.98.36.22/185811.mp3", NULL);

    //pPlayerInfo->pMediaPlayer->setDataSource("http://47.98.36.22/185811.mp3", &kheader);

    ALOGE("player_open out");
    return (playerhandle)pPlayerInfo;
error:
    if(pPlayerInfo->pMediaPlayer != NULL) {
        pPlayerInfo->pMediaPlayer.clear();
    }
    if(pPlayerInfo) {
        free(pPlayerInfo);
    }
    ALOGE("pMediaPlayer open out error");
    return NULL;
}

int player_prepare(playerhandle handle)
{
    int status = 0;
    PLAYER_TRACK *pPlayerInfo = NULL;
    ALOGE("player_prepare in");
    pPlayerInfo = (PLAYER_TRACK*)handle;
    pthread_mutex_lock(&pPlayerInfo->lock);
    status = pPlayerInfo->pMediaPlayer->prepare();
    if(0 != status)
    {
        ALOGE("warning:start error: status = %d", status);
        return status;
    }
    pthread_mutex_unlock(&pPlayerInfo->lock);

    ALOGE("player_prepare out");	
    return 0;
}

int player_start(playerhandle handle)
{
    PLAYER_TRACK *pPlayerInfo = NULL;
    ALOGE("player_start in");
    pPlayerInfo = (PLAYER_TRACK*)handle;
    pPlayerInfo->pMediaPlayer->setLooping(0);
    pPlayerInfo->pMediaPlayer->start();
	pPlayerInfo->pMediaPlayer->setVolume(1.0f, 1.0f);
	
    while (!pPlayerInfo->pMediaPlayer->isPlaying()) {
	sleep(1);
        LOGE("peter: while player start loop\n");
    }
    ALOGE("player_start out");
    return 0;
}

int player_pause(playerhandle handle)
{
    PLAYER_TRACK *pPlayerInfo = NULL;
    ALOGE("player_pause in");
    pPlayerInfo = (PLAYER_TRACK*)handle;
    pthread_mutex_lock(&pPlayerInfo->lock);
    pPlayerInfo->pMediaPlayer->pause();
    pthread_mutex_unlock(&pPlayerInfo->lock);

    ALOGE("player_pause out");
    return 0;
}

int player_stop(playerhandle handle)
{
    PLAYER_TRACK *pPlayerInfo = NULL;
    ALOGE("player_stop in");
    pPlayerInfo = (PLAYER_TRACK*)handle;
    pthread_mutex_lock(&pPlayerInfo->lock);
    pPlayerInfo->pMediaPlayer->stop();
    pthread_mutex_unlock(&pPlayerInfo->lock);
    ALOGE("player_stop out");
    return 0;
}

int player_delete(playerhandle handle)
{
    PLAYER_TRACK *pPlayerInfo = NULL;
    ALOGE("player_delete in");
    pPlayerInfo = (PLAYER_TRACK*)handle;
    //delete pPlayerInfo->pMediaPlayer;
    //delete pPlayerInfo->pListener;
	pPlayerInfo->pMediaPlayer.clear();
    ALOGE("player_delete out");
    return 0;
}

int player_get_volume(char *dev, int *l_vol, int *r_vol)
{
	return 0;
}

int player_set_volume(char *dev, int l_vol, int r_vol)
{
	return 0;
}

int player_set_channel_volume(playerhandle pPlayerInfo, float multiplier)
{
	return 0;
}

int player_set_device(playerhandle pPlayerInfo, char *device)
{
	return 0;	
}

}


