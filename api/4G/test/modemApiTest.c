#include <modemApi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#define TEST_PHONE_CALL 0
#define TEST_PHONE_WITH_DIAL 1


int main(int argc, char** argv)
{
    int s_gprs_attach_state = -1;
    int retry_times = 0;
    open4G();
    //无卡状态获取
    /*int state = getIccCardState();
    printf("getIccCardState %d\n",state);
    char manufacture[32] = {0};
    GetModuleManufacture(manufacture);
    printf("manuFacture = %s\n",manufacture);
    sleep(1);
    char imei[32] = {0};
    getIMEI(imei);
    printf("imei = %s\n",imei);
    int strength = getSignalStrength();
    printf("getSignalStrength %d \n",strength);
    printf("begin getIccCardState\n");
    Cellinfo cellinfoStruct;
    int cellinfo = getCellInfoList( &cellinfoStruct );
    printf("getCellInfoList mcc:%d  mnc:%d ci:%d pci:%d tac:%d \n",
                 cellinfoStruct.mcc,cellinfoStruct.mnc,cellinfoStruct.ci,cellinfoStruct.pci,cellinfoStruct.tac);
    int states = getPdptruestate();
    printf("states = %d\n",states);
   // const char number[12] = "110";
  //  dial(number);
    getCellInfo();*/
    //有卡状态获取
    int state = getIccCardState();
    printf("getIccCardState %d\n",state);
    int pdpFlag = setupDataCall();
    printf("pdpFlag = %d\n",pdpFlag);
#if TEST_PHONE_CALL
    int strength = getSignalStrength();
    //setAnswerCallBack(answerCallBack);
    printf("getSignalStrength %d \n",strength);
#if TEST_PHONE_WITH_DIAL
    const char number[12] = "13236582712";
    dial(number);
#endif
    while(1);
#else
    DataCallConconnect("16", "0", "3gnet", "", NULL, "0", "IPV4V6");
    int strength = getSignalStrength();
    sleep(1);
    getSignalStrength();
    getSignalStrength();
    getSignalStrength();
    getSignalStrength();
    getSignalStrength();
    printf("getSignalStrength %d \n",strength);
    char netTypeWork[32] = {0};
    int netWorkFlag = getVoiceNetworkRegistrationState(netTypeWork);
    printf("netWorkFlag %d\n",netWorkFlag);
    int dataWorkFlag = getDataNetworkRegistrationState();
    printf("dataWorkFlag %d\n",dataWorkFlag);
    printf("begin getIccCardState\n");
    char manufacture[32] = {0};
    GetModuleManufacture(manufacture);
    printf("manuFacture = %s\n",manufacture);
    Cellinfo cellinfoStruct;
    int cellinfo = getCellInfoList( &cellinfoStruct );
    printf("getCellInfoList mcc:%d  mnc:%d ci:%d pci:%d tac:%d \n",
                 cellinfoStruct.mcc,cellinfoStruct.mnc,cellinfoStruct.ci,cellinfoStruct.pci,cellinfoStruct.tac);
    printf("cellinfo = %d\n",cellinfo);
    int state1 = getIccCardState();
    printf("getIccCardState %d\n",state1);
    char imei[32] = {0};
    getIMEI(imei);
    sleep(1);
    getIMEI(imei);
    getIMEI(imei);
    printf("imei = %s\n",imei);
    //int deactivePdp = DataCallDisconnect();
    //printf("deactivePdp = %d\n",deactivePdp);
    int states = getPdptruestate();
    sleep(1);
    printf("states = %d\n",states);
    getPdptruestate();
    getPdptruestate();
    getPdptruestate();
    char sw0[32] = {0};
    getSimIccId(sw0);
    sleep(1);
    getSimIccId(sw0);
    getSimIccId(sw0);
    getSimIccId(sw0);
    printf("sw = %s \n",sw0);
    char netOwner[32] = {0};
    char netType[32] = {0};
    pollState(netOwner,netType);
    printf("netOwner = %s \n",netOwner);
    printf("netType = %s \n",netType);
    char imsi[32] = {0};
    getIMSI(imsi);
    printf("imsi = %s \n",imsi);
    char CCID[64] ={0};
    getIccid(CCID);
    printf("CCID = %s \n",CCID);
    DataCallConconnect("16", "0", "3gnet", "", NULL, "0", "IPV4V6");
    //int states1 = getPdptruestate();
    //printf("states1 = %d\n",states1);
    getCellInfo();
    //6:4G   14:3G   10:2G    
    //char netWorkType[8] = "6";
    //changeNetWork(netWorkType);
#endif
    close4G();
    return 0;
}
