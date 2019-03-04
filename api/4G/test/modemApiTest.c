#include <modemApi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

int main(int argc, char** argv)
{
    int s_gprs_attach_state = -1;
    int retry_times = 0;
    printf("======================1\n");
    open4G();   
    int strengh = getSignalStrength();
    printf("getSignalStrength %d \n",strengh);
    int ICCID = getIccCardState();
    printf("getIccCardState %d\n",ICCID);
    int netWorkFlag = getVoiceNetworkRegistrationState();
    printf("netWorkFlag %d\n",netWorkFlag);
    int dataWorkFlag = getDataNetworkRegistrationState();
    printf("dataWorkFlag %d\n",dataWorkFlag);
    char imsi[32] = {0};
    getIMSI(imsi);
    printf("IMSI %s\n",imsi);
    char sw0[32] = {0};
    getSimIccId(sw0);
    printf("SimResponse %s\n",sw0);
    int pdpFlag = setupDataCall("16", "0", "3gnet", "", NULL, "0", "IPV4V6");
    printf("pdpFlag = %d\n",pdpFlag);
    char manufacture[32] = {0};
    GetModuleManufacture(manufacture);
    printf("manuFacture = %s\n",manufacture);
    int cellinfo = getCellInfoList();
    printf("cellinfo = %d\n",cellinfo);
    char imei[32] = {0};
    getIMEI(imei);
    printf("imei = %s\n",imei);    
    int states = getPdptruestate();
    printf("states = %d\n",states);    
    int deactivePdp = DataCallDisconnect();
    printf("deactivePdp = %d\n",deactivePdp);
    close4G();
    printf("======================3\n");
    return 0;
}
