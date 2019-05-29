//
// Spreadtrum wifi Tester
//
//
//
#include <pthread.h>
#include <stdlib.h>

#include <cutils/properties.h>
#include <hardware_legacy/wifi.h>
#include <netutils/dhcp.h>
#include <netutils/ifc.h>
#include <string.h>

#include "type.h"
#include "wifi.h"

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <cutils/sockets.h>
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


#define WIFI_CMD_RETRY_NUM          3
#define WIFI_CMD_RETRY_TIME         1 // second
#define WIFI_MAX_AP                20
#define PROP_VALUE_MAX             92
#define PROPERTY_VALUE_MAX  PROP_VALUE_MAX
#define DISABLED_UNKNOWN_REASON     0
#define DISABLED_DNS_FAILURE        1
#define DISABLED_DHCP_FAILURE       2
#define DISABLED_AUTH_FAILURE       3
#define DISABLED_ASSOCIATION_REJECT 4
#define CONNECTED                   5
#define CONNECTING                  6

#define GB_FLAG 0x85
#define PATH "wpa_wlan0"

struct sockaddr_un local;
static int counter;

//------------------------------------------------------------------------------
static wifi_ap_t       sAPs[WIFI_MAX_AP];
static int             sStatus       = 0;
static int             sConnectStatus= 0;
static int             sAPNum        = 0;
static pthread_mutex_t sMutxEvent    = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  sCondEvent    = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t sMutxDhcp     = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  sCondDhcp     = PTHREAD_COND_INITIALIZER;
static volatile int    sEventLooping = 0;
static char mInterfaceCmd[64];
//------------------------------------------------------------------------------
static void * wifiEventLoop( void *param );
static void * wifiScanThread( void *param );
static int    wifiCommand(const char * cmder, char * replyBuf, int replySize);
static void   wifiParseLine(const char * begin, const char * end, struct wifi_ap_t * ap);
static int    wifiGetScanResult(struct wifi_ap_t * aps, int maxnum);
static int    wifiSaveConfig();
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//wg for ipaddr api
char ip_addr[PROPERTY_VALUE_MAX]="UNKNOWN";
char ip_gate[PROPERTY_VALUE_MAX]="UNKNOWN";
char ip_mask[PROPERTY_VALUE_MAX]="UNKNOWN";

typedef unsigned short u16;
typedef unsigned char u8;

void * os_malloc(size_t size)
{
	return malloc(size);
}

/**
* Check if the string need to do a gbk to utf-8 or utf-8 to gbk convertion
* str: input string
* str_len : input string byte length
* return 1 for yes
* return 0 for no
*/
static int needConvert(const char* str, unsigned str_len)
{
	unsigned i = 0;
	int bNeed_convert = 0;

	if(NULL == str || 0 == str_len)
		return 0; // see as no

	while(i < str_len)
	{
		if(0 == (str[i] & 0x80)) //is ascii
		{
			i++;
			continue;
		}

		bNeed_convert = 1;
		break;
	}
	return bNeed_convert;
}


static int hex2num(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}


int hex2byte(const char *hex)
{
	int a, b;
	a = hex2num(*hex++);
	if (a < 0)
		return -1;
	b = hex2num(*hex++);
	if (b < 0)
		return -1;
	return (a << 4) | b;
}

size_t printf_decode(char *buf, size_t maxlen, const char *str)
{   const char *pos = str;
	size_t len = 0;
	int val;
	while (*pos) {
	//INFMSG("set wifi *pos=%x \n",*pos);
		if (len == maxlen)
			break;
		switch (*pos) {
		case '\\':
			pos++;
			switch (*pos) {
			case '\\':
				buf[len++] = '\\';
				pos++;
				break;
			case '"':
				buf[len++] = '"';
				pos++;
				break;
			case 'n':
				buf[len++] = '\n';
				pos++;
				break;
			case 'r':
				buf[len++] = '\r';
				pos++;
				break;
			case 't':
				buf[len++] = '\t';
				pos++;
				break;
			case 'e':
				buf[len++] = '\e';
				pos++;
				break;
			case 'x':
				pos++;
				val = hex2byte(pos);
				INFMSG("set wifi val=%x\n",val);
				if (val < 0) {
					val = hex2num(*pos);
					if (val < 0)
						break;
					buf[len++] = val;
					pos++;
				} else {  
					buf[len++] = val;
					pos += 2;
				}
				//INFMSG("set wifi buf=%x len=%d\n",buf[len],len);
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				val = *pos++ - '0';
				if (*pos >= '0' && *pos <= '7')
					val = val * 8 + (*pos++ - '0');
				if (*pos >= '0' && *pos <= '7')
					val = val * 8 + (*pos++ - '0');
				buf[len++] = val;
				break;
			default:
				break;
			}
			break;
		default:
			buf[len++] = *pos++;
			//INFMSG("set wifi buf=%x len=%d\n",buf[len],len);
			break;
		}
	}

	//return len;
	return strlen((const char*)buf);

}


int wifiGetStatus( void )
{
    DBGMSG("status = %d\n", sStatus);
    return sStatus;
}

int wifiOpen( void )
{
    FUN_ENTER;
    wifiClose();
    usleep(200 * 1000);

    if( is_wifi_driver_loaded() ) {
        wifiClose();
    }
    DBGMSG("........ wifi_load_driver begin ........\n");
    if( wifi_load_driver() != 0 ) {
        ERRMSG("wifi_load_driver fail!\n");
        return -1;
    }
    DBGMSG("........ wifi_load_driver end ........\n");

    DBGMSG("........ wifi_start_supplicant begin ........\n");
    if(wifi_start_supplicant(0) != 0 ) {
        wifi_unload_driver();
        ERRMSG("wifi_start_supplicant fail!\n");
        return -2;
    }
    DBGMSG("........ wifi_start_supplicant end ........\n");

    DBGMSG("........ wifi_connect_to_supplicant begin ........\n");

    int cnn_num = 5;
    int cnn_ret = -1;
    while( cnn_num-- ) {
        usleep(200 * 1000);

        if( wifi_connect_to_supplicant() != 0 ) {
            continue;
        } else {
            cnn_ret = 0;
            break;
        }
    }
    DBGMSG("........ wifi_connect_to_supplicant end ........\n");

    if( 0 != cnn_ret ) {
        wifi_stop_supplicant(0);
        wifi_unload_driver();
        ERRMSG("wifi_connect_to_supplicant fail!\n");
        return -3;
    }

    sStatus       = 0;
    sAPNum        = -1;
    sEventLooping = 1;

    pthread_t      ptid;
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&ptid, &attr, wifiEventLoop, NULL);

    sStatus |= WIFI_STATUS_OPENED;
    FUN_EXIT;
    return 0;
}

int wifiAsyncScanAP( void )
{
    FUN_ENTER;
    if( !(sStatus & WIFI_STATUS_OPENED) ) {
        DBGMSG("wifi not opened\n");
        return -1;
    }
    if( sStatus & WIFI_STATUS_SCANNING ) {
        DBGMSG("already scannning...\n");
        return 0;
    }

    sStatus |= WIFI_STATUS_SCANNING;
    sStatus &= ~WIFI_STATUS_SCAN_END;

    DBGMSG("---- wifiScanThread start ----\n");
    int retryNum = 3;
    while( retryNum-- ) {
        sAPNum = wifiSyncScanAP(sAPs, WIFI_MAX_AP);
        if( 0 == sAPNum ) {
            usleep(1400 * 1000);
            DBGMSG("---- wifi retry scan ----\n");
        } else {
            break;
        }
    }

    sStatus &= ~WIFI_STATUS_SCANNING;
    if( sAPNum > 0 ) {
        sStatus |= WIFI_STATUS_SCAN_END;
    } else {

    }
    DBGMSG("---- wifiScanThread exit: num = %d ----\n", sAPNum);
    FUN_EXIT;
    return 0;
}

int wifiGetAPs(struct wifi_ap_t * aps, int maxnum)
{
    FUN_ENTER;
    if( sAPNum <= 0 ) {
        return -1;
    }

    int num = maxnum > sAPNum ? sAPNum : maxnum;
    memcpy(aps, sAPs, num * sizeof(wifi_ap_t));
    FUN_EXIT;
    return num;
}
//------------------------------------------------------------------------------

int wifiGetMACAddr( char * addr, int size )
{
    AT_ASSERT( addr != NULL );
    AT_ASSERT( size >  18 );

    char reply[256];
    int  len = sizeof reply;

    len = wifiCommand("DRIVER MACADDR", reply, len);
    if( len > 0 ) {
        for( int i = 0; i < len; ++i ) {
            if( reply[i] >= 'a' && reply[i] <= 'z' ) {
                reply[i] += ('A' - 'a');
            }
        }
        char * paddr = strstr(reply, "MACADDR = ");
        if( NULL != paddr ) {
            strncpy(addr, paddr + 10, size);
            return len - 10;
        }
    }

    INFMSG("wifiGetMACAddr fail: reply = %s\n", reply);
    return -1;
}

int wifiGetRssi( void )
{
    char reply[256];
    int  len = sizeof reply;
    int  ret = 0;

    len = wifiCommand("SIGNAL_POLL", reply, len);
    if( len > 0) {
        INFMSG("wifiGetRssi: len = %d, rep = \n%s\n", len, reply);
        const char * split = "\n";
        char * p;
        p = strtok (reply, split);
        char rssi[8];
        strncpy(rssi,p+5,strlen(p)-5);
        ret = atoi(rssi);
    }

    return ret;
}

int wifiSyncScanAP(struct wifi_ap_t * aps, int maxnum)
{
    AT_ASSERT( aps != NULL );
    AT_ASSERT( maxnum >  0 );

    char reply[64];
    int  len;

    reply[0] = 0;
    len = sizeof reply;
    if( (wifiCommand("SCAN", reply, len) <= 0) || (NULL == strstr(reply, "OK")) ) {
        ERRMSG("scan fail: %s\n", reply);
        return -2;
    }

    struct timespec to;
    to.tv_nsec = 0;
    to.tv_sec  = time(NULL) + 4;

    pthread_mutex_lock(&sMutxEvent);
    if( 0 == pthread_cond_timedwait(&sCondEvent, &sMutxEvent, &to) ) {
        DBGMSG("wait done.\n");
    } else {
        WRNMSG("wait timeout or error: %s!\n", strerror(errno));
    }
    pthread_mutex_unlock(&sMutxEvent);

    int ret = wifiGetScanResult(aps, maxnum);
    FUN_EXIT;
    return ret;
}

int wifiGetScanResult(struct wifi_ap_t * aps, int maxnum)
{
    FUN_ENTER;
    AT_ASSERT( aps != NULL );
    AT_ASSERT( maxnum >  0 );

    char reply[4096];
    int  len;

    len = wifiCommand("SCAN_RESULTS", reply, sizeof reply);
    if( (len <= 0) || (NULL == strstr(reply, "bssid")) ) {
        ERRMSG("scan results fail: %s\n", reply);
        return -2;
    }

    char * pcur = reply;
    char * pend = reply + len;

    // skip first line(end with '\n'): bssid / frequency / signal level / flags / ssid
    while( *pcur++ != '\n' && pcur < pend ) {
        // nothing
    }

    //
    int num = 0;
    for( ; (num < maxnum) && (pcur < pend); ++num ) {

        char * line = pcur;
        while( *pcur != '\n' && pcur++ < pend ) {
            // nothing
        }

        wifiParseLine(line, pcur, aps + num);
        // skip \n
        pcur++;
    }
    FUN_EXIT;
    return num;
}

//------------------------------------------------------------------------------
int wifiClose( void )
{
    FUN_ENTER;
    sEventLooping = 0;

    wifi_stop_supplicant(0);
    wifi_unload_driver();
    wifi_close_supplicant_connection();

    sStatus = 0;

    memset(sAPs, 0, WIFI_MAX_AP * sizeof(wifi_ap_t));
    sAPNum = 0;

    FUN_EXIT;
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void * wifiEventLoop( void *param )
{
    DBGMSG("---- wifiEventLoop enter ----\n");

    #define EVT_MAX_LEN 127
    char evt[EVT_MAX_LEN + 1];
    int len = 0;

    evt[EVT_MAX_LEN] = 0;
    while( sEventLooping ) {
        evt[0] = 0;
        len = wifi_wait_for_event(evt,EVT_MAX_LEN);
        INFMSG("event: %s\n", evt);

        if( (len > 0) &&(NULL != strstr(evt, "SCAN-RESULTS")) ) {
            DBGMSG(".... scan complete ....\n");
            pthread_mutex_lock(&sMutxEvent);
            pthread_cond_signal(&sCondEvent);
            pthread_mutex_unlock(&sMutxEvent);
        }

        if( (len > 0) &&(NULL != strstr(evt, "CTRL-EVENT-CONNECTED")) ) {
            DBGMSG(".... connect complete ....\n");
            if(sConnectStatus != CONNECTING){
                wifiDhcp();
            }else{
                sConnectStatus = CONNECTED;
                pthread_mutex_lock(&sMutxDhcp);
                pthread_cond_signal(&sCondDhcp);
                pthread_mutex_unlock(&sMutxDhcp);
            }
        }
        if( (len > 0) &&(NULL != strstr(evt, "pre-shared key may be incorrect")) ) {
            DBGMSG(".... connect pre-shared key may be incorrect ....\n");
            sConnectStatus = DISABLED_AUTH_FAILURE;
            pthread_mutex_lock(&sMutxDhcp);
            pthread_cond_signal(&sCondDhcp);
            pthread_mutex_unlock(&sMutxDhcp);
        }
        if( (len > 0) &&(NULL != strstr(evt, "ASSOC-REJECT")) ) {
            DBGMSG(".... connect ASSOC-REJECT ....\n");
            sConnectStatus = DISABLED_ASSOCIATION_REJECT;
            pthread_mutex_lock(&sMutxDhcp);
            pthread_cond_signal(&sCondDhcp);
            pthread_mutex_unlock(&sMutxDhcp);
        }

        if( NULL != strstr(evt, "TERMINATING") ) {
            break;
        }
    }

    DBGMSG("---- wifiEventLoop exit ----\n");
    return NULL;
}
/*//no need to start a thread
void * wifiScanThread( void *param )
{
    DBGMSG("---- wifiScanThread enter ----\n");
    int retryNum = 3;
    while( retryNum-- ) {
        sAPNum = wifiSyncScanAP(sAPs, WIFI_MAX_AP);
        if( 0 == sAPNum ) {
            usleep(1400 * 1000);
            DBGMSG("---- wifi retry scan ----\n");
        } else {
            break;
        }
    }

    sStatus &= ~WIFI_STATUS_SCANNING;
    if( sAPNum > 0 ) {
        sStatus |= WIFI_STATUS_SCAN_END;
    } else {

    }
    DBGMSG("---- wifiScanThread exit: num = %d ----\n", sAPNum);
    return NULL;
}
*/
//------------------------------------------------------------------------------
int wifiCommand(const char * cmder, char * replyBuf, int replySize)
{
    AT_ASSERT( cmder != NULL );
    AT_ASSERT( replyBuf != NULL );
    AT_ASSERT( replySize > 0 );

    size_t replyLen;

    int fail = -1;

    snprintf(mInterfaceCmd, sizeof(mInterfaceCmd), "IFNAME=wlan0 %s", cmder);
    DBGMSG("---- mInterfaceCmd = %s ----\n", mInterfaceCmd);
    for( int i = 0; i < WIFI_CMD_RETRY_NUM; ++i ) {
        replyLen = (size_t)(replySize - 1);

        if( wifi_command(mInterfaceCmd, replyBuf, &replyLen) != 0 ) {
            WRNMSG("'%s'(%d): error, %s(%d)\n", mInterfaceCmd, i, strerror(errno), errno);
            sleep(WIFI_CMD_RETRY_TIME);
            continue;
        } else {
            fail = 0;
            break;
        }
    }

    if( fail ) {
        ERRMSG("'%s' retry %d, always fail!\n", cmder, WIFI_CMD_RETRY_NUM);
        return -1;
    }

    replyBuf[replyLen] = 0;
    return replyLen;
}

//------------------------------------------------------------------------------
void wifiParseLine(const char * begin, const char * end, struct wifi_ap_t * ap)
{
    const char * pcur = begin;
    size_t i = 0;
    // bssid
    while( *pcur != '\t' && pcur < end ) {
        if( i < sizeof(ap->smac) - 1 ) {
            ap->smac[i++] = *pcur;
        }
        pcur++;
    }
    ap->smac[i] = 0;
    int bi = 5;
    char * ps = ap->smac;
    ap->bmac[bi--] = strtol(ps, NULL, 16);
    while( bi >= 0 ) {
        while( *ps && ':' != *ps++ );
        ap->bmac[bi--] = strtol(ps, NULL, 16);
    }
    //INFMSG("mac = %s\n", ap->mac);
    AT_ASSERT( '\t' == *pcur );
    pcur++;
    if( '\t' == *pcur ) {
        pcur++;
    }
    // frequency
    while( *pcur != '\t' && pcur < end ) {
        pcur++;
    }
    AT_ASSERT( '\t' == *pcur );
    pcur++;
    if( '\t' == *pcur ) {
        pcur++;
    }
    // signal level
    char siglev[32];
    int  sli = 0;
    while( *pcur != '\t' && pcur < end ) {
        if( sli < 31 ) {
            siglev[sli++] = *pcur;
        }
        pcur++;
    }
    AT_ASSERT( '\t' == *pcur );
    siglev[sli] = 0;
    //INFMSG("signal = %s\n", siglev);
    ap->sig_level = strtol(siglev, NULL, 10);

    pcur++;
    if( '\t' == *pcur ) {
        pcur++;
    }
    // flags
    int  flagsindex = 0;
    while( *pcur != '\t' && pcur < end ) {
 	if( flagsindex < 63 ) {
            ap->flags[flagsindex++] = *pcur;
        }
        pcur++;
    }
    ap->flags[flagsindex] = 0;
    INFMSG("flags = %s\n", ap->flags);
    AT_ASSERT( '\t' == *pcur );
    pcur++;
    if( '\t' == *pcur ) {
        pcur++;
    }
    // ssid
    memset(ap->name,'\0',sizeof(ap->name));
    i = 0;
    while( *pcur != '\t' && pcur < end ) {
        if( i < sizeof(ap->name) - 1 ) {
            ap->name[i++] = *pcur;
        }
        pcur++;
    }
    ap->name[i] = 0;

    char ssid[128];
    char ssid_1[128];
    memset(ssid,'\0',sizeof(ssid));
    memset(ssid_1,'\0',sizeof(ssid_1));
    int len =strlen(ap->name);
    strncpy(ssid,ap->name,len);
    INFMSG("set wifi before ssid:%s len =%d \n",ssid,len);
    int length = printf_decode((char *) ssid_1, len, ssid);
    INFMSG("set wifi end ssid:%s length=%x \n",ssid_1,length);
    strncpy(ap->name,ssid_1,len);   
    if(needConvert(ssid_1,length) && (ssid_1[length - 1] == GB_FLAG)){
     ssid_1[length - 1] = '\0';//remove the last GB_FLAG
     length --;
     INFMSG("set wifi needConvert :%s \n",ssid_1)
     strncpy(ap->name,ssid_1,len);
       }
    if(length > (len+1)){
    INFMSG("set wifi not need convert :%s \n",ssid); 
    strncpy(ap->name,ssid,len);
       }
    INFMSG("mac = %s, name = %s, sig = %d, flags=%s\n", ap->smac, ap->name, ap->sig_level,ap->flags);
}

static char * wifiStrSkipSpace(const char * pfirst, const char * plast)
{
    AT_ASSERT( pfirst != NULL );
    AT_ASSERT( plast != NULL );
    AT_ASSERT( *pfirst == ' ' );

    while( *pfirst++ == ' ' && pfirst < plast ) {
        // nothing
    }
    if( pfirst >= plast ) {
        ERRMSG("first = %p, last = %p\n", pfirst, plast);
        return NULL;
    }

    return (char *)pfirst;
}

//------------------------------------------------------------------------------
static char * wifiStrFindChar(const char * pfirst, const char * plast, char val)
{
    AT_ASSERT( pfirst != NULL );
    AT_ASSERT( plast != NULL );

    while(*pfirst++ != val && pfirst < plast) {
        // nothing
    }
    if( pfirst >= plast ) {
        ERRMSG("first = %p, last = %p\n", pfirst, plast);
        return NULL;
    }
    return (char *)pfirst;
}

int wifiAddNetwork( char * ssid, char * psk )
{
    FUN_ENTER;
    AT_ASSERT( ssid != NULL );
    AT_ASSERT( psk != NULL );

    char reply[64];
    int  len;
    int netId = -1;

    //add_network
    reply[0] = 0;
    len = sizeof reply;
    len = wifiCommand("ADD_NETWORK", reply, len);
    netId = atoi(reply);
    if( len >= 0 && netId >= 0 ) {
        INFMSG("wifiAddNetwork success: reply = %s  len = %d netId = %d\n", reply, len, netId);
    }else{
        INFMSG("wifiAddNetwork fail: reply = %s\n", reply);
        return -1;
    }
    char netIdchar[4];
    sprintf(netIdchar,"%d",netId);
    char * setnetwork = "SET_NETWORK ";


    //set_network for ssid
    reply[0] = 0;
    len = sizeof reply;
    char * SSID = " ssid ";
    char cmdssid[64] = "";
    strcat(cmdssid,setnetwork);
    strcat(cmdssid,netIdchar);
    strcat(cmdssid,SSID);
    strcat(cmdssid,ssid);
    INFMSG("wifiAddNetwork cmdssid = %s\n", cmdssid);
    if((wifiCommand(cmdssid, reply, len) <= 0) || (NULL == strstr(reply, "OK"))) {
        ERRMSG("wifiAddNetwork cmdssid fail: reply = %s\n", reply);
        return -2;
    }else{
        INFMSG("wifiAddNetwork cmdssid success: reply = %s  len = %d\n", reply, len);
    }

    //set_network for psk
    reply[0] = 0;
    len = sizeof reply;
    char * PSK = " psk ";
    char cmdpsk[64] = "";
    strcat(cmdpsk,setnetwork);
    strcat(cmdpsk,netIdchar);
    strcat(cmdpsk,PSK);
    strcat(cmdpsk,psk);
    INFMSG("wifiAddNetwork cmdpsk = %s\n", cmdpsk);
    if((wifiCommand(cmdpsk, reply, len) <= 0) || (NULL == strstr(reply, "OK"))) {
        ERRMSG("wifiAddNetwork cmdpsk fail: reply = %s\n", reply);
        return -3;
    }else{
        INFMSG("wifiAddNetwork cmdpsk success: reply = %s  len = %d\n", reply, len);
    }


    //enable_network
    reply[0] = 0;
    len = sizeof reply;
    char * enablenetwork = "ENABLE_NETWORK ";
    char cmdenable[64] = "";
    strcat(cmdenable,enablenetwork);
    strcat(cmdenable,netIdchar);
    INFMSG("wifiAddNetwork cmdenable = %s\n", cmdenable);
    if((wifiCommand(cmdenable, reply, len) <= 0) || (NULL == strstr(reply, "OK"))) {
        ERRMSG("wifiAddNetwork cmdenable fail: reply = %s\n", reply);
        return -4;
    }else{
        INFMSG("wifiAddNetwork cmdenable success: reply = %s  len = %d\n", reply, len);
    }

    //save config
    if(0 != wifiSaveConfig()) {
        ERRMSG("wifiAddNetwork SAVE_CONFIG fail");
        return -5;
    }else{
        INFMSG("wifiAddNetwork SAVE_CONFIG success");
    }

    FUN_EXIT;
    return netId;
}

int wifiAddOpenNetwork( char * ssid )
{
    FUN_ENTER;
    AT_ASSERT( ssid != NULL );

    char reply[64];
    int  len;
    int netId = -1;

    //add_network
    reply[0] = 0;
    len = sizeof reply;
    len = wifiCommand("ADD_NETWORK", reply, len);
    netId = atoi(reply);
    if( len >= 0 && netId >= 0 ) {
        INFMSG("wifiAddNetwork success: reply = %s  len = %d netId = %d\n", reply, len, netId);
    }else{
        INFMSG("wifiAddNetwork fail: reply = %s\n", reply);
        return -1;
    }
    char netIdchar[4];
    sprintf(netIdchar,"%d",netId);
    char * setnetwork = "SET_NETWORK ";


    //set_network for ssid
    reply[0] = 0;
    len = sizeof reply;
    char * SSID = " ssid ";
    char cmdssid[64] = "";
    strcat(cmdssid,setnetwork);
    strcat(cmdssid,netIdchar);
    strcat(cmdssid,SSID);
    strcat(cmdssid,ssid);
    INFMSG("wifiAddNetwork cmdssid = %s\n", cmdssid);
    if((wifiCommand(cmdssid, reply, len) <= 0) || (NULL == strstr(reply, "OK"))) {
        ERRMSG("wifiAddNetwork cmdssid fail: reply = %s\n", reply);
        return -2;
    }else{
        INFMSG("wifiAddNetwork cmdssid success: reply = %s  len = %d\n", reply, len);
    }

    //set_network for psk
    reply[0] = 0;
    len = sizeof reply;
    char * KEY_MGMT = " key_mgmt ";
    char cmdpsk[64] = "";
    strcat(cmdpsk,setnetwork);
    strcat(cmdpsk,netIdchar);
    strcat(cmdpsk,KEY_MGMT);
    strcat(cmdpsk,"NONE");
    INFMSG("wifiAddNetwork cmdpsk = %s\n", cmdpsk);
    if((wifiCommand(cmdpsk, reply, len) <= 0) || (NULL == strstr(reply, "OK"))) {
	ERRMSG("wifiAddNetwork cmdpsk fail: reply = %s\n", reply);
	return -3;
    }else{
	INFMSG("wifiAddNetwork cmdpsk success: reply = %s  len = %d\n", reply, len);
    }
		


    //enable_network
    reply[0] = 0;
    len = sizeof reply;
    char * enablenetwork = "ENABLE_NETWORK ";
    char cmdenable[64] = "";
    strcat(cmdenable,enablenetwork);
    strcat(cmdenable,netIdchar);
    INFMSG("wifiAddNetwork cmdenable = %s\n", cmdenable);
    if((wifiCommand(cmdenable, reply, len) <= 0) || (NULL == strstr(reply, "OK"))) {
        ERRMSG("wifiAddNetwork cmdenable fail: reply = %s\n", reply);
        return -4;
    }else{
        INFMSG("wifiAddNetwork cmdenable success: reply = %s  len = %d\n", reply, len);
    }

    //save config
    if(0 != wifiSaveConfig()) {
        ERRMSG("wifiAddNetwork SAVE_CONFIG fail");
        return -5;
    }else{
        INFMSG("wifiAddNetwork SAVE_CONFIG success");
    }

    FUN_EXIT;
    return netId;
}

/*
    public static final int DISABLED_UNKNOWN_REASON                         = 0;
    public static final int DISABLED_DNS_FAILURE                            = 1;
    public static final int DISABLED_DHCP_FAILURE                           = 2;
    public static final int DISABLED_AUTH_FAILURE                           = 3;
    public static final int DISABLED_ASSOCIATION_REJECT                     = 4;
    public static final int CONNECT_SUCCESS                                 = 5;
*/
int wifiConnectNetwork(int netId){
    FUN_ENTER;
    AT_ASSERT( netId != NULL );

    sConnectStatus = CONNECTING;
    char reply[64];
    int  len;
    char netIdchar[4];
    sprintf(netIdchar,"%d",netId);

    //select_network
    reply[0] = 0;
    len = sizeof reply;
    char * selectnetwork = "SELECT_NETWORK ";
    char cmdselect[64] = "";
    strcat(cmdselect,selectnetwork);
    strcat(cmdselect,netIdchar);
    INFMSG("wifiConnectNetwork cmdselect = %s\n", cmdselect);
    if((wifiCommand(cmdselect, reply, len) <= 0) || (NULL == strstr(reply, "OK"))) {
        ERRMSG("wifiConnectNetwork cmdselect fail: reply = %s\n", reply);
        return -1;
    }else{
        INFMSG("wifiConnectNetwork cmdselect success: reply = %s  len = %d\n", reply, len);
    }

    //reconnect
    reply[0] = 0;
    len = sizeof reply;
    if((wifiCommand("RECONNECT", reply, len) <= 0) || (NULL == strstr(reply, "OK"))) {
        ERRMSG("wifiConnectNetwork RECONNECT fail: reply = %s\n", reply);
        return -2;
    }else{
        INFMSG("wifiConnectNetwork RECONNECT success: reply = %s  len = %d\n", reply, len);
    }

    struct timespec to;
    to.tv_nsec = 0;
    to.tv_sec  = time(NULL) + 10;

    pthread_mutex_lock(&sMutxDhcp);
    if( 0 == pthread_cond_timedwait(&sCondDhcp, &sMutxDhcp, &to) ) {
        pthread_mutex_unlock(&sMutxDhcp);
        DBGMSG("wait done.\n");
        if(sConnectStatus != CONNECTED){
            return sConnectStatus;
        }
    } else {
        WRNMSG("wait timeout or error: %s!\n", strerror(errno));
        pthread_mutex_unlock(&sMutxDhcp);
        return DISABLED_UNKNOWN_REASON;
    }

    int retryNum = 3;
    while( retryNum-- ) {
        if( 0 != wifiDhcp() ) {
            usleep(1400 * 1000);
            DBGMSG("---- wifi retry dhcp ----\n");
        } else {
            break;
        }
    }

    if(retryNum == 0){
        return DISABLED_DHCP_FAILURE;
    }
    FUN_EXIT;
    return CONNECTED;
}



int  WiFiIsOnline(char* p_rssi){
    char rssi[15];
    size_t cmd_len;
	char reply[2048];
	char * cmd = "IFNAME=wlan0 STATUS";
	int res;
	int fd;
	cmd_len = strlen(cmd);
    fd = socket(PF_UNIX, SOCK_DGRAM, 0);
	local.sun_family = AF_UNIX;
	snprintf(local.sun_path,
			sizeof(local.sun_path),
			"/data/misc/wifi/sockets/wpa_ctrl_%d_%d",
			(int) getpid(), ++counter);
	/* bind addr */
	if (bind(fd, (struct sockaddr *) &local,
		    sizeof(local)) < 0) {
		if (errno == EADDRINUSE) {
			INFMSG("bind failed: %s\n", strerror(errno));	
		}
		close(fd);
		return -1;
	}

	/* connect to server */
	if (socket_local_client_connect(fd, PATH,
			ANDROID_SOCKET_NAMESPACE_RESERVED, SOCK_DGRAM) < 0) {
		INFMSG("connect to wpa_supplicant failed: %s\n", strerror(errno));	
		close(fd);
		unlink(local.sun_path);
		return -2;
	}

	/* send cmd */
	if (send(fd, cmd, cmd_len, 0) < 0) {
		INFMSG("send cmd failed failed: %s\n", strerror(errno));	
		return -3;
	}

/* get replay */
	res = recv(fd, reply, sizeof(reply), 0);
	reply[res] = '\0';
	INFMSG("replay:\n %s \n", reply);
	/* parse wpa_state value */
    if(res > 0) {
        const char * split = "\n";
        char * p = NULL;
        char rssi[15];
        int wpa_state_value = 0;
        p = strtok(reply, split);
        while(p != NULL){
            if( strncmp("wpa_state",p,9) ==0 ){
                strncpy(rssi, p+10, strlen(p)-10);
                usleep(1000 * 1000);
               //INFMSG("wpa_state_value = %s \n", rssi);
                if(strcmp(rssi,"COMPLETED") == 0){
                INFMSG("Wifi Link Succeeded\n");
                    break;
                }
            }
             p=strtok(NULL,split);
        }
    }
        
	/* close fd */
	close(fd);
	/* remove /data/misc/wifi/sockets/wpa_ctrl_%d_%d */
	unlink(local.sun_path);
	strncpy(p_rssi,rssi,strlen(rssi));
	return 0;
} 
	


int wifiDhcp(){
    DBGMSG("---- wifiDhcp enter ----\n");
    int result;

    DBGMSG("---- wifiDhcp dhcp_stop begin----\n");
    result = dhcp_stop("wlan0");
    if (result != 0) {
        INFMSG("dhcp_stop failed : wlan0");
    }else{
        INFMSG("dhcp_stop success : wlan0");
    }
    DBGMSG("---- wifiDhcp dhcp_stop end----\n");
	
    char  ipaddr[PROPERTY_VALUE_MAX];
    uint32_t prefixLength;
    char gateway[PROPERTY_VALUE_MAX];
    char    dns1[PROPERTY_VALUE_MAX];
    char    dns2[PROPERTY_VALUE_MAX];
    char    dns3[PROPERTY_VALUE_MAX];
    char    dns4[PROPERTY_VALUE_MAX];
    char *dns[5] = {dns1, dns2, dns3, dns4, NULL};
    char  server[PROPERTY_VALUE_MAX];
    uint32_t lease;
    char vendorInfo[PROPERTY_VALUE_MAX];
    char domains[PROPERTY_VALUE_MAX];
    char mtu[PROPERTY_VALUE_MAX];
    DBGMSG("---- wifiDhcp dhcp_do_request begin----\n");
    result = dhcp_do_request("wlan0", ipaddr,gateway, &prefixLength,
            dns, server, &lease, vendorInfo, domains, mtu);
    DBGMSG("---- wifiDhcp dhcp_do_request end----\n");
    if (result != 0) {
        INFMSG("dhcp_do_request failed : wlan0");
    }else{
        INFMSG("wifiConnectNetwork dhcp_do_request  ipaddr = %s\n", ipaddr);
        INFMSG("wifiConnectNetwork dhcp_do_request  gateway = %s\n", gateway);
        INFMSG("wifiConnectNetwork dhcp_do_request  dns1 = %s\n", dns[0]);
        INFMSG("wifiConnectNetwork dhcp_do_request  dns2 = %s  length = %d\n", dns[1], strlen(dns[1]));
        INFMSG("wifiConnectNetwork dhcp_do_request  server = %s\n", server);
        INFMSG("wifiConnectNetwork dhcp_do_request  vendorInfo = %s\n", vendorInfo);
        INFMSG("wifiConnectNetwork dhcp_do_request  mtu = %s\n", mtu);
        strncpy(ip_addr,ipaddr,strlen(ipaddr));
	strncpy(ip_gate,gateway,strlen(gateway));
	INFMSG("wifiConnectNetwork dhcp_do_request  ip_addr = %s\n", ip_addr);
	INFMSG("wifiConnectNetwork dhcp_do_request  ip_gate = %s\n", ip_gate);
    }
    DBGMSG("---- wifiDhcp exit ----\n");

    char cmd[256];
    system("ndc resolver setdefaultif wlan0");
    if(strlen(dns1)>1)
    {
        snprintf(cmd,256,"ndc resolver setifdns wlan0 \"\" %s",dns1);
    }
    if(strlen(dns2)>1)
    {
        snprintf(cmd,256,"%s %s",cmd,dns2);
    }
    if(strlen(dns3)>1)
    {
        snprintf(cmd,256,"%s %s",cmd,dns3);
    }
    if(strlen(dns4)>1)
    {
        snprintf(cmd,256,"%s %s",cmd,dns4);
    }
    system(cmd);
    memset(cmd,0,256);
    if(strlen(gateway)>1)
    {
        snprintf(cmd,128,"ip route add default via %s dev wlan0 table main",gateway);
    }
    system(cmd);

    return result;
}

int wifiForgetNetwork( int netId){
    FUN_ENTER;
    AT_ASSERT( netId != NULL );

    char reply[64];
    int  len;
    char netIdchar[4];
    sprintf(netIdchar,"%d",netId);

    //remove_network
    reply[0] = 0;
    len = sizeof reply;
    char * forgetnetwork = "REMOVE_NETWORK ";
    char cmdforget[64] = "";
    strcat(cmdforget,forgetnetwork);
    strcat(cmdforget,netIdchar);
    INFMSG("wifiForgetNetwork cmdforget = %s\n", cmdforget);
    if((wifiCommand(cmdforget, reply, len) <= 0) || (NULL == strstr(reply, "OK"))) {
        ERRMSG("wifiForgetNetwork cmdforget fail: reply = %s\n", reply);
        return -1;
    }else{
        INFMSG("wifiForgetNetwork cmdforget success: reply = %s  len = %d\n", reply, len);
    }

    //save config
    if(0 != wifiSaveConfig()) {
        ERRMSG("wifiForgetNetwork SAVE_CONFIG fail");
        return -2;
    }else{
        INFMSG("wifiForgetNetwork SAVE_CONFIG success");
    }

    FUN_EXIT;
    return 0;
}

int wifiSaveConfig(){
    char reply[64];
    int  len;
    reply[0] = 0;
    len = sizeof reply;
    if((wifiCommand("SAVE_CONFIG", reply, len) <= 0) || (NULL == strstr(reply, "OK"))) {
        ERRMSG("wifiAddNetwork SAVE_CONFIG fail: reply = %s\n", reply);
        return -1;
    }else{
        INFMSG("wifiAddNetwork SAVE_CONFIG success: reply = %s  len = %d\n", reply, len);
        return 0;
    }
}


int wifiDisconnect( void ){
    char reply[64];
    int  len;
    reply[0] = 0;
    len = sizeof reply;
    if((wifiCommand("DISCONNECT", reply, len) <= 0) || (NULL == strstr(reply, "OK"))) {
        ERRMSG("wifiDisconnect DISCONNECT fail: reply = %s\n", reply);
        return -1;
    }else{
        INFMSG("wifiDisconnect DISCONNECT success: reply = %s  len = %d\n", reply, len);
        return 0;
    }
}

int wifiGetCurrentStatus( void ){
    char reply[256];
    int  len;
    reply[0] = 0;
    len = sizeof reply;
    if(wifiCommand("STATUS", reply, len) <= 0) {
        ERRMSG("wifiGetCurrentStatus STATUS fail: reply = \n%s\n", reply);
        return -1;
    }else{
        INFMSG("wifiGetCurrentStatus STATUS success: reply = \n%s\n  len = %d\n", reply, len);
        return 0;
    }
}

int wifiGetipaddr(char* p_ip_addr)
{
    FUN_ENTER;
    INFMSG("wifiGetipaddr ipaddr = %s\n", ip_addr);
  //  for(int i=0;i<strlen(ip_addr);i++){

    //    INFMSG("wifiGetipaddr   ipaddr = %c\n", ip_addr[i]);
  //      *(p_ip_addr+i)=ip_addr[i];

  //  }
    strncpy(p_ip_addr,ip_addr,strlen(ip_addr));
    INFMSG("wifiGetipaddr ipaddr = %s\n", p_ip_addr); 
    FUN_EXIT;
    return 0;
}

int WiFiGetipMask(char *p_ip_mask)
{
    FUN_ENTER;
    memset( ip_mask, 0, sizeof( ip_mask));
    property_get(ENG_IP_MASK,  ip_mask, "");
    INFMSG("iP_MASK = %s\n",  ip_mask);
    strncpy(p_ip_mask,ip_mask,strlen(ip_mask));
    FUN_EXIT;
    return 0;
}

int WiFiGetGATE(char *p_ip_gate)
{
    FUN_ENTER;
    INFMSG("wifiGetipgate   ip_gate = %s\n", ip_gate); 
    strncpy(p_ip_gate,ip_gate,strlen(ip_gate)); 
    INFMSG("wifiGetipgate   p_ip_gate = %s\n", p_ip_gate); 
    FUN_EXIT;
    return 0;
}

//softap

void * softapEventLoop( void *param )
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

int softapOpen( void )
{
    FUN_ENTER;
    //first close wifi
    wifiClose();
    usleep(100 * 1000);
    //first close softap
    softapClose();
    usleep(200 * 1000);
    DBGMSG("........ wifi_load_driver begin ........\n");
    if( wifi_load_driver() != 0 ) {
        ERRMSG("wifi_load_driver fail!\n");
        return -1;
    }
    DBGMSG("........ wifi_load_driver end ........\n");
    system("ndc softap fwreload wlan0 AP");
    system("ndc softap startap");
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

int softapClose( void ){
    FUN_ENTER;
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


int softapSet(char ssid[], char psk[]){
    FUN_ENTER;
    char cmd[256];
    snprintf(cmd,256,"ndc softap set wlan0 %s broadcast 6 wpa2-psk %s", ssid, psk);
    system(cmd);
    FUN_EXIT;
    return 0;
}

int softapSetMoreParam(int argc, char *argv[]){
    FUN_ENTER;
    char cmd[256];
    snprintf(cmd,256,"ndc softap set %s %s broadcast %s %s %s", argv[2], argv[3], argv[5], argv[6], argv[7]);
    system(cmd);
    FUN_EXIT;
    return 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//--} // namespace
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


