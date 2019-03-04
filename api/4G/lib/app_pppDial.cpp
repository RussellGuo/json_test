#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <utils/Log.h>
#include <sys/wait.h>
#include <time.h>
#include "include/sprd_ril.h"

using namespace android;

#define LOG_TAG "app_pppDial"

#define USBDEV "/dev/ttyGS1"


/****** Global variable declaration******/
pid_t  mPid = 0;
static bool s_pppd_attached_flag = false;
static pthread_mutex_t s_pppd_killed_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t s_pppd_killed_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t s_timer_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t s_timer_cond = PTHREAD_COND_INITIALIZER;


/**** extern external vars & func ****/
extern bool s_setup_dataconn_done;
extern RIL_RegState s_reg_state;
extern struct PDP_INFO pdp_info;
extern void initUsb(void);
extern void rebootUsb(void);

/****** function declaration******/
static bool isNetworkRegSuccess(void);
static void detachPppd(void);
static void setTimespecRelative(struct timespec *p_ts, long long msec);
static void *monitor_pppd_state_thread(void* para);
static void *detect_dataConn_thread(void* para);
static void *timer_thread(void* para);
void initPppDial(void);
void attachPppd(void);


/****** function definition******/
static bool isNetworkRegSuccess(void){
    return ( s_reg_state == RIL_REG_STATE_HOME )?true:false;
}

//pppd -detach 192.168.1.201:10.99.148.132 /dev/ttyGS1 noauth ms-dns 211.136.112.50
void attachPppd(void){
    pid_t  pid, wret, eret;
    int status;
    char route_string[128];

    ALOGI("%s: ip = %s, dns = %s, ifname = %s", __FUNCTION__, pdp_info.ip, pdp_info.dns1addr, pdp_info.ifname );
    pid = fork();
    ALOGI( "%s: pid = %d.", __FUNCTION__, pid );
    if ( pid < 0 ){
        ALOGI("%s: fork failed (%s)", __FUNCTION__, strerror(errno) );
    }else if ( pid == 0 ){ // run in child process
        char address[40];
        sprintf(address,"%s:%s","192.168.168.1", pdp_info.ip );
        int eret = execl("/system/bin/pppd", "pppd", "-detach", address, USBDEV, "noauth", "ms-dns", pdp_info.dns1addr, "lcp-max-configure", "99999", (char*)NULL);
        ALOGI("%s: eret = %d", __FUNCTION__, eret );
        if ( eret < 0 ){
            ALOGI("%s: execl faild (%s)", __FUNCTION__, strerror(errno) );
        }

    } else { //run in parent process
        ALOGI("%s: monitoring...", __FUNCTION__);
        mPid = pid;

        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        ALOGI("%s: flush route table for ppp. ", __FUNCTION__);
        sprintf(route_string," /system/bin/ppp_route.sh %s %s", pdp_info.ifname, pdp_info.ip);
        system( route_string );
        //set flag = true
         s_pppd_attached_flag = true;

         ALOGI("%s: set pppd attached flag = true", __FUNCTION__);
        //create a child thread, to monitor child process state
        if (pthread_create(&tid, &attr, monitor_pppd_state_thread, &mPid) < 0){
            ALOGI("%s: Failed to create monitor_pppd_state_thread", __FUNCTION__);
        }

        //wait child process killed signal
        pthread_mutex_lock(&s_pppd_killed_mutex);
        pthread_cond_wait(&s_pppd_killed_cond, &s_pppd_killed_mutex);
        pthread_mutex_unlock(&s_pppd_killed_mutex);

        //set flag = false
        s_pppd_attached_flag = false;

        //reboot uart
        rebootUsb();
    }
}

static void detachPppd(void){
    ALOGI("%s", __FUNCTION__);
    if (mPid == 0)
    {
        ALOGI("%s: pppd already stopped.", __FUNCTION__);
        return;
    }

    kill( mPid, SIGTERM );
    waitpid( mPid, NULL, 0 );
    mPid = 0;
    ALOGI("%s: pppd services stopped.", __FUNCTION__ );
}

static void setTimespecRelative(struct timespec *p_ts, long long msec){
    struct timeval tv;

    gettimeofday(&tv, (struct timezone *) NULL);

    /* what's really funny about this is that I know
       pthread_cond_timedwait just turns around and makes this
       a relative time again */
    p_ts->tv_sec = tv.tv_sec + (msec / 1000);

    p_ts->tv_nsec = (tv.tv_usec + (msec % 1000) * 1000L ) * 1000L;

}

static void *timer_thread(void* para){
    struct timespec ts;

    ALOGI("%s: wait for 10s.", __FUNCTION__);
    setTimespecRelative( &ts, 10000); //wait for 10s

    pthread_mutex_lock( &s_timer_mutex );
    pthread_cond_timedwait( &s_timer_cond, &s_timer_mutex, &ts );
    ALOGI("%s: time out .. return..", __FUNCTION__);
    pthread_mutex_unlock( &s_timer_mutex );

    return NULL;
}

static void *detect_dataConn_thread(void* para){

    ALOGI("%s", __FUNCTION__);
    while(1){

        //here, process time wait 10s, check data connection state again.
        if ( (s_setup_dataconn_done == false) && (s_pppd_attached_flag == true) ){
        //if ( s_pppd_attached_flag == true) {

            ALOGI("%s: current pppd has attached, when data link may disconnect, start timer to ensure data link really disconnected.",  __FUNCTION__);
            pthread_t tid;
            pthread_attr_t attr;
            pthread_attr_init(&attr);

            pthread_mutex_init( &s_timer_mutex, NULL );
            pthread_cond_init( &s_timer_cond, NULL );

            //create a child thread, to monitor child process state
            if (pthread_create(&tid, &attr, timer_thread, NULL) < 0){
                ALOGI("%s: Failed to create timer_thread", __FUNCTION__);
            }

            pthread_join( tid, NULL );
            ALOGI("%s: wait tid thread finished.", __FUNCTION__);
            if ( (s_setup_dataconn_done == false) && (s_pppd_attached_flag == true) ){
            //if ( s_pppd_attached_flag == true) {
                ALOGI("%s: pdp link really disconnected, to detach pppd and reboot uart", __FUNCTION__);

                //release pppd
                detachPppd();

                //set flag = false
                s_pppd_attached_flag = false;

                //reboot uart
                rebootUsb();
            }
        }

    }
    return NULL; 
}

static void *monitor_pppd_state_thread(void* para){
    pid_t pid = *(pid_t *)para;
    int wret;

    wret = waitpid( pid, NULL, 0 );

    if ( wret == pid ){
        ALOGI("%s: pppd already release, send killed signal", __FUNCTION__ );

        /* send signal to parent process */
        pthread_mutex_lock(&s_pppd_killed_mutex);
        pthread_cond_signal(&s_pppd_killed_cond);
        pthread_mutex_unlock(&s_pppd_killed_mutex);

    }else{
        ALOGI("%s: some error occured.", __FUNCTION__ );
    }

    //exit thread
    pthread_exit(NULL);

    return NULL;
}

void initPppDial(void){
    ALOGI("%s", __FUNCTION__);

    initUsb();

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if (pthread_create(&tid, &attr, detect_dataConn_thread, NULL) < 0){
        ALOGI("%s: Failed to create detect_dataConn_thread", __FUNCTION__);
    }
}
