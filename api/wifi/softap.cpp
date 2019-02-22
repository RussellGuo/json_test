//
// Spreadtrum wifi Tester
//
//
//
#include <pthread.h>
#include <stdlib.h>
#include <netutils/ifc.h>
#include <cutils/properties.h>
#include <hardware_legacy/wifi.h>
#include <string.h>

#include "type.h"
#include "softap.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//--namespace sci_wifi {
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------
// enable or disable local debug
#define DBG_ENABLE_DBGMSG
#define DBG_ENABLE_WRNMSG
#define DBG_ENABLE_ERRMSG
#define DBG_ENABLE_INFMSG
#define DBG_ENABLE_FUNINF
#include "debug.h"
//------------------------------------------------------------------------------
SoftapAPI::SoftapAPI() {

}

int SoftapAPI::softapOpen( void )
{
    FUN_ENTER;
    DBGMSG("........ wifi_load_driver begin ........\n");
    if( wifi_load_driver() != 0 ) {
        ERRMSG("wifi_load_driver fail!\n");
        return -1;
    }
    DBGMSG("........ wifi_load_driver end ........\n");
    system("ndc softap fwreload wlan0 AP");
    system("ndc softap startap");
    /*
    system("ndc interface setcfg wlan0 192.168.43.1 24 running multicast broadcast up");
    system("ndc tether interface add wlan0");
    system("ndc ipfwd enable");
    system("ndc tether start 192.168.42.2 192.168.42.254 192.168.43.2 192.168.43.254 192.168.44.2 192.168.44.254 192.168.45.2 192.168.45.254 192.168.46.2 192.168.46.254 192.168.47.2 192.168.47.254 192.168.48.2 192.168.48.254");
    system("ndc tether dns set 8.8.8.8 8.8.4.4");

    char dns1[PROPERTY_VALUE_MAX] = {'\0'};
    char dns2[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("net.dns1", dns1, NULL);
    property_get("net.dns2", dns2, NULL);
    if(dns1 != NULL && dns2 != NULL && strlen(dns1)>0 && strlen(dns2)>0){
        DBGMSG("........ dns1 = %s, dns2 = %s ........\n", dns1, dns2);
        char cmd[256];
        snprintf(cmd,256,"ndc tether dns set %s %s", dns1, dns2);
        system(cmd);
        system("ndc nat enable wlan0 seth_lte0 2 fe80::/64 192.168.43.0/24");
    }*/
    system("net_mw tether wlan0");
    system("net_mw enable_nat wlan0 seth_lte0");

    DBGMSG("........ wifi_connect_to_hostapd begin ........\n");

    int cnn_num = 5;
    int cnn_ret = -1;
    while( cnn_num-- ) {
        usleep(200 * 1000);

        if( wifi_connect_to_hostapd("wlan0") != 0 ) {
            continue;
        } else {
            cnn_ret = 0;
            break;
        }
    }
    DBGMSG("........ wifi_connect_to_hostapd end ........\n");

    if( 0 != cnn_ret ) {
        ERRMSG("wifi_connect_to_hostapd fail!\n");
        return -3;
    }

    pthread_t      ptid;
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&ptid, &attr, softapEventLoop, NULL);

    FUN_EXIT;
    return 0;
}



void * SoftapAPI::softapEventLoop( void *param )
{
    DBGMSG("---- softapEventLoop enter ----\n");

    #define EVT_MAX_LEN 127
    char evt[EVT_MAX_LEN + 1];
    int len = 0;

    evt[EVT_MAX_LEN] = 0;
    while( true ) {
        evt[0] = 0;
        len = wifi_hostapd_wait_for_event(evt,EVT_MAX_LEN);
        INFMSG("softap event: %s\n", evt);
        if( NULL != strstr(evt, "AP-TERMINATING") ) {
            wifi_close_hostapd_connection("wlan0");
            break;
        }

        if( NULL != strstr(evt, "AP-STA-CONNECTED") ) {
            INFMSG("softap ap CONNECTED");
        }

        if( NULL != strstr(evt, "AP-STA-DISCONNECTED") ) {
            INFMSG("softap ap DISCONNECTED");
        }
    }

    DBGMSG("---- softapEventLoop exit ----\n");
    return NULL;
}


int SoftapAPI::softapClose( void ){
    FUN_ENTER;
    /*system("ndc interface setcfg wlan0 0.0.0.0 0 running multicast broadcast up");

    char dns1[PROPERTY_VALUE_MAX] = {'\0'};
    char dns2[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("net.dns1", dns1, NULL);
    property_get("net.dns2", dns2, NULL);
    if(dns1 != NULL && dns2 != NULL && strlen(dns1)>0 && strlen(dns2)>0){
        system("ndc nat disable wlan0 seth_lte0 2 fe80::/64 2409:8920:b1:8a40::/64");
    }

    system("ndc tether interface remove wlan0");
    system("ndc softap stopap");
    system("ndc tether stop");
    system("ndc softap fwreload wlan0 STA");
    system("ndc ipfwd disable");*/
    system("net_mw disable_nat wlan0 seth_lte0");
    system("net_mw untether wlan0");
    system("ndc softap stopap");
    system("ndc softap fwreload wlan0 STA");

    DBGMSG("........ wifi_stop_connect_to_hostapd ........\n");
    wifi_stop_connect_to_hostapd("wlan0");

    DBGMSG("........ wifi_unload_driver ........\n");
    wifi_unload_driver();
    FUN_EXIT;
    return 0;
}


int SoftapAPI::softapSet(char ssid[], char psk[]){
    FUN_ENTER;
    char cmd[256];
    snprintf(cmd,256,"ndc softap set wlan0 %s broadcast 6 wpa2-psk %s", ssid, psk);
    system(cmd);
    FUN_EXIT;
    return 0;
}

//cmd is similar to {"softap","set","wlan0","softapname","broadcast","6","wpa2-psk","123456789"};
//only wlanIface, SSID, accessPointChannel, securityType and preSharedKey can set according to framework.
int SoftapAPI::softapSetMoreParam(int argc, char *argv[]){
    FUN_ENTER;
    char cmd[256];
    snprintf(cmd,256,"ndc softap set %s %s broadcast %s %s %s", argv[2], argv[3], argv[5], argv[6], argv[7]);
    system(cmd);
    FUN_EXIT;
    return 0;
}