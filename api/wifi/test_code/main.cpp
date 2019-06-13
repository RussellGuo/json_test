#include "wifi.h"
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
		    DBGMSG("flags : %s", aps[i].flags);
                    DBGMSG("sig_level : %d", aps[i].sig_level);
                }
            }
        }/*else if(0 == strcmp("wifiAddNetwork", argv[i])){
            DBGMSG("wifiAddNetwork argv");
            char * ssid = "\"huaqin\"";
            char * psk  = "\"12345678\"";
            //the netid is for wifiConnectNetwork
            int netid = wifiAddNetwork(ssid, psk);
            usleep(2000 * 1000);
             DBGMSG("wifiAddNetwork netid : %d ", netid);
	}*/
else if(0 == strcmp("wifiConnectNetwork", argv[i])){
            DBGMSG("wifiConnectNetwork argv");
            char * ssid = "\"huaqin\"";
            char * psk  = "\"12345678\"";
            char ip_mask[15];
            char ip_addr[15];
            char ip_gate[15];
            char mac_addr[19];
            //the netid is for wifiConnectNetwork
            int netid = wifiAddNetwork(ssid, psk);
            wifiConnectNetwork(netid);
            int pstatus = WiFiIsOnline();
           DBGMSG("pstatus : %d", pstatus);
             wifiGetMACAddr(mac_addr, 19);
             DBGMSG("mac_addr : %s\n", mac_addr);
             wifiGetipaddr(ip_addr);
             DBGMSG("ip_addr : %s\n", ip_addr);
             WiFiGetGATE(ip_gate);
             DBGMSG("ip_gate : %s\n", ip_gate);
             WiFiGetipMask(ip_mask);
             DBGMSG("ip_mask : %s\n", ip_mask);
             usleep(20000 * 1000);
        }
	else if(0 == strcmp("wifiAddNetOpenwork", argv[i])){
            DBGMSG("wifiAddNetwork argv");
 	    char ip_addr[15];
            char ip_gate[15];
            char mac_addr[19];
            char * ssid = "\"华勤open\"";
            //the netid is for wifiConnectNetwork
            int netid = wifiAddOpenNetwork(ssid);
            usleep(2000 * 1000);
            DBGMSG("wifiAddNetwork netid : %d ", netid);
        }

	else if(0 == strcmp("wifiConnectOpenNetwork", argv[i])){
            DBGMSG("wifiAddNetwork argv");
            char ip_mask[15];
 	    char ip_addr[15];
            char ip_gate[15];
            char mac_addr[19];
            char * ssid = "\"华勤open\"";
            //the netid is for wifiConnectNetwork
            int netid = wifiAddOpenNetwork(ssid);
            usleep(2000 * 1000);
            DBGMSG("wifiAddNetwork netid : %d ", netid);
	    wifiConnectNetwork(netid);
             wifiGetMACAddr(mac_addr, 19);
             DBGMSG("mac_addr : %s\n", mac_addr);
             wifiGetipaddr(ip_addr);
             DBGMSG("ip_add : %s\n", ip_addr);
             WiFiGetGATE(ip_gate);
             DBGMSG("ip_gate : %s\n", ip_gate);
             WiFiGetipMask(ip_mask);
             DBGMSG("ip_mask : %s\n", ip_mask);
        }else if(0 == strcmp("wifiConnectNetwork", argv[i])){
            DBGMSG("wifiConnectNetwork argv");
            char * ssid = "\"VIP-guest\"";
            char * psk  = "\"huaqin2018\"";
            char ip_mask[15];
            char ip_addr[15];
            char ip_gate[15];
            char mac_addr[19];       
            //the netid is for wifiConnectNetwork
            int netid = wifiAddNetwork(ssid, psk);
             wifiConnectNetwork(netid);
             wifiGetMACAddr(mac_addr, 19);
             DBGMSG("mac_addr : %s\n", mac_addr);
             wifiGetipaddr(ip_addr);
             DBGMSG("ip_add : %s\n", ip_addr);
             WiFiGetGATE(ip_gate);
	     DBGMSG("ip_add : %s\n", ip_addr);
             DBGMSG("ip_gate : %s\n", ip_gate);
             WiFiGetipMask(ip_mask);
             DBGMSG("ip_mask : %s\n", ip_mask);
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
            softapOpen();
            usleep(2000 * 1000);
        }else if(0 == strcmp("stopSoftap", argv[i])){
            DBGMSG("stopSoftap argv");
            softapClose();
            usleep(2000 * 1000);
        }else if(0 == strcmp("setSoftap", argv[i])){
            DBGMSG("setSoftap argv");
            char ssid[] = "wifitestsoftapname";
            char psk[] = "123456789";
            softapSet(ssid, psk);
        }
    }
    DBGMSG("........ wifitest_main exit ........\n");
    return 0;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

