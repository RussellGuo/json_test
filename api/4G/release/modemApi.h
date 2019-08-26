#ifndef MODEMAPI_H
#define MODEMAPI_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int mcc;
    int mnc;
    int ci;
    int pci;
    int tac;
} Cellinfo;

typedef struct
{
    int rat;
    int mcc;//国家码
    int mnc;//运营商码
    int lac;//地区码
    int ci; //编号
    int pci;
    int frq;
    int rsrp;//信号强度
}CellInfo_CCED;


/**
*  说明：所有含SIM卡信息的接口必须需要插上SIM卡后才能运行，不然会报错
*/
/**
* 函数说明：初始化4G API的环境，否则后面函数不能运行
* 参数：无
* 返回值：0： 成功
*        -1：失败
*/
int open4G();
/**
* 函数说明：得到基站信息
* 参数：无
* 返回值：无
*/
CellInfo_CCED * getCellInfo(int* cellcount);
//void getCellInfo();
/**
* 函数说明：更改4G/3G/2G网络
* 参数：网络值  6:4G   14:3G   10:2G
* 返回值：无
*/
void changeNetWork(char* Type);
/**
* 函数说明：初始化网络 ，否则后面函数不能运行
* 示例：setupDataCall();
* 返回值：0： 成功
*        -1：失败
*/
int setupDataCall();
/**
* 函数说明：获取信号强度
* 参数：无
* 返回值：0： 失败
*        其他：成功
*/
int getSignalStrength();
/**
* 函数说明：查询当前SIM卡在位状态
* 参数：无
* 返回值：1： 成功
*         0：失败
*/
int getIccCardState();
/**
* 函数说明：获取网络注册状态
* 参数：无
* 返回值：0： 成功
*        其他：失败
*/
int getVoiceNetworkRegistrationState(char* netTypeWork);
/**
* 函数说明：Creg cgreg注册状态
* 参数：网络格式
* 返回值：1： 注册成功
*        其他：失败
*/
int getDataNetworkRegistrationState();
/**
* 函数说明：获取SIM卡IMSI信息
* 参数：查询的IMSI字符串
* 返回值：0：查询成功
*        -1：查询失败
*/
int getIMSI(char* imsiid);
/**
* 函数说明：获取SIM卡ICCID信息
* 参数：为ICCID信息字符串
* 返回值：0： 查询成功
*        -1：查询失败
*/
int getSimIccId(char* sw0);
/**
* 函数说明：打开PDP上下文
* 参数：信号格式，配置文件，apn，账户，密码，认证方法，协议
* 示例：DataCallConconnect("16", "0", "3gnet", "", NULL, "0", "IPV4V6");
* 返回值：0： 成功
*        -1：失败
*/
int DataCallConconnect(const char* radioTechnology, const char* profile, const char* apn, const char* user, const char* password, const char* authType, const char* protocol );
/**
* 函数说明：关闭PDP上下文
* 参数：无
* 返回值：0： 成功
*        -1：失败
*/
int DataCallDisconnect();
/**
* 函数说明：获取PDP状态
* 参数：无
* 返回值：1： 成功
*         0：失败
*/
int getPdptruestate();
/**
* 函数说明：获取模块制造商信息
* 参数：制造商信息的字符串
* 返回值：无
*/
void GetModuleManufacture(char* manuFacture);
/**
* 函数说明：获取基站列表
* 参数：无
* 返回值：0： 成功
*        其他：失败
*/
int getCellInfoList(Cellinfo* cellinfo);
/**
* 函数说明：获取IMEI号
* 参数：获取的IMEI字符串
* 返回值：无
*/
void getIMEI(char* imei);
/**
* 函数说明：实现打电话的功能
* 参数：电话号码
* 返回值：0： 成功
*        -1：失败
*/
int dial( const char* number );


int hangupConnection( int index );


/**
* 函数说明：查询运营商 网络格式
* 参数：运营商，网络格式
* 返回值：无
*/
void pollState(char* netOwner,char* netType);
/**
* 函数说明：获取SIM卡的ICCID信息
* 参数：ICCID信息
* 返回值：无
*/
void getIccid(char* CCID);
/**
* 函数说明：获取SIM卡的ICCID信息
* 参数：ICCID信息
* 返回值：无
*/
void acceptCall();
/**
* 函数说明：关闭4G API的环境，清理内存（暂未实现）
* 参数：无
* 返回值：0： 成功
*        -1：失败
*/
int close4G();

typedef void (*callBackF)();

void setAnswerCallBack(void (*callBackF)());

#ifdef __cplusplus
}
#endif

#endif
