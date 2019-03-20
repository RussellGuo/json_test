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

    printf("begin getIccCardState\n");
    int state = getIccCardState();
    printf("getIccCardState %d\n",state);

    int pdpFlag = setupDataCall();
    printf("pdpFlag = %d\n",pdpFlag);
#if TEST_PHONE_CALL
    int strength = getSignalStrength();
    printf("getSignalStrength %d \n",strength);
#if TEST_PHONE_WITH_DIAL
    const char number[12] = "15951568547";
    dial(number);
#endif
    while(1);
#else
    int strength = getSignalStrength();
    printf("getSignalStrength %d \n",strength);
    char netTypeWork[32] = {0};
    int netWorkFlag = getVoiceNetworkRegistrationState(netTypeWork);
    printf("netWorkFlag %d\n",netWorkFlag);
    int dataWorkFlag = getDataNetworkRegistrationState();
    printf("dataWorkFlag %d\n",dataWorkFlag);
    char imsi[32] = {0};
    getIMSI(imsi);
    printf("IMSI %s\n",imsi);
    char manufacture[32] = {0};
    GetModuleManufacture(manufacture);
    printf("manuFacture = %s\n",manufacture);
    Cellinfo cellinfoStruct;
    int cellinfo = getCellInfoList( &cellinfoStruct );
    printf("==== getCellInfoList mcc:%d  mnc:%d ci:%d pci:%d tac:%d \n",
                 cellinfoStruct.mcc,cellinfoStruct.mnc,cellinfoStruct.ci,cellinfoStruct.pci,cellinfoStruct.tac);

    printf("cellinfo = %d\n",cellinfo);
    char imei[32] = {0};
    getIMEI(imei);
    printf("imei = %s\n",imei);
    int states = getPdptruestate();
    printf("states = %d\n",states);
    int deactivePdp = DataCallDisconnect();
    printf("deactivePdp = %d\n",deactivePdp);
    char netOwner[32] = {0};
    char netType[32] = {0};
    pollState(netOwner,netType);
    printf("netOwner = %s \n",netOwner);
    printf("netType = %s \n",netType);
    char CCID[64] ={0};
    getIccid(CCID);
    printf("CCID = %s \n",CCID);
#endif
    close4G();
    return 0;
}
