#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <utils/Log.h>

using namespace android;

#define LOG_TAG "app_at"

#define USBDEV "/dev/ttyGS1"


/****** Global variable declaration******/
int s_usb_fd = -1;
char atcmd[1024] = {0}; 
char response_pc[1024] = {0};
static pthread_mutex_t s_USBMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t s_writetopc_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  s_writetopc_cond = PTHREAD_COND_INITIALIZER;


/**** extern external vars & func ****/
extern bool s_setup_dataconn_done;
extern void attachPppd(void);

/****** function declaration******/
static void read_or_write_USB(bool flag);
static void *readUSB_thread(void* para);
static void *writeUSB_thread(void* para);
static void closeUsb(void);
static void setTermios(struct termios *pNewtio, unsigned short uBaudRate);
void rebootUsb(void);
void initUsb(void);

/****** function definition******/
//flag=true: write into USB;  flag=false: read from USB
static void read_or_write_USB(bool flag){
    int res = 0;
    int nread;
    fd_set rfds;
    int ret = 0;
    int n;

    if (!flag) {
        ALOGI( "%s: start read USB dev", __FUNCTION__ );
        FD_ZERO(&rfds);
        FD_SET(s_usb_fd, &rfds);
        res = select(s_usb_fd + 1, &rfds, NULL, NULL, NULL);
        memset(atcmd, 0, sizeof(atcmd));
    }

    pthread_mutex_lock(&s_USBMutex);

    if (!flag) {
        while(1) {
            n = read(s_usb_fd, atcmd, sizeof(atcmd) - 1);

            if(n==0){
              sleep(1);
              continue;
            }
            atcmd[n]=0;
            ALOGI("%s: Get AT and send OK,n=%d, atcmd = %s. \n", __FUNCTION__, n, atcmd);
            break;
        }
    } else {

        ret = write(s_usb_fd, response_pc, strlen(response_pc) + 1);
        if(ret < 0){
            ALOGI( "%s: write fail, errno = %s, ret = %d, str is %s. ", __FUNCTION__, strerror(errno), ret, response_pc );
        }else{
            ALOGI( "%s: send %d bytes, str is %s .\n", __FUNCTION__, ret, response_pc );
        }

        pthread_mutex_lock(&s_writetopc_mutex);
        pthread_cond_signal(&s_writetopc_cond);
        pthread_mutex_unlock(&s_writetopc_mutex);
    }

    pthread_mutex_unlock(&s_USBMutex);
}

/*
**Read usb at command  and send request
*/
static void *readUSB_thread(void* para) {
    char *p = NULL;
    char  res_buf[] = {0x0D,0x0A,0x4F,0x4B,0x0D,0x0A};// "..OK.."
    char  res_con[] = {0x0D,0x0A, 0x43, 0x4F, 0x4E, 0x4E, 0x45, 0x43, 0x54, 0x20, 0x32, 0x39, 0x36, 0x0D, 0x0A};// "..CONNECT 296.."
    while(1) {

        //read usb at command
        read_or_write_USB(false);
        p = strstr(atcmd, "ATD");
        if ( p ){
            /* do ppp dial connect request */
            /* get current gprs attach state, if state is attach, direct response ok to pc */
            while(1){
                if (!s_setup_dataconn_done ){
                    sleep(1);
                    continue;
                }
                break;
            }

            ALOGI( "%s: setup dataconnection done.", __FUNCTION__ );
            memset( response_pc, 0, sizeof(response_pc) );
            memcpy( response_pc, res_con, sizeof(res_con) );
            /* close usb fd, then attach pppd */
            pthread_mutex_lock(&s_writetopc_mutex);
            pthread_cond_wait(&s_writetopc_cond, &s_writetopc_mutex);
            pthread_mutex_unlock(&s_writetopc_mutex);
            closeUsb();
            attachPppd();

        }else{
            memset( response_pc, 0, sizeof(response_pc) );
            memcpy( response_pc, res_buf, sizeof(res_buf) );
        }

    }
    return NULL; 
}

/*
**Read rild socket data and write to usb dev
*/
static void *writeUSB_thread(void* para) {

    while(1) {

        if (strlen(response_pc) == 0){
            sleep(1);
            continue;
        }

        read_or_write_USB(true);
        memset( response_pc, 0, sizeof(response_pc) );
    }
    return NULL; 
}

static void closeUsb(void){
    ALOGI("%s", __FUNCTION__);
    close(s_usb_fd);
}

/*
 *初始化串口选项
 */
static void setTermios(struct termios *pNewtio, unsigned short uBaudRate){

      bzero(pNewtio,sizeof(struct termios));/* clear struct for new port settings */

      pNewtio->c_cflag = uBaudRate|CS8|CREAD|CLOCAL;

      pNewtio->c_iflag = IGNPAR;

      pNewtio->c_oflag = 0;

      pNewtio->c_lflag = 0;

      /* initialize all control characters  */
      pNewtio->c_cc[VINTR] = 0;

      pNewtio->c_cc[VQUIT] = 0;

      pNewtio->c_cc[VERASE] = 0;

      pNewtio->c_cc[VKILL] = 0;

      pNewtio->c_cc[VEOF] = 4;

      pNewtio->c_cc[VTIME] = 5;

      pNewtio->c_cc[VMIN] = 0;

      pNewtio->c_cc[VSWTC] = 0;

      pNewtio->c_cc[VSTART] = 0;

      pNewtio->c_cc[VSTOP] = 0;

      pNewtio->c_cc[VSUSP] = 0;

      pNewtio->c_cc[VEOL] = 0;

      pNewtio->c_cc[VREPRINT] = 0;

      pNewtio->c_cc[VDISCARD] = 0;

      pNewtio->c_cc[VWERASE] = 0;

      pNewtio->c_cc[VLNEXT] = 0;

      pNewtio->c_cc[VEOL2] = 0;

}

void rebootUsb(void){
    int retry_times = 0;
    struct termios oldtio,newtio;

     retry_times = 0;
     s_usb_fd = open(USBDEV, O_RDWR|O_NOCTTY);
     while (s_usb_fd < 0 && retry_times < 10) {
         retry_times ++;
         sleep(1);
         s_usb_fd = open(USBDEV, O_RDWR|O_NOCTTY);
    }
    ALOGI("%s: s_usb_fd = %d !", __FUNCTION__, s_usb_fd);

    if (s_usb_fd < 0) {
        ALOGI("%s: open %s failed!, s_usb_fd = %d.\n", __FUNCTION__, USBDEV, s_usb_fd);
        //return -1;
    }

    tcgetattr(s_usb_fd,&oldtio);
    setTermios(&newtio,B115200);
    tcflush(s_usb_fd,TCIFLUSH);
    tcsetattr(s_usb_fd,TCSANOW,&newtio);
}

void initUsb(void){
    int retry_times = 0;
    pthread_t tid;
    pthread_attr_t attr;
    struct termios oldtio,newtio;

    //open usb dev and create thread for reading data from pc and writing data to pc
    //pppbrd_usb_dev_init();
    retry_times = 0;
    s_usb_fd = open(USBDEV, O_RDWR|O_NOCTTY);
    while (s_usb_fd < 0 && retry_times < 10) {
         retry_times ++;
         sleep(1);
         s_usb_fd = open(USBDEV, O_RDWR|O_NOCTTY);
    }
    ALOGI("%s: s_usb_fd = %d !", __FUNCTION__, s_usb_fd);

    if (s_usb_fd < 0) {
        ALOGI("%s: open %s failed!, s_usb_fd = %d.\n", __FUNCTION__, USBDEV, s_usb_fd);
        //return -1;
    }

    tcgetattr(s_usb_fd,&oldtio);
    setTermios(&newtio,B115200);
    tcflush(s_usb_fd,TCIFLUSH);
    tcsetattr(s_usb_fd,TCSANOW,&newtio);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if (pthread_create(&tid, &attr, readUSB_thread, NULL) < 0) {
        ALOGI("%s: Failed to create readUSB_thread", __FUNCTION__);
    }

    if (pthread_create(&tid, &attr, writeUSB_thread, NULL) < 0) {
        ALOGI("%s: Failed to create writeUSB_thread", __FUNCTION__);
    }
}
