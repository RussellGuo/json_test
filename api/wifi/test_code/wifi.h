//
// Spreadtrum Wifi Tester
//
// for linux
//
#ifndef _WIFI_20121110_H__
#define _WIFI_20121110_H__

//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
//--namespace sci_wifi {
//-----------------------------------------------------------------------------

struct wifi_ap_t {
    char bmac[6];  // binary mac
    char smac[24]; // string mac
    char name[24];
    int  sig_level;
};

#define WIFI_STATUS_UNKNOWN     0x0001
#define WIFI_STATUS_OPENED      0x0002
#define WIFI_STATUS_SCANNING    0x0004
#define WIFI_STATUS_SCAN_END    0x0008

typedef unsigned char  byte;
typedef unsigned char  uchar;

#ifndef uint
typedef unsigned int   uint;
#endif // uint

typedef unsigned short ushort;

int wifiGetStatus( void );

int wifiOpen( void );

int wifiGetMACAddr( char * addr, int size );

int wifiGetRssi( void );

int wifiAsyncScanAP( void );
int wifiGetAPs(struct wifi_ap_t * aps, int maxnum);

int wifiSyncScanAP(struct wifi_ap_t * aps, int maxnum);

int wifiClose( void );

int wifiAddNetwork( char * ssid, char * psk );

int wifiConnectNetwork(int netId);

int wifiDhcp( void );

int wifiForgetNetwork( int netId);

int wifiDisconnect( void );

int wifiGetCurrentStatus( void );

//-----------------------------------------------------------------------------
//--};
#ifdef __cplusplus
}
#endif // __cplusplus
//-----------------------------------------------------------------------------

#endif // _WIFI_20121110_H__
