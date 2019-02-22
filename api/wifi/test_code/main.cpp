#include "wifi.h"
#include "softap.h"
#include <string.h>

//------------------------------------------------------------------------------
// enable or disable local debug
#define DBG_ENABLE_DBGMSG
#include "debug.h"
//------------------------------------------------------------------------------


int main( int argc, char *argv[] )
{
    DBGMSG("........ wifitest enter ........\n");
    DBGMSG("argc is %d\n", argc);
    SoftapAPI *softap = new SoftapAPI();
    for(int i = 0; i < argc ; i++){
        DBGMSG("argv[%d] is %s\n", i, argv[i]);
        if(0 == strcmp("wifiOpen", argv[i])){
            DBGMSG("wifiOpen argv");
            wifiOpen();
            usleep(3000 * 1000);
        }else if(0 == strcmp("wifiAsyncScanAP", argv[i])){
            DBGMSG("wifiAsyncScanAP argv");
            wifiAsyncScanAP();
            usleep(5000 * 1000);
        }else if(0 == strcmp("wifiGetAPs", argv[i])){
            DBGMSG("wifiGetAPs argv");
            struct wifi_ap_t aps[10];
            int num = wifiGetAPs(aps, 10);
            if( num > 0 ) {
                for( int i = 0; i < num; ++i ) {
                    DBGMSG("wifiGetAPs %d", i);
                    //DBGMSG("bmac : %s", aps[i].bmac);
                    DBGMSG("mac : %s", aps[i].smac);
                    DBGMSG("name : %s", aps[i].name);
                    DBGMSG("sig_level : %d", aps[i].sig_level);
                }
            }
        }else if(0 == strcmp("wifiAddNetwork", argv[i])){
            DBGMSG("wifiAddNetwork argv");
            char * ssid = "\"wifitest\"";
            char * psk  = "\"12345678\"";
            //the netid is for wifiConnectNetwork
            int netid = wifiAddNetwork(ssid, psk);
            usleep(2000 * 1000);
            DBGMSG("wifiAddNetwork netid : %d ", netid);
        }else if(0 == strcmp("wifiConnectNetwork", argv[i])){
            DBGMSG("wifiConnectNetwork argv");
            char * ssid = "\"wifitest\"";
            char * psk  = "\"12345678\"";
            //the netid is for wifiConnectNetwork
            int netid = wifiAddNetwork(ssid, psk);
            wifiConnectNetwork(netid);
            usleep(20000 * 1000);
        }else if(0 == strcmp("wifiClose", argv[i])){
            DBGMSG("wifiClose argv");
            wifiClose();
            usleep(1000 * 1000);
        }else if(0 == strcmp("wifiDhcp", argv[i])){
            DBGMSG("wifiDhcp argv");
            wifiDhcp();
            usleep(10000 * 1000);
        }else if(0 == strcmp("wifiForgetNetwork", argv[i])){
            DBGMSG("wifiForgetNetwork argv");
            wifiForgetNetwork(0);
            usleep(3000 * 1000);
        }else if(0 == strcmp("wifiGetRssi", argv[i])){
            DBGMSG("wifiGetRssi argv");
            int rssi = wifiGetRssi();
            DBGMSG("rssi : %d", rssi);
        }else if(0 == strcmp("wifiDisconnect", argv[i])){
            DBGMSG("wifiDisconnect argv");
            wifiDisconnect();
        }else if(0 == strcmp("wifiGetCurrentStatus", argv[i])){
            DBGMSG("wifiGetCurrentStatus argv");
            wifiGetCurrentStatus();
        }else if(0 == strcmp("startSoftap", argv[i])){
            DBGMSG("startSoftap argv");
            softap->softapOpen();
            usleep(2000 * 1000);
        }else if(0 == strcmp("stopSoftap", argv[i])){
            DBGMSG("stopSoftap argv");
            softap->softapClose();
            usleep(2000 * 1000);
        }else if(0 == strcmp("setSoftap", argv[i])){
            DBGMSG("setSoftap argv");
            char ssid[] = "wifitestsoftapname";
            char psk[] = "123456789";
            softap->softapSet(ssid, psk);
        }
    }
    DBGMSG("........ wifitest_main exit ........\n");
    return 0;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
