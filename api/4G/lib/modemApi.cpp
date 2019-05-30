#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <alloca.h>
#include <arpa/inet.h>
#include <cutils/sockets.h>
#include <binder/Parcel.h>
#include <cutils/jstring.h>
#include <utils/Log.h>
#include <sys/wait.h>
#include <signal.h>
#include <cutils/properties.h>
#include "include/ril_command.h"
#include "record_stream.h"
#include "include/sprd_ril.h"
//Add by wangcong for ICCID
#include "iccConstants.h"
//Add by wangcong for ChangeNetWork
#include <cutils/properties.h>

using namespace android;
using namespace std;

//#ifndef LOG_TAG
#define LOG_TAG "AIPenBox"
//#endif

#define SOCKET_NAME "rild"

#define MAX_COMMAND_BYTES (8 * 1024)
#define MAX_POOL_COUNT 5
#define PDP_STATUS_ACTIVE 1
#define PDP_STATUS_DEACTIVE 0
//Add by wangcong for PDP
#define MAX_PDP 11

typedef struct request{
    int serial;
    int requestId;
    bool isFree;
    bool persist;
    struct request *next;
}requestList;

typedef struct
{
    int mcc;
    int mnc;
    int ci;
    int pci;
    int tac;
} Cellinfo;

struct PDP_INFO pdp_info;
int MCC,MNC,CI,PCI,TAC = 0;
int signal_strength = -100;
int simFlag = 0;
int cellFlag = 1;
int netWorkFlag = 1;
int inner = 0;
int outState = 0;
int pdpState = 0;//Add by wangcong in 5.28
bool isSimReadyret = false;
bool states = false;
int isRomaing = 0;
int netRegStatus = 0;
int s_active_pdp_cid = 0;
int s_socket_fd = -1;
int pdpFlag = 1;
static bool s_radio_power_on = false;
char cid[8] = {0};
char imsiid[32] = {0};
char sw[32] = {0};
char sw0[32] = {0};
char manuFacture[32] = {0};
char SimResponse[32] = {0};
char IMEI[32] = {0};
char netOwner[64] = {0};
char numberForNetType[8] = {0};
char netType[32] = {0};
char netWorkOwner [32]= {0};
char netTypeWork [32]= {0};
char ICCID[64]= {0};
char AT[64] = {0};
static int s_serial = 0;
static requestList* s_request_header = NULL;
static RIL_RadioState s_radioState;
static int s_net_type = -1; //Record current network type
static bool s_is_roaming = false; //Record current network if roaming
//Add by wangcong for ICCID
static int s_cur_get_fileid = 0xffff; //Record current access EF file Id
//Add by wangcong for PDP
int num_pdp;
struct PDP_INFO pdplist[MAX_PDP];// record data call list


bool already_regist = false;

int s_gprs_attach_state = -1;
bool s_setup_dataconn_done = false;
RIL_RegState s_reg_state = RIL_REG_STATE_NOT_REG;
pthread_mutex_t s_network_register_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  s_network_register_cond = PTHREAD_COND_INITIALIZER;
//add by wangcong for PDP
pthread_mutex_t s_get_data_call_list_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  s_get_data_call_list_cond = PTHREAD_COND_INITIALIZER;


pthread_mutex_t s_network_api_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  s_network_api_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t s_network_dial_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  s_network_dial_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t s_network_signal_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  s_network_signal_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t s_network_accept_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  s_network_accept_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t s_network_cell_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  s_network_cell_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t s_network_cellid_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  s_network_cellid_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t s_network_imsi_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  s_network_imsi_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t s_network_imei_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  s_network_imei_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t s_network_gprs_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  s_network_gprs_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t s_network_detactive_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  s_network_detactive_cond = PTHREAD_COND_INITIALIZER;
/*extern external vars & func */
#if defined (APP_PPPDIAL_MODULE)
extern void initPppDial(void);
#endif

/* function implementation */
/*
extern "C"
int add(int a , int b , int (*add_value)())
{
return (*add_value)(a,b);
}
*/

static void initRequestList(){
    int i = 0;
    requestList* temp;
    for(i = 0; i < MAX_POOL_COUNT; ++i){
        temp = (requestList *)malloc(sizeof(requestList));
        if(temp == NULL){
            ALOGI("malloc failed!");
            return;
        }
        temp->serial = 0;
        temp->requestId = 0;
        temp->isFree = true;
        temp->persist = true;
        temp->next = NULL;
        if(s_request_header == NULL){
            s_request_header = temp;
        }else{
            temp->next = s_request_header;
            s_request_header = temp;
        }
    }
}
/****
find a free node
****/
static requestList* findFreeListNode(){
    requestList* temp = s_request_header;

    while(temp != NULL){
        if(temp->isFree) return temp;
        temp = temp->next;
    }

    temp = (requestList *)malloc(sizeof(requestList));
    if(temp == NULL) return NULL;

    temp->serial = 0;
    temp->requestId = 0;
    temp->isFree = true;
    temp->persist = false;
    temp->next = NULL;

    temp->next = s_request_header;
    s_request_header = temp;

    return temp;
}

static int findRequestIdBySerial(int serial){
    int requestId;
    requestList* temp_pre = NULL;
    requestList* temp = s_request_header;

    while(temp != NULL && temp->serial != serial){
        temp_pre = temp;
        temp = temp->next;
    }

    if(temp == NULL){
        ALOGI("not find!");
        return 0;
    }

    requestId = temp->requestId;

    if(!temp->isFree){
        temp->isFree = true;
    }

    if(!temp->persist){
        if(temp_pre == NULL){
            s_request_header = temp->next;
        }else{
            temp_pre->next = temp->next;
        }
        free(temp);
    }

    return requestId;

}


static char* strdupReadString(Parcel &p) {
    size_t stringlen;
    const char16_t *s16;

    s16 = p.readString16Inplace(&stringlen);

    return strndup16to8(s16, stringlen);
}

static void writeStringToParcel(Parcel &p, const char *s) {
    char16_t *s16;
    size_t s16_len;
    s16 = strdup8to16(s, &s16_len);
    p.writeString16(s16, s16_len);
    free(s16);
}

int blockingWrite(int fd, const void *buffer, size_t len) {
    size_t writeOffset = 0;
    const uint8_t *toWrite;

    if (fd < 0) {
        ALOGE("fd < 0");
    }

    toWrite = (const uint8_t *)buffer;

    while (writeOffset < len) {
        ssize_t written;
        do {
            written = write (fd, toWrite + writeOffset,
                                len - writeOffset);
        } while (written < 0 && ((errno == EINTR) || (errno == EAGAIN)));

        if (written >= 0) {
            writeOffset += written;
        } else {   // written < 0
            ALOGE("RIL Response: unexpected error on write errno:%d, %s", errno, strerror(errno));
            return -1;
        }
    }

    ALOGI("blockingWrite OVER");
    return 0;
}

int sendRequest (const void *data, size_t dataSize, int fd) {
    int ret;
    uint32_t header;

    if (fd < 0)
        return -1;

    ALOGI("start send request");
    if (dataSize > MAX_COMMAND_BYTES) {
        ALOGE("packet larger than %u (%u)",
                MAX_COMMAND_BYTES, (unsigned int )dataSize);
        return -1;
    }

    header = htonl(dataSize);
    ret = blockingWrite(fd, (void *)&header, sizeof(header));
    if (ret < 0) {
        ALOGE(" blockingWrite header error");
        return ret;
    }

    ret = blockingWrite(fd, data, dataSize);
    if (ret < 0) {
        ALOGE(" blockingWrite data error");
        return ret;
    }

    ALOGI("send request over");
    return 0;
}

void updatePdpInfo(RIL_Data_Call_Response_v11* datalist){
    ALOGI("%s", __FUNCTION__);

    if (datalist == NULL)
        return;

    memset(&pdp_info, 0x0, sizeof(pdp_info) );
    pdp_info.status = datalist->status;
    pdp_info.cid = datalist->cid;
    pdp_info.active = datalist->active;

    if ( datalist->active == PDP_STATUS_ACTIVE ){
        s_setup_dataconn_done = true;
    }else{
        s_setup_dataconn_done = false;
    }

    memcpy(pdp_info.type, datalist->type, strlen(datalist->type) + 1 );
    memcpy(pdp_info.ifname, datalist->ifname, strlen(datalist->ifname) + 1 );

    std::string address;
    address = datalist->addresses;
    char* ip;
    ip = (char*)(address.substr( 0, address.find(" ") )).data();
    memcpy(pdp_info.ip, ip, strlen(ip) + 1 );

    std::string dnses;
    dnses = datalist->dnses;
    char* dns1;
    dns1 = (char*)(dnses.substr( 0, dnses.find(" ") )).data();
    memcpy(pdp_info.dns1addr, dns1, strlen(dns1) + 1 );

    ALOGI( "%s: pdp_info status:%d, cid:%d, active:%d, type:%s, ifname:%s, addr:%s, dns1:%s", __FUNCTION__,
            pdp_info.status, pdp_info.cid, pdp_info.active, pdp_info.type, pdp_info.ifname, pdp_info.ip, pdp_info.dns1addr );
}

//Add by wangcong for IMEI
extern "C"
void getIMEI(char* imei){
    if(strcmp(IMEI,"0") > 0){
      ALOGE("The last time IMEI is %s",IMEI);
      strcpy(imei,IMEI);
      return ;
    }
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return;
    
    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_GET_IMEI;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token
    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);
    if (ret < 0) {
        ALOGE("send at ERROR!");
    }
    ALOGE("Beginning imei2 =====");
     //Add by wangcong
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_imei_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 2;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_imei_cond, &s_network_imei_mutex, &outtime);
    pthread_mutex_unlock(&s_network_imei_mutex);
    strcpy(imei,IMEI);
    ALOGI("write 'get imei' into socket OVER!");
}


/**
* 函数说明：初始化网络 ，否则后面函数不能运行
* 参数：信号格式，配置文件，apn，账户，密码，认证方法，协议
* DataCallConconnect("16", "0", "3gnet", "", NULL, "0", "IPV4V6");
* 返回值：0： 成功
*        -1：失败
*/
extern "C"
int DataCallConconnect(const char* radioTechnology, const char* profile, const char* apn, const char* user, const char* password, const char* authType, const char* protocol) {
    int ret;
    Parcel p;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return 1;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_SETUP_DATA_CALL;
    rnode->isFree = false;

    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token
    p.writeInt32(7);
    writeStringToParcel(p, radioTechnology);
    writeStringToParcel(p, profile);
    writeStringToParcel(p, apn);
    writeStringToParcel(p, user);
    writeStringToParcel(p, password);
    writeStringToParcel(p, authType);
    writeStringToParcel(p, protocol);

    ALOGI("setup data call data data ====");
    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }
    //Add by wangcong
    struct timeval now;
    struct timespec outtime;
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 3;
    outtime.tv_nsec = now.tv_usec * 1000;
    pthread_mutex_lock(&s_network_gprs_mutex);
    int red = pthread_cond_timedwait(&s_network_gprs_cond, &s_network_gprs_mutex, &outtime);
    pthread_mutex_unlock(&s_network_gprs_mutex);
   if(red == ETIMEDOUT)
       return -100;
    return pdpFlag;
}

static void radioPower(bool isOn) {
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_RADIO_POWER;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token
    p.writeInt32(1);
    p.writeInt32(isOn?1:0);

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }

    ALOGI("write 'RadioPowerOn' into socket OVER!");
}

static void getOperatorPlmn(char* netWorkOwner){
    int ret;
    Parcel p;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_OPERATOR;
    rnode->isFree = false;

    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if(ret < 0){
        ALOGE("send at error");
    }

    ALOGI("write 'get operator plmn' into socket over!");
     //Add by wangcong
    /*struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_api_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 5;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_api_cond, &s_network_api_mutex, &outtime);
    pthread_mutex_unlock(&s_network_api_mutex);*/
    strcpy(netWorkOwner,netOwner);
//    if(red == ETIMEDOUT)
//        return -100;
}

void getIccCardStateInner(){
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();
    inner = 1;
    if(rnode == NULL) return ;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_GET_SIM_STATUS;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }
	ALOGI("simFlaginner: %d", simFlag);
    ALOGI("write 'get sim statusInner' into socket OVER!");
}

extern "C"
int getIccCardState(){
    /*int ret;
    requestList* rnode;

    rnode = findFreeListNode();
    outState = 1;
    if(rnode == NULL) return 0;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_GET_SIM_STATUS;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }
    //Add by wangcong
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_api_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 8;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_api_cond, &s_network_api_mutex, &outtime);
    pthread_mutex_unlock(&s_network_api_mutex);
    if(red == ETIMEDOUT)
        return -100;
    ALOGI("write 'get sim status' into socket OVER!");
	outState = 0;*/	
	/*while(simFlag == 0){
	  ALOGI("simFlagBefore: %d",simFlag);
	  sleep(1);
	  getIccCardStateInner();
	  ALOGI("simFlagInside: %d",simFlag);
	}*/
	if(simFlag == 0){ 
	ALOGI("05302022");
	ALOGI("simFlagBefore: %d",simFlag);
	sleep(1);
	getIccCardStateInner();
	ALOGI("simFlagInside: %d",simFlag);
	}
    ALOGI("simFlag: %d", simFlag);
    return simFlag;
}

static bool isSimReady(RIL_CardStatus& cardstatus){
    int i;

    for(i = 0; i < cardstatus.num_applications; i++){
        if(cardstatus.applications[i].app_state != RIL_APPSTATE_READY){
            return false;
        }
    }

    if(i == cardstatus.num_applications) return true;

    return false;
}

extern "C"
int getVoiceNetworkRegistrationState(char* netTypeWork){
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return 0;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_VOICE_REGISTRATION_STATE;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }
    ALOGE("NETWORK REGISTRA:(%d)",rnode->requestId);
    ALOGE("NETWORK REGISTRA1:(%d)",RIL_REQUEST_VOICE_REGISTRATION_STATE);
    ALOGE("NETWORK REGISTRA2:(%d)",ret);
    //Add by wangcong
    /*struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_api_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 5;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_api_cond, &s_network_api_mutex, &outtime);
    pthread_mutex_unlock(&s_network_api_mutex);*/
    strcpy(netTypeWork,netType);
    //if(red == ETIMEDOUT)
    //    return -100;
    return netWorkFlag;
}

extern "C"
int getDataNetworkRegistrationState(){
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return 0 ;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_DATA_REGISTRATION_STATE;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }
    ALOGE("DATA REGISTRA:(%d)",ret);
    ALOGE("DATA REGISTRA:(%d)",rnode->requestId);
    //Add by wangcong
    /*struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_api_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 5;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_api_cond, &s_network_api_mutex, &outtime);
    pthread_mutex_unlock(&s_network_api_mutex);
    if(red == ETIMEDOUT)
        return -100;*/
    if(netRegStatus && isRomaing){
         return 3;
      }else if(!netRegStatus && isRomaing){
         return 2;
      }else if(netRegStatus && !isRomaing){
         return 1;
      }else{
         return 0;
      }
}

void getDataNetworkRegistrationState_nosync(){
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_DATA_REGISTRATION_STATE;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }
    ALOGE("send DATA REGISTRA:(%d)",ret);
    ALOGE("send DATA REGISTRA:(%d)",rnode->requestId);
}

extern "C"
int getSignalStrength(){
    int ret;
    requestList* rnode;
    rnode = findFreeListNode();

    if(rnode == NULL) return 0;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_SIGNAL_STRENGTH;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }
    ALOGI("write 'get signal strength' into socket OVER!");
    if(signal_strength > 0){
    ALOGE("The last time signal_strength is %d",signal_strength);
    return signal_strength;
    }
    //Add by wangcong
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_signal_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 2;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_signal_cond, &s_network_signal_mutex, &outtime);
    pthread_mutex_unlock(&s_network_signal_mutex);
    if(red == ETIMEDOUT)
        return -100;
    ALOGE("send signal_strength complete!");
    return signal_strength;
}

extern "C"
void pollState(char* netWorkOwner,char* netTypeWork){

    ALOGI("poll network state, radio state: %d", s_radioState);
    switch(s_radioState){
        case RADIO_STATE_OFF:
            ALOGI("it is not need to poll");
            break;

        case RADIO_STATE_UNAVAILABLE:
            ALOGI("it is not need to poll");
            break;

        case RADIO_STATE_ON:
            getOperatorPlmn(netWorkOwner);
            ALOGI("netWorkOwner %s\n",netWorkOwner);
            getVoiceNetworkRegistrationState(netTypeWork);
            ALOGI("netTypeWork %s\n",netTypeWork);
            getDataNetworkRegistrationState();
            break;

        default:
            ALOGE("default value");
            break;
    }

}

extern "C"
int deactivateDataCall(int cid){
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();
    if(rnode == NULL)
        return 1;
    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_DEACTIVATE_DATA_CALL;
    rnode->isFree = false;

    Parcel p;
    char temp_buf[4];
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token
    p.writeInt32(2);
    memset(temp_buf, 0, sizeof(temp_buf));
    snprintf(temp_buf, sizeof(temp_buf), "%d", cid);
    writeStringToParcel(p, temp_buf);   // cid
    writeStringToParcel(p, "0");    //reason

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }
    ALOGE("关闭PDP上下文:(%d)",rnode->requestId);
    ALOGE("关闭PDP上下文:(%d)",ret);
    ALOGI("write 'deactivate data call' into socket OVER!");
    //Add by wangcong
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_detactive_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 2;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_detactive_cond, &s_network_detactive_mutex, &outtime);
    pthread_mutex_unlock(&s_network_detactive_mutex);
    if(red == ETIMEDOUT)
      return -100;
      return 0;
}

//Add by wangcong for cellinfo
//Cellnfo *ci = new Cellnfo();
extern "C"
int getCellInfoList(Cellinfo* cellinfo){
    if(cellinfo == NULL)
        return -200;

    int ret;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return 1;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_GET_CELL_INFO_LIST;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }

    ALOGI("write 'get cell info list' into socket OVER!");
    //Add by wangcong
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_cell_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 2;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_cell_cond, &s_network_cell_mutex, &outtime);
    pthread_mutex_unlock(&s_network_cell_mutex);
    if(red == ETIMEDOUT)
      return -100;
    cellinfo->mcc = MCC;
    cellinfo->mnc = MNC;
    cellinfo->ci = CI;
    cellinfo->pci = PCI;
    cellinfo->tac = TAC;

    return cellFlag;
}

//Add by wangcong for Dial
/**call**/
extern "C"
int dial( const char* number ){
    ALOGI("dial====================");
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL)
        return -1;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_DIAL;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    writeStringToParcel( p, number );
    p.writeInt32(0);//clir mode
    p.writeInt32(0);//uusinfo

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }
    ALOGI("write 'dial' into socket OVER !");
    //Add by wangcong
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_dial_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 5;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_dial_cond, &s_network_dial_mutex, &outtime);
    pthread_mutex_unlock(&s_network_dial_mutex);
    if(red == ETIMEDOUT)
      return -100;
    return 0;
}

static void getCurrentCalls(){
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_GET_CURRENT_CALLS;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }

    ALOGI("write 'get current calls' into socket OVER!");
    //Add by wangcong
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_accept_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 5;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_accept_cond, &s_network_accept_mutex, &outtime);
    pthread_mutex_unlock(&s_network_accept_mutex);
    if(red == ETIMEDOUT)
      return ;
}

extern "C"
void acceptCall(){
    ALOGI("acceptCall====================");
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_ANSWER;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }

    ALOGI("write 'accept call' into socket OVER!");
}

extern "C"
int hangupConnection( int index ){
    ALOGI("hangupConnection====================");
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return -1;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_HANGUP;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    p.writeInt32(1);//count
    p.writeInt32(index);//index

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }

    ALOGI("write 'hangup' into socket OVER!");
    return 0;
}
/****/
/*
 *@param:
 *  protocol: ip protocol typ, value is One of the values "IP","IPV6","IPV4V6"
 *  authType: auth type, One of the SETUP_DATA_AUTH_TYPE values
*/
//apn : 3gnet ,  protocol : ipv4v6,  authType : 0,  user/passward : "";
extern "C"
void setInitialAttachApn(const char* apn, const char* protocol, int authType, const char* username, const char* password){
    int ret;
    Parcel p;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_SET_INITIAL_ATTACH_APN;
    rnode->isFree = false;

    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token
    writeStringToParcel(p, apn);
    writeStringToParcel(p, protocol);
    p.writeInt32(authType);
    writeStringToParcel(p, username);
    writeStringToParcel(p, password);

    ALOGI("setInitialAttachApn apn=%s,  protocol=%s, authType=%d, username=%s, password=%s.",
                                            apn, protocol, authType, username, password);

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }

    ALOGI("set initial attach apn OVER!");
}

//Add by wangcong for Gprs
extern "C"
void sendAtCmd(char *cmd_str){
    int ret;
    requestList *rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_SEND_AT;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);
    writeStringToParcel(p, cmd_str);
    ALOGI("send at cmd: %s", cmd_str);
    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if(ret < 0){
        ALOGE("send at error!");
    }else{
        ALOGI("send at cmd success!");
    }
}

extern "C"
void getGprsAttachState(){
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_GET_GPRS_ATTACH_STATE;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);
    ALOGI("send at cmd success! %d",ret);
    if (ret < 0) {
        ALOGE("send at ERROR!");
    }
     //Add by wangcong
    /*struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_api_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 5;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_api_cond, &s_network_api_mutex, &outtime);
    pthread_mutex_unlock(&s_network_api_mutex);
    ALOGI("write 'get gprs attach state' into socket OVER!");*/
}

//Add by wangcong for PDP
extern "C"
void hanldeGetDataCallList(Parcel &p){
    int ver;
    int num;
    RIL_Data_Call_Response_v11 *datalist;

    p.readInt32(&ver);
    p.readInt32(&num);
    ALOGI("%s: num = %d", __FUNCTION__, num);

    datalist = (RIL_Data_Call_Response_v11 *)malloc(sizeof(RIL_Data_Call_Response_v11) * num);
    if(datalist == NULL){
        ALOGE("malloc mem failed.");
        return;
    }

    for(int i = 0; i < num; i++){
        p.readInt32(&(datalist[i].status));
        p.readInt32(&(datalist[i].suggestedRetryTime));
        p.readInt32(&(datalist[i].cid));
        p.readInt32(&(datalist[i].active));

        datalist[i].type = strdupReadString(p);
        datalist[i].ifname = strdupReadString(p);
        datalist[i].addresses = strdupReadString(p);
        datalist[i].dnses = strdupReadString(p);
        datalist[i].gateways = strdupReadString(p);
        datalist[i].pcscf = strdupReadString(p);

        p.readInt32(&(datalist[i].mtu));

        ALOGI("%s: status = %d", __FUNCTION__, datalist[i].status);
        ALOGI("%s: cid = %d", __FUNCTION__, datalist[i].cid);
        ALOGI("%s: active = %d", __FUNCTION__, datalist[i].active);

        ALOGI("%s: type = %s", __FUNCTION__, datalist[i].type);
        ALOGI("%s: ifname = %s", __FUNCTION__, datalist[i].ifname);
        ALOGI("%s: addr = %s", __FUNCTION__, datalist[i].addresses);
        ALOGI("%s: dns = %s", __FUNCTION__, datalist[i].dnses);
    }

    num_pdp=num;
    //pdplist=datalist;

    for(int i=0;i<min(MAX_PDP,num);i++){
        memset(&pdplist[i], 0x0, sizeof(pdplist[i]) );
        pdplist[i].status = datalist[i].status;
        pdplist[i].cid = datalist[i].cid;
        pdplist[i].active = datalist[i].active;
        memcpy(pdplist[i].type, datalist[i].type, strlen(datalist[i].type) + 1 );
        memcpy(pdplist[i].ifname, datalist[i].ifname, strlen(datalist[i].ifname) + 1 );
        memcpy(pdplist[i].ip, datalist[i].addresses, strlen(datalist[i].addresses) + 1 );

        std::string dnses;
        dnses = datalist[i].dnses;
        char* dns1;
        dns1 = (char*)(dnses.substr( 0, dnses.find(" ") )).data();
        memcpy(pdplist[i].dns1addr, dns1, strlen(dns1) + 1 );
        memcpy(pdplist[i].dns2addr, dns1, strlen(dns1) + 1 );
    }

    pthread_mutex_lock(&s_get_data_call_list_mutex);
    pthread_cond_signal(&s_get_data_call_list_cond);
    pthread_mutex_unlock(&s_get_data_call_list_mutex);

    //finally free mem
    for(int i = 0; i < num; i++){
        if(datalist[i].type != NULL){
            free(datalist[i].type);
        }
        if(datalist[i].ifname != NULL){
            free(datalist[i].ifname);
        }
        if(datalist[i].addresses != NULL){
            free(datalist[i].addresses);
        }
        if(datalist[i].dnses != NULL){
            free(datalist[i].dnses);
        }
        if(datalist[i].gateways != NULL){
            free(datalist[i].gateways);
        }
        if(datalist[i].pcscf != NULL){
            free(datalist[i].pcscf);
        }
    }
    free(datalist);
}

// get data call list
void getDataCallList() {

    int ret;
    Parcel p;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_DATA_CALL_LIST;
    rnode->isFree = false;

    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);

    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if(ret < 0){
        ALOGE("send at ERROR!");
    }

    ALOGI("write 'get data call list' into socket over!");
}

extern "C"
void GetModuleManufacture(char* manuFacture){
    char temp_value[PROPERTY_VALUE_MAX] = {0};
    property_get("ro.product.manufacturer", temp_value, "not_find");
    strcpy(manuFacture,temp_value);
}
extern "C"
int getPdpState(void){

    getDataCallList();
     //Add by wangcong
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_get_data_call_list_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 1;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_get_data_call_list_cond, &s_get_data_call_list_mutex, &outtime);
    pthread_mutex_unlock(&s_get_data_call_list_mutex);
    if(red == ETIMEDOUT)
        return -100;
    /*pthread_mutex_lock(&s_get_data_call_list_mutex);//Modifeid by wangcong in 5.27
    pthread_cond_wait(&s_get_data_call_list_cond, &s_get_data_call_list_mutex);
    pthread_mutex_unlock(&s_get_data_call_list_mutex);*/

      for(int i = 0; i < num_pdp; i++){
        std::string s = pdplist[i].ip;
        if(pdplist[i].active==PDP_STATUS_ACTIVE && !s.empty()){
         
            ALOGI(" pdp active !");
            pdpState = 1;
            return 1;
      
        }
    }
    ALOGI("there is no active pdp!");
    pdpState = 0;
    return 0;
}
//end
//Add by wangcong for PDP
extern "C"
int getPdptruestate(){
    ALOGI("getPdptruestate: %d",pdpState);
    if(pdpState == 1){
    return pdpState;
    }
    return getPdpState();
}
//end
//Add by wangcong for AttachGprs
extern "C"
int attachGprs(){
   int ret;
   sleep(5);
   getGprsAttachState();
   sleep(5);
   ALOGI("test0===getPdpState: %d", getPdpState());
    while(s_gprs_attach_state == false){
        continue;
    }
    //Add by wangcong for PDP
    //test get pdp state
    ALOGI("test1===getPdpState: %d", getPdpState());
    sendAtCmd("AT+CGATT=1");
    sleep(10);
    ALOGI("test1===getPdpState: %d", getPdpState());
    if(getPdpState() == 1)
    sleep(5);
       return 0;
       return 1;
}

//Add by wangcong for detachGprs
extern "C"
int detachGprs(){
   int ret;
   sleep(5);
   getGprsAttachState();
   sleep(5);
   ALOGI("test4===getPdpState: %d", getPdpState());
    while(s_gprs_attach_state == true){
        continue;
    }
    //Add by wangcong for PDP
    //test get pdp state
    ALOGI("test5===getPdpState: %d", getPdpState());
    sendAtCmd("AT+CGATT=0");
    sleep(10);
    ALOGI("test6===getPdpState: %d", getPdpState());
    if(getPdpState() == 0)
    sleep(5);
       return 0;
       return 1;
}
//Add by wangcong for ICCID
extern "C"
void iccIO(int command, int fileid, char* path, int p1, int p2, int p3, char* data, char* pin2){
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_SIM_IO;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    p.writeInt32(command);
    p.writeInt32(fileid);
    writeStringToParcel(p, path);
    p.writeInt32(p1);
    p.writeInt32(p2);
    p.writeInt32(p3);
    writeStringToParcel(p, data);
    writeStringToParcel(p, pin2);
    writeStringToParcel(p, NULL);

    ALOGI("iccIO: command: %#x, fileid: %#x, path: %s, p1: %d, p2:%d, p3:%d.",
            command, fileid, path, p1, p2, p3);
    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);

    if (ret < 0) {
        ALOGE("send at ERROR!");
    }

    ALOGI("write 'get iccIO' into socket OVER!");
}

char* getEFPath(int efid){
    switch(efid){
        case EF_ICCID:
            return MF_SIM;
        default:
            return NULL;
    }
}

extern "C"
int getSimIccId(char* sw0){
    if(strcmp(SimResponse,"0") > 0){
     ALOGI("The last time SimResponse is %s",SimResponse); 
     strcpy(sw0,SimResponse);
     return 0;
    }
    EF_COMMAND_TYPE command = COMMAND_GET_RESPONSE;
    EF_FILE_ID fileid = EF_ICCID;
    s_cur_get_fileid = fileid;

    iccIO(command, fileid, getEFPath(fileid), 0, 0, GET_RESPONSE_EF_SIZE_BYTES,NULL, NULL);
    //Add by wangcong
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_api_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 2;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_api_cond, &s_network_api_mutex, &outtime);
    pthread_mutex_unlock(&s_network_api_mutex);
    if(red == ETIMEDOUT)
        return -100;
    strcpy(sw0,SimResponse);
    return 0;
}
//end

static void hanldeDataCallListChangeMessage(Parcel &p){
    int ver;
    int num;
    RIL_Data_Call_Response_v11 *datalist;

    p.readInt32(&ver);
    p.readInt32(&num);
    ALOGI("num = %d", num);

    datalist = (RIL_Data_Call_Response_v11 *)malloc(sizeof(RIL_Data_Call_Response_v11) * num);
    if(datalist == NULL){
        ALOGE("malloc mem failed.");
        return;
    }

    for(int i = 0; i < num; i++){
        p.readInt32(&(datalist[i].status));
        p.readInt32(&(datalist[i].suggestedRetryTime));
        p.readInt32(&(datalist[i].cid));
        p.readInt32(&(datalist[i].active));

        datalist[i].type = strdupReadString(p);
        datalist[i].ifname = strdupReadString(p);
        datalist[i].addresses = strdupReadString(p);
        datalist[i].dnses = strdupReadString(p);
        datalist[i].gateways = strdupReadString(p);
        datalist[i].pcscf = strdupReadString(p);

        p.readInt32(&(datalist[i].mtu));

        ALOGI("status = %d", datalist[i].status);
        ALOGI("cid = %d", datalist[i].cid);
        ALOGI("active = %d", datalist[i].active);

        ALOGI("type = %s", datalist[i].type);
        ALOGI("ifname = %s", datalist[i].ifname);
        ALOGI("addr = %s", datalist[i].addresses);
        ALOGI("dns = %s", datalist[i].dnses);
        
    }
    if(datalist[s_active_pdp_cid - 1].active == PDP_STATUS_ACTIVE)//Add by wangcong in 05301544
    {
      ALOGI("need retry setup data call");
      pdpState = 1;
    }
    if(datalist[s_active_pdp_cid - 1].active == PDP_STATUS_DEACTIVE){
        ALOGI("need retry setup data call");
        pdpState = 0;
        DataCallConconnect("16", "0", "3gnet", "", NULL, "0", "IPV4V6");
    }

    //finally free mem
    for(int i = 0; i < num; i++){
        if(datalist[i].type != NULL){
            free(datalist[i].type);
        }
        if(datalist[i].ifname != NULL){
            free(datalist[i].ifname);
        }
        if(datalist[i].addresses != NULL){
            free(datalist[i].addresses);
        }
        if(datalist[i].dnses != NULL){
            free(datalist[i].dnses);
        }
        if(datalist[i].gateways != NULL){
            free(datalist[i].gateways);
        }
        if(datalist[i].pcscf != NULL){
            free(datalist[i].pcscf);
        }
    }
    free(datalist);
}

//Add by wangcong for ModuleManufacter
//void GetModuleManufacter(){
//     String ret;
//     ret = property_get("ro.product.manufacturer", temp_value, "not_find");
//}

static void handlesetupDataCallImplComplete(Parcel &p){
    int ver;
    int num;
    RIL_Data_Call_Response_v11 *datalist;

    p.readInt32(&ver);
    p.readInt32(&num);
    ALOGI("num = %d", num);

    datalist = (RIL_Data_Call_Response_v11 *)malloc(sizeof(RIL_Data_Call_Response_v11) * num);
    if(datalist == NULL){
        ALOGE("malloc mem failed.");
        return;
    }

    for(int i = 0; i < num; i++){
        p.readInt32(&(datalist[i].status));
        p.readInt32(&(datalist[i].suggestedRetryTime));
        p.readInt32(&(datalist[i].cid));
        p.readInt32(&(datalist[i].active));

        if(datalist[i].active == PDP_STATUS_ACTIVE){
            s_active_pdp_cid = datalist[i].cid;
        }

        datalist[i].type = strdupReadString(p);
        datalist[i].ifname = strdupReadString(p);
        datalist[i].addresses = strdupReadString(p);
        datalist[i].dnses = strdupReadString(p);
        datalist[i].gateways = strdupReadString(p);
        datalist[i].pcscf = strdupReadString(p);

        p.readInt32(&(datalist[i].mtu));

        ALOGI("status = %d", datalist[i].status);
        ALOGI("cid = %d", datalist[i].cid);
        ALOGI("active = %d", datalist[i].active);

        ALOGI("type = %s", datalist[i].type);
        ALOGI("ifname = %s", datalist[i].ifname);
        ALOGI("addr = %s", datalist[i].addresses);
        ALOGI("dns = %s", datalist[i].dnses);
        if(datalist[i].status == 0 && datalist[i].cid && datalist[i].active && datalist[i].type && datalist[i].ifname && datalist[i].addresses && datalist[i].dnses)
         {
           pdpFlag = 0;
         }else{
           pdpFlag = 1;
     }
    }

    updatePdpInfo(&datalist[0]);

    //finally free mem
    for(int i = 0; i < num; i++){
        if(datalist[i].type != NULL){
            free(datalist[i].type);
        }
        if(datalist[i].ifname != NULL){
            free(datalist[i].ifname);
        }
        if(datalist[i].addresses != NULL){
            free(datalist[i].addresses);
        }
        if(datalist[i].dnses != NULL){
            free(datalist[i].dnses);
        }
        if(datalist[i].gateways != NULL){
            free(datalist[i].gateways);
        }
        if(datalist[i].pcscf != NULL){
            free(datalist[i].pcscf);
        }
    }
    free(datalist);
    pthread_mutex_lock(&s_network_gprs_mutex);
    pthread_cond_signal(&s_network_gprs_cond);
    pthread_mutex_unlock(&s_network_gprs_mutex);
}

int handleGetSignalStrengthComplete(Parcel &p){
    int ver;
    int num;
    RIL_SignalStrength_v10 *signalStrength;

    signalStrength = (RIL_SignalStrength_v10 *)malloc( sizeof(RIL_SignalStrength_v10) );
    if(signalStrength == NULL){
        ALOGE("malloc mem failed.");
        return 0;
    }

    p.readInt32( &signalStrength->GW_SignalStrength.signalStrength );
    p.readInt32( &signalStrength->GW_SignalStrength.bitErrorRate );
    p.readInt32( &signalStrength->CDMA_SignalStrength.dbm );
    p.readInt32( &signalStrength->CDMA_SignalStrength.ecio );
    p.readInt32( &signalStrength->EVDO_SignalStrength.dbm );
    p.readInt32( &signalStrength->EVDO_SignalStrength.ecio );
    p.readInt32( &signalStrength->EVDO_SignalStrength.signalNoiseRatio );
    p.readInt32( &signalStrength->LTE_SignalStrength.signalStrength );
    p.readInt32( &signalStrength->LTE_SignalStrength.rsrp );
    p.readInt32( &signalStrength->LTE_SignalStrength.rsrq );
    p.readInt32( &signalStrength->LTE_SignalStrength.rssnr );
    p.readInt32( &signalStrength->LTE_SignalStrength.cqi );
    signalStrength->LTE_SignalStrength.timingAdvance = 2147483647;//INT_MAX
    signalStrength->TD_SCDMA_SignalStrength.rscp = 2147483647;//INT_MAX
    
    ALOGI("GW_SignalStrength.signalStrength = %d", signalStrength->GW_SignalStrength.signalStrength);
    ALOGI("GW_SignalStrength.bitErrorRate = %d", signalStrength->GW_SignalStrength.bitErrorRate);
    ALOGI("CDMA_SignalStrength.dbm = %d", signalStrength->CDMA_SignalStrength.dbm);
    ALOGI("CDMA_SignalStrength.ecio = %d", signalStrength->CDMA_SignalStrength.ecio);
    ALOGI("EVDO_SignalStrength.dbm = %d", signalStrength->EVDO_SignalStrength.dbm );
    ALOGI("EVDO_SignalStrength.ecio = %d", signalStrength->EVDO_SignalStrength.ecio);
    ALOGI("EVDO_SignalStrength.signalNoiseRatio = %d", signalStrength->EVDO_SignalStrength.signalNoiseRatio );

    ALOGI("LTE_SignalStrength.signalStrength = %d", signalStrength->LTE_SignalStrength.signalStrength);
    ALOGI("LTE_SignalStrength.rsrp = %d", signalStrength->LTE_SignalStrength.rsrp);
    ALOGI("LTE_SignalStrength.rsrq = %d", signalStrength->LTE_SignalStrength.rsrq);
    ALOGI("LTE_SignalStrength.rssnr = %d", signalStrength->LTE_SignalStrength.rssnr );
    ALOGI("LTE_SignalStrength.cqi = %d", signalStrength->LTE_SignalStrength.cqi );
    ALOGI("LTE_SignalStrength.timingAdvance = %d", signalStrength->LTE_SignalStrength.timingAdvance );
    ALOGI("TD_SCDMA_SignalStrength.rscp = %d", signalStrength->TD_SCDMA_SignalStrength.rscp );
    if(signalStrength->LTE_SignalStrength.rsrp < 141 && signalStrength->LTE_SignalStrength.rsrp > 0){
    signal_strength = 141 - signalStrength->LTE_SignalStrength.rsrp  ;
    }else{
    signal_strength = signalStrength->LTE_SignalStrength.rsrp;
    }
    ALOGI("signalStrength->GW_SignalStrength.signalStrength: %d ",signalStrength->GW_SignalStrength.signalStrength);
    if(signalStrength->GW_SignalStrength.signalStrength > 0 && signalStrength->GW_SignalStrength.signalStrength < 50){
    signal_strength = signalStrength->GW_SignalStrength.signalStrength;
    }
    ALOGI("signal_strength: %d ",signal_strength);
    free(signalStrength);

    pthread_mutex_lock(&s_network_signal_mutex);
    pthread_cond_signal(&s_network_signal_cond);
    pthread_mutex_unlock(&s_network_signal_mutex);
    return 0;
}

//Add by wangcong for Gprs
static void handleGetCellInfoListComplete(Parcel &p){
    int num;
    RIL_CellInfo *cellInfo;

    p.readInt32(&num);
    ALOGI("num = %d", num);

    cellInfo = (RIL_CellInfo *)malloc( sizeof(RIL_CellInfo) * num );
    if(cellInfo == NULL){
        ALOGE("malloc mem failed.");
        return;
    }


    for (int i = 0; i < num; ++i){
        int value;

        int64_t timeStamp;//<1>

        p.readInt32( &value );
        cellInfo[i].cellInfoType = (RIL_CellInfoType)value;
        p.readInt32( &cellInfo[i].registered );
        p.readInt32( &value );
        cellInfo[i].timeStampType = (RIL_TimeStampType)value;
        p.readInt64( &timeStamp );//<2>
        cellInfo[i].timeStamp = (uint64_t)timeStamp;// <3>
        ALOGI("cellInfoType: %d, registered:%d, timeStampType:%d, timeStamp:%lld.",// <4>
                cellInfo[i].cellInfoType, cellInfo[i].registered, cellInfo[i].timeStampType, cellInfo[i].timeStamp );
        switch( cellInfo[i].cellInfoType ){
            case RIL_CELL_INFO_TYPE_GSM:
            {
                p.readInt32( &(cellInfo[i].CellInfo.gsm.cellIdentityGsm.mcc) );
                p.readInt32( &(cellInfo[i].CellInfo.gsm.cellIdentityGsm.mnc) );
                p.readInt32( &(cellInfo[i].CellInfo.gsm.cellIdentityGsm.lac) );
                p.readInt32( &(cellInfo[i].CellInfo.gsm.cellIdentityGsm.cid) );
                p.readInt32( &(cellInfo[i].CellInfo.gsm.signalStrengthGsm.signalStrength) );
                p.readInt32( &(cellInfo[i].CellInfo.gsm.signalStrengthGsm.bitErrorRate) );

                ALOGI("mcc:%d, mnc:%d, lac:%d, cid:%d, signalStrength:%d, bitErrorRate:%d.",
                        cellInfo[i].CellInfo.gsm.cellIdentityGsm.mcc,
                        cellInfo[i].CellInfo.gsm.cellIdentityGsm.mnc,
                        cellInfo[i].CellInfo.gsm.cellIdentityGsm.lac,
                        cellInfo[i].CellInfo.gsm.cellIdentityGsm.cid,
                        cellInfo[i].CellInfo.gsm.signalStrengthGsm.signalStrength,
                        cellInfo[i].CellInfo.gsm.signalStrengthGsm.bitErrorRate
                    );
                break;
            }

            case RIL_CELL_INFO_TYPE_WCDMA:
            {
                p.readInt32( &(cellInfo[i].CellInfo.wcdma.cellIdentityWcdma.mcc) );
                p.readInt32( &(cellInfo[i].CellInfo.wcdma.cellIdentityWcdma.mnc) );
                p.readInt32( &(cellInfo[i].CellInfo.wcdma.cellIdentityWcdma.lac) );
                p.readInt32( &(cellInfo[i].CellInfo.wcdma.cellIdentityWcdma.cid) );
                p.readInt32( &(cellInfo[i].CellInfo.wcdma.cellIdentityWcdma.psc) );
                p.readInt32( &(cellInfo[i].CellInfo.wcdma.signalStrengthWcdma.signalStrength) );
                p.readInt32( &(cellInfo[i].CellInfo.wcdma.signalStrengthWcdma.bitErrorRate) );

                ALOGI( "mcc:%d, mnc:%d, lac:%d, cid:%d, psc:%d, signalStrength:%d, bitErrorRate:%d.",
                    cellInfo[i].CellInfo.wcdma.cellIdentityWcdma.mcc,
                    cellInfo[i].CellInfo.wcdma.cellIdentityWcdma.mnc,
                    cellInfo[i].CellInfo.wcdma.cellIdentityWcdma.lac,
                    cellInfo[i].CellInfo.wcdma.cellIdentityWcdma.cid,
                    cellInfo[i].CellInfo.wcdma.cellIdentityWcdma.psc,
                    cellInfo[i].CellInfo.wcdma.signalStrengthWcdma.signalStrength,
                    cellInfo[i].CellInfo.wcdma.signalStrengthWcdma.bitErrorRate
                     );
                break;
            }

            case RIL_CELL_INFO_TYPE_CDMA:
            {
                p.readInt32( &(cellInfo[i].CellInfo.cdma.cellIdentityCdma.networkId) );
                p.readInt32( &(cellInfo[i].CellInfo.cdma.cellIdentityCdma.systemId) );
                p.readInt32( &(cellInfo[i].CellInfo.cdma.cellIdentityCdma.basestationId) );
                p.readInt32( &(cellInfo[i].CellInfo.cdma.cellIdentityCdma.longitude) );
                p.readInt32( &(cellInfo[i].CellInfo.cdma.cellIdentityCdma.latitude) );

                ALOGI( "networkId:%d, systemId:%d, basestationId:%d, longitude:%d, latitude:%d.",
                        cellInfo[i].CellInfo.cdma.cellIdentityCdma.networkId,
                        cellInfo[i].CellInfo.cdma.cellIdentityCdma.systemId,
                        cellInfo[i].CellInfo.cdma.cellIdentityCdma.basestationId,
                        cellInfo[i].CellInfo.cdma.cellIdentityCdma.longitude,
                        cellInfo[i].CellInfo.cdma.cellIdentityCdma.latitude
                );

                p.readInt32( &(cellInfo[i].CellInfo.cdma.signalStrengthCdma.dbm) );
                p.readInt32( &(cellInfo[i].CellInfo.cdma.signalStrengthCdma.ecio) );
                p.readInt32( &(cellInfo[i].CellInfo.cdma.signalStrengthEvdo.dbm) );
                p.readInt32( &(cellInfo[i].CellInfo.cdma.signalStrengthEvdo.ecio) );
                p.readInt32( &(cellInfo[i].CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio) );

                ALOGI( "Cdma.dbm:%d, Cdma.ecio:%d, Evdo.dbm:%d, Evdo.ecio:%d, Evdo.signalNoiseRatio:%d.",
                        cellInfo[i].CellInfo.cdma.signalStrengthCdma.dbm,
                        cellInfo[i].CellInfo.cdma.signalStrengthCdma.ecio,
                        cellInfo[i].CellInfo.cdma.signalStrengthEvdo.dbm,
                        cellInfo[i].CellInfo.cdma.signalStrengthEvdo.ecio,
                        cellInfo[i].CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio
                );
                break;
            }

            case RIL_CELL_INFO_TYPE_LTE:
            {
                p.readInt32( &(cellInfo[i].CellInfo.lte.cellIdentityLte.mcc) );
                p.readInt32( &(cellInfo[i].CellInfo.lte.cellIdentityLte.mnc) );
                p.readInt32( &(cellInfo[i].CellInfo.lte.cellIdentityLte.ci) );
                p.readInt32( &(cellInfo[i].CellInfo.lte.cellIdentityLte.pci) );
                p.readInt32( &(cellInfo[i].CellInfo.lte.cellIdentityLte.tac) );

                ALOGI("mcc:%d, mnc:%d, ci:%d, pci:%d, tac:%d.",
                    cellInfo[i].CellInfo.lte.cellIdentityLte.mcc,
                    cellInfo[i].CellInfo.lte.cellIdentityLte.mnc,
                    cellInfo[i].CellInfo.lte.cellIdentityLte.ci,
                    cellInfo[i].CellInfo.lte.cellIdentityLte.pci,
                    cellInfo[i].CellInfo.lte.cellIdentityLte.tac
                    );
                MCC = cellInfo[i].CellInfo.lte.cellIdentityLte.mcc;
                MNC = cellInfo[i].CellInfo.lte.cellIdentityLte.mnc;
                CI = cellInfo[i].CellInfo.lte.cellIdentityLte.ci;
                PCI = cellInfo[i].CellInfo.lte.cellIdentityLte.pci;
                TAC = cellInfo[i].CellInfo.lte.cellIdentityLte.tac;
                ALOGI("MCC:%d, MNC:%d, CI:%d, PCI:%d, TAC:%d.",MCC,MNC,CI,PCI,TAC);
                p.readInt32( &(cellInfo[i].CellInfo.lte.signalStrengthLte.signalStrength) );
                p.readInt32( &(cellInfo[i].CellInfo.lte.signalStrengthLte.rsrp) );
                p.readInt32( &(cellInfo[i].CellInfo.lte.signalStrengthLte.rsrq) );
                p.readInt32( &(cellInfo[i].CellInfo.lte.signalStrengthLte.rssnr) );
                p.readInt32( &(cellInfo[i].CellInfo.lte.signalStrengthLte.cqi) );
                p.readInt32( &(cellInfo[i].CellInfo.lte.signalStrengthLte.timingAdvance) );

                ALOGI("signalStrength:%d, rsrp:%d, rsrq:%d, rssnr:%d, cqi:%d, timingAdvance:%d.",
                    cellInfo[i].CellInfo.lte.signalStrengthLte.signalStrength,
                    cellInfo[i].CellInfo.lte.signalStrengthLte.rsrp,
                    cellInfo[i].CellInfo.lte.signalStrengthLte.rsrq,
                    cellInfo[i].CellInfo.lte.signalStrengthLte.rssnr,
                    cellInfo[i].CellInfo.lte.signalStrengthLte.cqi,
                    cellInfo[i].CellInfo.lte.signalStrengthLte.timingAdvance
                    );
                if(cellInfo[i].CellInfo.lte.signalStrengthLte.signalStrength && cellInfo[i].CellInfo.lte.signalStrengthLte.rsrp
                && cellInfo[i].CellInfo.lte.signalStrengthLte.rsrq && cellInfo[i].CellInfo.lte.signalStrengthLte.rssnr &&
                cellInfo[i].CellInfo.lte.signalStrengthLte.cqi && cellInfo[i].CellInfo.lte.signalStrengthLte.timingAdvance)
                cellFlag = 0;
                break;
            }

            case RIL_CELL_INFO_TYPE_TD_SCDMA:
            {
                p.readInt32( &(cellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.mcc) );
                p.readInt32( &(cellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.mnc) );
                p.readInt32( &(cellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.lac) );
                p.readInt32( &(cellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.cid) );
                p.readInt32( &(cellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.cpid) );
                p.readInt32( &(cellInfo[i].CellInfo.tdscdma.signalStrengthTdscdma.rscp) );

                ALOGI( "mcc:%d, mnc:%d, lac:%d, cid:%d, cpid:%d, rscp:%d.",
                    cellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.mcc,
                    cellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.mnc,
                    cellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.lac,
                    cellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.cid,
                    cellInfo[i].CellInfo.tdscdma.cellIdentityTdscdma.cpid,
                    cellInfo[i].CellInfo.tdscdma.signalStrengthTdscdma.rscp
                );
                break;
            }

        }//end switch
    }//end for

    free(cellInfo);
    pthread_mutex_lock(&s_network_cell_mutex);
    pthread_cond_signal(&s_network_cell_cond);
    pthread_mutex_unlock(&s_network_cell_mutex);
}

//Add by wangcong for Dial
static void handleGetCurrentCallList(Parcel &p){
    int num;

    int i;
    RIL_Call *p_calls;

    p.readInt32(&num);
    ALOGI("num = %d", num);


    p_calls = (RIL_Call *)malloc(num * sizeof(RIL_Call));
    memset (p_calls, 0, num * sizeof(RIL_Call));


    //+CLCC: <id1>, <dir>, <stat>, <mode>, <mpty>, <number>, <type>
    //+CLCC: 1,0,2,0,0,\"+18005551212\",145
    //+CLCC: 1, 0, 2, 2, 0, “13888888888”, 129
    //      index,isMT,state,mode,isMpty(,number,TOA)?

    for (i = 0; i < num; ++i){
        int value;
        int unused;

        p.readInt32(&value);
        p_calls[i].state = (RIL_CallState)value;
        p.readInt32(&p_calls[i].index);
        p.readInt32(&p_calls[i].toa);
        p.readInt32(&p_calls[i].isMpty);
        p.readInt32(&p_calls[i].isMT);
        p.readInt32(&p_calls[i].als);
        p.readInt32(&p_calls[i].isVoice);
        p.readInt32(&p_calls[i].isVoicePrivacy);

        p_calls[i].number = strdupReadString(p);

        p.readInt32(&p_calls[i].numberPresentation);
        p_calls[i].name = strdupReadString(p);
        p.readInt32(&p_calls[i].namePresentation);
        p.readInt32(&unused);
        p_calls[i].uusInfo = NULL; /* UUS Information is absent */

        ALOGI("state: %d, index:%d, toa:%d, isMpty:%d, isMT:%d, als:%d, isVoice:%d, isVoicePrivacy:%d, number:%s, numberPresentation:%d,name:%s, namePresentation:%d ",
                    p_calls[i].state, p_calls[i].index, p_calls[i].toa, p_calls[i].isMpty, p_calls[i].isMT, p_calls[i].als,
                    p_calls[i].isVoice, p_calls[i].isVoicePrivacy, p_calls[i].number, p_calls[i].numberPresentation,
                    p_calls[i].name, p_calls[i].namePresentation );

    }//end for

    if ( p_calls[0].state == RIL_CALL_INCOMING ){
        ALOGI("incoming call, to accept");
        acceptCall();
    }

    for (i = 0; i < num; ++i){
        if (p_calls[i].number != NULL){
            free(p_calls[i].number);
        }

        if (p_calls[i].name != NULL){
            free(p_calls[i].name);
        }
    }

    if (p_calls != NULL){
        free(p_calls);
    }


}
//End

//Add bu wangcong for CellInfo
extern "C"
void getCellInfo(){
    int ret;
    //AT+SPFREQSCAN=3,\"\",\"\",2,\"\",\"\",3,\"\",\"\"
    //AT^MBCELLID=1
    //AT+SPQ4GNCELLEX
    //AT+SPQ4GNCELLEX=5,6
    //AT+SPNCELLSCAN=16 
    sendAtCmd("AT^MBCELLID=1");
    strcpy(AT,"AT^MBCELLID=1");
    //Add by wangcong
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_cellid_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 10;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_cellid_cond, &s_network_cellid_mutex, &outtime);
    pthread_mutex_unlock(&s_network_cellid_mutex);

}

//Add bu wangcong for CCID
extern "C"
void getIccid(char* CCID){
    int ret;
    sendAtCmd("AT+CCID");
    strcpy(AT,"AT+CCID");
    //Add by wangcong
    struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_api_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 10;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_api_cond, &s_network_api_mutex, &outtime);
    pthread_mutex_unlock(&s_network_api_mutex);
    int a=0;
    char ccid[100];
    sscanf(ICCID, "+CCID: \"%[0-9A-Z]\"\nOK", ccid);
    strcpy(CCID,ccid);

}

//Add by wangcong for ICCID
static void handleGetSimIOComplete(Parcel &p){
    int ver;
    int num;
    RIL_SIM_IO_Response *sim_io_response;

    sim_io_response = (RIL_SIM_IO_Response *)malloc( sizeof(RIL_SIM_IO_Response) );
    if(sim_io_response == NULL){
        ALOGE("malloc mem failed.");
        return;
    }

    p.readInt32(&sim_io_response->sw1);
    p.readInt32(&sim_io_response->sw2);
    sim_io_response->simResponse = strdupReadString(p);
    ALOGI("sw1:%#x, sw2:%#x, simResponse:%s.", sim_io_response->sw1, sim_io_response->sw2, sim_io_response->simResponse );
    if( s_cur_get_fileid == EF_ICCID ){
        ALOGI("iccid string : %s.",  sim_io_response->simResponse );
    }
    if(sim_io_response->simResponse != NULL){
    strcpy(SimResponse,sim_io_response->simResponse);
    ALOGE("SimResponse %s",SimResponse);
    }
    if (sim_io_response->simResponse != NULL) {
        free(sim_io_response->simResponse );
    }
    //Add by  wangcong
    pthread_mutex_lock(&s_network_api_mutex);
    pthread_cond_signal(&s_network_api_cond);
    pthread_mutex_unlock(&s_network_api_mutex);
}
//end

//Add for recall
extern "C"
void setAnswerCallBack(void (*callBackF)()){
      (*callBackF)();
}
//end
static void processUnsolicited(Parcel &p){
    int message_id;
    int i;
    char * urc;
    p.readInt32(&message_id);
    ALOGI("unsolicited message id = %d", message_id);
    switch(message_id){
        case RIL_UNSOL_RIL_CONNECTED:
            int num;
            int *data;

            p.readInt32(&num);
            data = new int[num];

            for(i = 0; i < num; i++){
                p.readInt32(data + i);
                ALOGI("ril version = %d", data[i]);
            }

            delete []data;

            break;

        case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED:  //1000

            int state;

            p.readInt32(&state);
            ALOGI("radio state is %d", state);

            s_radioState = (RIL_RadioState)state;

            pollState(netOwner,netType);

            break;

        case RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED:  //1002
            pollState(netOwner,netType);
            break;

        case RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED:  //1019
             getIccCardStateInner();
             //getIccCardState();

            break;
        case RIL_UNSOL_DATA_CALL_LIST_CHANGED:   //1010
            ALOGI("ril unsolicited data call list changed.==================================");
            hanldeDataCallListChangeMessage(p);
            break;
            
            //Add by wangcong for Dial
        case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED: //1001
            ALOGI("call state changed.==================================");
            //to get Current Calls
            getCurrentCalls();
            break;
            
            //Add by wangcong for cellinfo
        case RIL_UNSOL_URC_STRING:   //6034
            urc = strdupReadString(p);
            ALOGI("ril unsolicited string:%s",urc);
            break;
        default:
        ALOGI("default not handle");
        break;
    }
}

void processSolicited(Parcel &p){
    int request_id;
    int token;
    int err;
    int index;
    int num;
    int value;
    char *responsed[15] = {NULL};
    RIL_RegState regState;

    p.readInt32(&token);
    p.readInt32(&err);

    request_id = findRequestIdBySerial(token);
    ALOGI("solicited request id = %d", request_id);
    if(err == 0){

        switch(request_id){
            case RIL_REQUEST_GET_SIM_STATUS:

                RIL_CardStatus cardstatus;

                p.readInt32(&value);
                cardstatus.card_state = (RIL_CardState)value;
                p.readInt32(&value);
                cardstatus.universal_pin_state = (RIL_PinState)value;

                p.readInt32(&cardstatus.gsm_umts_subscription_app_index);
                p.readInt32(&cardstatus.cdma_subscription_app_index);
                p.readInt32(&cardstatus.ims_subscription_app_index);

                p.readInt32(&cardstatus.num_applications);

                for(index = 0; index < cardstatus.num_applications; ++index){
                    p.readInt32(&value);
                    cardstatus.applications[index].app_type = (RIL_AppType)value;
                    p.readInt32(&value);
                    cardstatus.applications[index].app_state = (RIL_AppState)value;
                    ALOGI("SIM States %d \n", (RIL_AppState)value);
                    if((RIL_AppState)value == 5)
                    {
                    simFlag = 1;
                    }else{
                    simFlag = 0;
                    }
                    p.readInt32(&value);
                    cardstatus.applications[index].perso_substate = (RIL_PersoSubstate)value;

                    cardstatus.applications[index].aid_ptr = strdupReadString(p);
                    cardstatus.applications[index].app_label_ptr = strdupReadString(p);

                    p.readInt32(&cardstatus.applications[index].pin1_replaced);
                    p.readInt32(&value);
                    cardstatus.applications[index].pin1 = (RIL_PinState)value;
                    p.readInt32(&value);
                    ALOGI("SIM States %d \n", (RIL_AppState)value);
                    cardstatus.applications[index].pin2 = (RIL_PinState)value;
                }
                isSimReadyret = isSimReady(cardstatus);
                if(isSimReadyret && !s_radio_power_on){
                    ALOGI("sim ready, and have power on %d", s_radio_power_on);
                    s_radio_power_on = true;
                    radioPower(true);//open protocal stack
               }
                //last free memory avoid mem leak
                for(index = 0; index < cardstatus.num_applications; ++index){
                    if(cardstatus.applications[index].aid_ptr != NULL){
                        free(cardstatus.applications[index].aid_ptr);
                    }
                    if(cardstatus.applications[index].app_label_ptr != NULL){
                        free(cardstatus.applications[index].app_label_ptr);
                    }
                }
				/*ALOGI("outState %d \n",outState);
                if(outState == 1){
					 ALOGI("inner(inner) %d \n", inner);
				     ALOGI("outState(inner) %d \n",outState);
				     pthread_mutex_lock(&s_network_api_mutex);
                     pthread_cond_signal(&s_network_api_cond);
                     pthread_mutex_unlock(&s_network_api_mutex);
                }else {
                        break;
                }*/
                break;
            case RIL_REQUEST_OPERATOR:
                ALOGI("get operator plmn success!");

                p.readInt32(&num);
                ALOGI("num = %d", num);

                for(index = 0; index < num; index++){
                    responsed[index] = strdupReadString(p);
                    ALOGI("num[%d]: %s", index, responsed[index]);
                }
                //if(responsed[0] == 46011){
                //    strcpy(responsed[0],"CHINA TEL");
                // }
                ALOGI("netOwner2 %s\n",responsed[0]);
                if(responsed[0] == NULL){
                 ALOGI("netOwner2 %s\n",responsed[0]);
                }else{
                 ALOGI("netOwner3 %s\n",responsed[0]);
                 strcpy(netOwner,responsed[0]);
                }
                strcpy(numberForNetType,"46011");
                ALOGI("netOwner6 %s\n",numberForNetType);
                if(strcmp(netOwner,numberForNetType) == 0){
                    strcpy(netOwner,"CHINA TEL");
                    ALOGI("netOwner5 %s\n",netOwner);
                }
                ALOGI("netOwner4 %s\n",netOwner);
                //finally free mem
                for(index = 0; index < num; index++){
                    free(responsed[index]);
                }
                /*pthread_mutex_lock(&s_network_api_mutex);
                pthread_cond_signal(&s_network_api_cond);
                pthread_mutex_unlock(&s_network_api_mutex);*/
                break;
            case RIL_REQUEST_RADIO_POWER:
                ALOGI("RADIO POWER ON OR OFF success!");
                break;

            case RIL_REQUEST_SETUP_DATA_CALL:
                ALOGI("setup data call complete!");
                handlesetupDataCallImplComplete(p);
                break;

            case RIL_REQUEST_DATA_REGISTRATION_STATE:
                ALOGI("======================data registration state");

                p.readInt32(&num);
                ALOGI("num = %d", num);

                for(index = 0; index < num; index++){
                    responsed[index] = strdupReadString(p);
                }
                value = atoi(responsed[0]);
                s_reg_state = (RIL_RegState)value;
                ALOGI("net reg status: %d", value);
                netRegStatus = value;
                for(index = 0; index < num; index++){
                    free(responsed[index]);
                }

                //If condition is true, indicates that the current network is roaming.
                if (s_reg_state == RIL_REG_STATE_ROAMING){
                    s_is_roaming = true;
                }
                ALOGI("Is if roaming? %d", s_is_roaming);
                if(s_is_roaming){
                   isRomaing = 1;
                           }else{
                   isRomaing = 0;
                }
                //Current network type
                if(s_reg_state == RIL_REG_STATE_HOME){
                    s_net_type = atoi(responsed[3]);
                    ALOGI("net type: %d", s_net_type);
                    pthread_mutex_lock(&s_network_register_mutex);
                    pthread_cond_signal(&s_network_register_cond);
                    pthread_mutex_unlock(&s_network_register_mutex);
                    ALOGI("already_regist setupdatacall");
                    already_regist = true;
                }
                /*pthread_mutex_lock(&s_network_api_mutex);
                pthread_cond_signal(&s_network_api_cond);
                pthread_mutex_unlock(&s_network_api_mutex);*/
                break;

            case RIL_REQUEST_VOICE_REGISTRATION_STATE:
                ALOGI("void registration state");
                p.readInt32(&num);
                ALOGI("num = %d", num);

                for(index = 0; index < num; index++){
                    responsed[index] = strdupReadString(p);
                }
                value = atoi(responsed[0]);
                regState = (RIL_RegState)value;
                ALOGI("net reg status: %d", value);
                ALOGI("regState: %d", regState);
                if(regState == RIL_REG_STATE_UNKNOWN){
                   if(s_net_type == 14){
                       strcpy(netType,"LTE");
                    }else {
                       ALOGI("UNKNOWN: %d", s_net_type);
                       strcpy(netType,"UNKNOW");
                    }
                }
                if(regState == RIL_REG_STATE_HOME){
                    s_net_type = atoi(responsed[3]);
                    ALOGI("net type: %d", s_net_type);
                    ALOGI("s_net_type: %d", s_net_type);
                    if(s_net_type == 14){
                       ALOGI("regState======: %d", regState);
                       strcpy(netType,"LTE");
                    }else if(s_net_type == 2){
                       ALOGI("regState========: %d", regState);
                       strcpy(netType,"EDGE");
                    }else if(s_net_type == 9){
                       ALOGI("regState========: %d", regState);
                       strcpy(netType,"HSPAP");
                    }else if(s_net_type == 11){
                       ALOGI("regState========: %d", regState);
                       strcpy(netType,"HSPAP");
                    }else if(s_net_type == 15){
                       ALOGI("regState========: %d", regState);
                       strcpy(netType,"HSPAP");
                    }
                    ALOGI("netType3: %s \n",netType);
                }
                for(index = 0; index < num; index++){
                    free(responsed[index]);
                }
                if(value == 1){
                    netWorkFlag = 0;
                }
                //pthread_mutex_lock(&s_network_api_mutex);
                //pthread_cond_signal(&s_network_api_cond);
                //pthread_mutex_unlock(&s_network_api_mutex);
                break;
            case RIL_REQUEST_DEACTIVATE_DATA_CALL:
                ALOGI("deactivate data call result");
                pthread_mutex_lock(&s_network_detactive_mutex);
                pthread_cond_signal(&s_network_detactive_cond);
                pthread_mutex_unlock(&s_network_detactive_mutex);
                break;

            case RIL_REQUEST_SIGNAL_STRENGTH:
                ALOGI("get signal strength result");
                handleGetSignalStrengthComplete(p);
                break;
            //Add by wangcong for GPRS
            case RIL_REQUEST_GET_CELL_INFO_LIST:
                ALOGI("get cell info list result");
                handleGetCellInfoListComplete(p);
                break;
            case RIL_REQUEST_GET_GPRS_ATTACH_STATE:
                {
                    int num;
                    int *data;
                    ALOGI("get gprs attach state result");
                    p.readInt32(&num);
                    data = new int[num];

                    for(int i = 0; i < num; i++){
                        p.readInt32(data + i);
                        ALOGI( "attach state = %d", data[i] );
                    }
                    s_gprs_attach_state = data[0];
                    delete []data;
                }
                /*pthread_mutex_lock(&s_network_api_mutex);
                pthread_cond_signal(&s_network_api_cond);
                pthread_mutex_unlock(&s_network_api_mutex);*/
                break;
                //Add by wangcong for Gprs
            case RIL_REQUEST_SEND_AT:
                 {
                     char *resp = NULL;
                     ALOGI("send atcommand complete!");
                     resp = strdupReadString(p);
                     ALOGI( "resp = %s >>><<<  ", resp );
                     if (resp != NULL)
                     {
                     if(strcmp(AT,"AT+CCID") == 0){
                          memset(ICCID,64,0);
                          strcpy(ICCID,resp);
                          ALOGI( "return is : %s >>>=========================================<<<  ", ICCID );
                          free(resp);
                          pthread_mutex_lock(&s_network_api_mutex);
                          pthread_cond_signal(&s_network_api_cond);
                          pthread_mutex_unlock(&s_network_api_mutex);
                      }else if(strcmp(AT,"AT+SPQ4GNCELLEX=4,4") == 0) {
                          ALOGI( "return is : %s >>>=========================================<<<  ", resp );
                          free(resp);
                      }else if(strcmp(AT,"AT^MBCELLID=1") == 0){
                          char cellInfoForNoCard[100];
                          sscanf(resp, "%*[^:]:%[^A-Z]", cellInfoForNoCard);
                          printf("无卡获取的基站信息：%s",cellInfoForNoCard );
                          free(resp);
                          pthread_mutex_lock(&s_network_cellid_mutex);
                          pthread_cond_signal(&s_network_cellid_cond);
                          pthread_mutex_unlock(&s_network_cellid_mutex);
                            }
                     }
                     //test get grps attach
                          getGprsAttachState();
                  }
                    break;
                case RIL_REQUEST_SET_INITIAL_ATTACH_APN:
                    ALOGI("set initial attach apn complete!");
                    break;
                 //Add by wangcong for IMSI
                case RIL_REQUEST_GET_IMSI:
                  {
                      char* value = NULL;
                      ALOGI("get imsi result");
                      value = strdupReadString(p);
                      ALOGI("value is %s.", value);
                      if (value != NULL)
                      strcpy(imsiid,value);
                      free(value);
                                }
                     /* pthread_mutex_lock(&s_network_imsi_mutex);
                      pthread_cond_signal(&s_network_imsi_cond);
                      pthread_mutex_unlock(&s_network_imsi_mutex);*///Modifeid by wangcong in 5.27 
                    break;
                //Add by wangcong for PDP
                case RIL_REQUEST_DATA_CALL_LIST:
                   {
                       ALOGI("get data call list complete!");
                       hanldeGetDataCallList(p);
                    break;
                                }
                 //Add by wnagcong for ICCID
                case RIL_REQUEST_SIM_IO:
                       ALOGI("get iccIO result");
                       handleGetSimIOComplete(p);
                    break;
                //Add by wangcong for IMEI
                case RIL_REQUEST_GET_IMEI:
                   {
                    char* value = NULL;
                    ALOGI("get imei result");
                    value = strdupReadString(p);
                    ALOGI("value is %s.", value);
                    strcpy(IMEI,value);
                    if (value != NULL)
                    free(value);
                   }
                     pthread_mutex_lock(&s_network_imei_mutex);
                     pthread_cond_signal(&s_network_imei_cond);
                     pthread_mutex_unlock(&s_network_imei_mutex);
                   break;
                 /**Add by wangcong for call **/
                case RIL_REQUEST_DIAL:
                    ALOGI("dial call complete");
                    pthread_mutex_lock(&s_network_dial_mutex);
                    pthread_cond_signal(&s_network_dial_cond);
                    pthread_mutex_unlock(&s_network_dial_mutex);
                    break;
                case RIL_REQUEST_ANSWER:
                    ALOGI("answer call complete==================================");
                    //setAnswerCallBack(*callBackF());
                    break;
                case RIL_REQUEST_GET_CURRENT_CALLS:
                    ALOGI("get current calls result==================================");
                    handleGetCurrentCallList(p);
                    pthread_mutex_lock(&s_network_accept_mutex);
                    pthread_cond_signal(&s_network_accept_cond);
                    pthread_mutex_unlock(&s_network_accept_mutex);
                    break;
                case RIL_REQUEST_HANGUP:
                    ALOGI("handup call complete==================================");
                    break;
                            /**call end**/
            default:
                ALOGI("default not handle");
                break;
        }
    }else{
        switch(request_id){
            case RIL_REQUEST_RADIO_POWER:

                ALOGE("error = %d", err);
                break;
            default:
                ALOGE("unhandle error = %d", err);
                break;
        }
    }

}

//Add by wangcong for IMSI
extern "C"
int getIMSI(char* imsi){
    int ret;
    requestList* rnode;

    rnode = findFreeListNode();

    if(rnode == NULL) return 0;

    rnode->serial = s_serial++;
    rnode->requestId = RIL_REQUEST_GET_IMSI;
    rnode->isFree = false;

    Parcel p;
    p.writeInt32(rnode->requestId);
    p.writeInt32(rnode->serial);//token

    p.writeInt32(1);
    writeStringToParcel(p, NULL);
    ret = sendRequest(p.data(), p.dataSize(), s_socket_fd);
    ALOGI("begnning imsi ==========");
    if (ret < 0) {
        ALOGE("send at ERROR!");
    }
    ALOGI("write 'get imsi' into socket OVER!");
    //Add by wangcong
    /*struct timeval now;
    struct timespec outtime;
    pthread_mutex_lock(&s_network_imsi_mutex);
    gettimeofday(&now, NULL);
    outtime.tv_sec = now.tv_sec + 3;
    outtime.tv_nsec = now.tv_usec * 1000;
    int red = pthread_cond_timedwait(&s_network_imsi_cond, &s_network_imsi_mutex, &outtime);
    pthread_mutex_unlock(&s_network_imsi_mutex);
    // printf("=================> %s ?? \n",imsiid);
    if(red == ETIMEDOUT)
        return -100;
    if(imsiid == NULL)
        return -1;*///Modifeid by wangcong for 5.27
    strcpy(imsi,imsiid);
    return 0;
}

extern "C"
void changeNetWork(char* Type){

    ALOGI("persist.radio.ssda.testmodetestmode========"); 
    property_set("persist.radio.ssda.testmode",Type);//
    ALOGI("persist.radio.ssda.testmodetestmode!!!!!!!!!!");
    char testmode[92 + 1];
    ALOGI("persist.radio.ssda.testmodetestmode~~~~~~~");
    property_get("persist.radio.ssda.testmode",testmode, "0");
    ALOGI("persist.radio.ssda.testmodetestmode is %s",testmode); 
    radioPower(0);
    sleep(5); 
    radioPower(1); 
    sleep(5);

}
extern "C"
void* rilReceiver(void* para){

    RecordStream *pRS;
    void *p_record;
    size_t recordlen;
    int retry_times = 0;
    int ret;
    ALOGI("penbox process try to connect rild");
    //try to connect rild
    s_socket_fd = socket_local_client(SOCKET_NAME,
                ANDROID_SOCKET_NAMESPACE_RESERVED, SOCK_STREAM);

    while (s_socket_fd < 0 && retry_times < 10) {
        ALOGI("try again...");
        retry_times++;
        sleep(1);
        s_socket_fd = socket_local_client(SOCKET_NAME,
                ANDROID_SOCKET_NAMESPACE_RESERVED, SOCK_STREAM);
    }

    if(s_socket_fd < 0){
        ALOGE("Failed to connect socket: rild! errno = %s", strerror(errno));
        return NULL;
    }

    ALOGI("connect established sucess!");

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(s_socket_fd, &rfds);

    ret = fcntl(s_socket_fd, F_SETFL, O_NONBLOCK);
    if (ret < 0) {
        ALOGE("Error setting O_NONBLOCK errno:%d", errno);
    }

    pRS = record_stream_new(s_socket_fd, MAX_COMMAND_BYTES);

    for( ; ; ){
        ALOGI("listening ril socket...");
        //listening ril socket fd have data to read.
        if(select(s_socket_fd + 1, &rfds, NULL, NULL, NULL) < 0) {
            ALOGE("select error: %s", strerror(errno));
            if (errno == EINTR) {
                continue;
            } else {
                ALOGE("select error, break");
                break;
            }
        }

        /***if condition statement may omitted because of only listen one socket***/
        if (FD_ISSET(s_socket_fd, &rfds)) {

            while(1){

                ALOGI("read data from ril socket");
                ret = record_stream_get_next(pRS, &p_record, &recordlen);

                if (ret == 0 && p_record == NULL) {
                    ALOGE("end of stream");
                    break;
                } else if (ret < 0) {
                    ALOGI("ret < 0");
                    break;
                } else if (ret == 0) {
                    Parcel p;
                    int isURC;

                    p.setData((uint8_t *) p_record, recordlen);

                    p.readInt32(&isURC);

                    if (isURC == 0) {
                        processSolicited(p);
                    } else {
                        processUnsolicited(p);
                    }
                }
            }

            if(ret == 0 || (ret < 0 && errno != EAGAIN && errno != EINTR)){
                ALOGE("socket occur error! errno: %s", strerror(errno));
                break;
            }
        }
    }

    ALOGI("ril receiver task exit!");

    return NULL;
}

extern "C"
int setupDataCall(){

    ALOGI("%s", __FUNCTION__);
#if defined (APP_PPPDIAL_MODULE)
    initPppDial();
#endif
    ALOGI("already_regist %d", already_regist);
    if(!already_regist)
    {
		//Add by wangcong
		struct timeval now;
		struct timespec outtime;
		pthread_mutex_lock(&s_network_register_mutex);
		gettimeofday(&now, NULL);
		outtime.tv_sec = now.tv_sec + 1;
		outtime.tv_nsec = now.tv_usec * 1000;
		int red = pthread_cond_timedwait(&s_network_register_cond, &s_network_register_mutex, &outtime);
		pthread_mutex_unlock(&s_network_register_mutex);
		if(red == ETIMEDOUT)
			return -100;
       /* pthread_mutex_lock(&s_network_register_mutex);
        pthread_cond_wait(&s_network_register_cond, &s_network_register_mutex);
        pthread_mutex_unlock(&s_network_register_mutex);*/
        ALOGI("already_regist1 %d", already_regist);
    }

    ALOGI("%s: network register ok, ready go...", __FUNCTION__);
    //DataCallConconnect("16", "0", "3gnet", "", NULL, "0", "IPV4V6");
    //sleep(2);
    return 0;
}

extern "C"
int DataCallDisconnect(){
    ALOGI("%s: DataCallDisconnect, cid = %d, active = %d.", __FUNCTION__, pdp_info.cid, pdp_info.active );
    if ( PDP_STATUS_ACTIVE == pdp_info.active){
         deactivateDataCall( pdp_info.cid );
         ALOGI("%s: 1DataCallDisconnect, cid = %d, active = %d.", __FUNCTION__, pdp_info.cid, pdp_info.active );
         return 0;
    }
    return 1;
}

void signal_handler(int unused){
    //deactive pdp context
    ALOGI("%s: catch signal %d", __FUNCTION__, unused);
    DataCallDisconnect();
    exit(0);
}

extern "C"
int open4G()
{
    printf("============open start\n");
    pthread_t tid;
    pthread_attr_t attr;
    int retry_times = 0;
    ALOGI("%s: aipenbox main thread started!!!", __FUNCTION__ );
    initRequestList();
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(pthread_create(&tid, &attr, rilReceiver, NULL) < 0){
        ALOGE("%s: create rilReceiver task failed!!!", __FUNCTION__);
    }
    sleep(3);
    signal( SIGTERM, signal_handler );
    return 0;
}


/*
    getSignalStrength();
    getCellInfoList();
    getGprsAttachState();
    while(s_gprs_attach_state == false){
        continue;
    }
    sendAtCmd("AT+CGATT=0");
    sleep(5);
    sendAtCmd("AT+CGATT=1");
    sleep(5);
    setupDataCallImpl("16", "0", "3gnet", "", NULL, "0", "IPV4V6");
    */
extern "C"
int close4G()
{
    printf("============close \n");
    return 0;
}
