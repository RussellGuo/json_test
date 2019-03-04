#ifndef MODEMAPI_H
#define MODEMAPI_H

#ifdef __cplusplus
extern "C" {
#endif

/**
* 函数说明：初始化4G API的环境，否则后面函数不能运行
* 参数：无
* 返回值：0： 成功
*        -1：失败
*/
int open4G();
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
int getVoiceNetworkRegistrationState();
/**
* 函数说明：Creg cgreg注册状态
* 参数：无
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
* 示例：setupDataCall("16", "0", "3gnet", "", NULL, "0", "IPV4V6");
* 返回值：0： 成功
*        -1：失败
*/
int setupDataCall(const char* radioTechnology, const char* profile, const char* apn, const char* user, const char* password, const char* authType, const char* protocol);
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
int getCellInfoList();
/**
* 函数说明：获取IMEI号
* 参数：获取的IMEI字符串
* 返回值：无
*/
void getIMEI(char* imei);
/**
* 函数说明：关闭4G API的环境，清理内存（暂未实现）
* 参数：无
* 返回值：0： 成功
*        -1：失败
*/
int close4G();
#ifdef __cplusplus
}
#endif

#endif
