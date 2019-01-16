#include <stdio.h>  
#include <stdlib.h>  
#include <fcntl.h>  
    /* example.c*/
#include <stdio.h>
#include <pthread.h>
#include <sched.h>

#include <sys/resource.h>
#include <semaphore.h>

#include <time.h>

#include "aud_player.h"



#define LOG_DEBUG 1

#if LOG_DEBUG>=1
#define LOGD(...)  printf("hal_test: " __VA_ARGS__), printf("  \n")
#define LOGE(...) LOGD(__VA_ARGS__)
#define ALOGD(...)  printf("hal_test: " __VA_ARGS__), printf("  \n")
#define ALOGE(...) LOGD(__VA_ARGS__)
#define ALOGI(...) LOGD(__VA_ARGS__)
#define ALOGE_IF(...)
#endif

#define false 0
#define true     1 



char* filename = "/system/bin/123.wav";

void * player_thread(void * arg)
{
    playerhandle hrec = NULL;

    hrec = player_open(filename);
    player_prepare(hrec);
    player_start(hrec);

    //player_stop(hrec);
    //player_delete(hrec);
	
    return NULL;
}


int main(int argc, char** argv)  
{  
    int val = 0; 
    int *data=NULL;
    pthread_t id0;
    pthread_attr_t attr;
    struct sched_param param;
    int newprio=99;
    
    int i,ret;
    int rc;

    LOGD("peter:start sipc test:\n");  


    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS); // or PTHREAD_SCOPE_PROCESS PTHREAD_SCOPE_SYSTEM

    pthread_attr_setschedpolicy(&attr, SCHED_FIFO); // or SCHED_FIFO  SCHED_RR SCHED_RR
     
    pthread_attr_getschedparam(&attr, &param);
    param.sched_priority=newprio;
    pthread_attr_setschedparam(&attr, &param);

    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE); // or PTHREAD_CREATE_JOINABLE PTHREAD_CREATE_DETACHED


    LOGD("peter:start sipc test: load lib in\n");


    rc = pthread_create(&id0, NULL, player_thread, (void *)NULL);
    if (rc) {
		ALOGE("pthread_create failed");
    }

    for(i=0;i<3;i++)
        LOGD("This is the main process.\n");

    sleep(10);

    return 0;  
}  

