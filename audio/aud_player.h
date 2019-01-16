#ifndef __SPRD_FM_TRACK_H__
#define __SPRD_FM_TRACK_H__
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <utils/Log.h>
#include <semaphore.h>
#include <stdlib.h>
#include <linux/types.h>



typedef void*  playerhandle;

extern playerhandle  player_open(char *path);
//extern audio_player_t *audio_player_new(audio_player_callback ccb, void *userdata);
extern int player_prepare(playerhandle playerhandle);
extern int player_start(playerhandle handle);
extern int player_pause(playerhandle handle);
extern int player_stop(playerhandle handle);
extern int player_delete(playerhandle handle);

extern int player_get_volume(char *dev, int *l_vol, int *r_vol);
extern int player_set_volume(char *dev, int l_vol, int r_vol);

extern int player_set_channel_volume(playerhandle handle, float multiplier);
extern int player_set_device(playerhandle handle, char *device);

#endif
