#ifndef __SC_APIS_H__
#define __SC_APIS_H__
#include "psam_type.h"
#include "psam_config.h"

// EMV TestCase 4.3c
#define SCIM_VERSION    "V1.0.0_2018.12.18"

// *********************************************************
// IC卡模块错误码定义 -2100 ~ -2200
#define ERR_SC_SUCCESS          (0)
#define ERR_SC_VCCERR           (-2100)		// 电压模式错误
#define ERR_SC_SLOTERR          (-2101)		// 卡通道错误
#define ERR_SC_PARERR           (-2102)		// 奇偶错误
#define ERR_SC_PARAERR          (-2103)		// 参数值为空
#define ERR_SC_PROTOCALERR      (-2104)		// 协议错误
#define ERR_SC_DATALENERR       (-2105)		// 数据长度错误
#define ERR_SC_CARDOUT          (-2106)		// 卡拨出
#define ERR_SC_NORESET          (-2107)		// 没有初始化
#define ERR_SC_TIMEOUT          (-2108)		// 卡通讯超时
#define ERR_SC_PPSERR           (-2109)     // PPS 错误
#define ERR_SC_ATRERR           (-2110)		// 复位错误
#define ERR_SC_APDUERR          (-2111)		// 卡通讯失败

#define EMV_SPEC_BIT_MASK   0x80
#define CITY_SPEC_BIT_MASK  0x40
#define PPS_SUP_BIT_MASK    0x20
#define SLOT_NUM_BIT_MASK   0x0F

MOD_PUBLIC void Lib_IccGetVer(char* ver);
MOD_PUBLIC int s_IccInit(void);
MOD_PUBLIC unsigned char s_NowSlot(void);
MOD_PUBLIC int Lib_SelectSlot(unsigned char slot);
MOD_PUBLIC int Lib_IccCheck(unsigned char slot);
MOD_PUBLIC int Lib_IccSetAutoResp(unsigned char slot, unsigned char auto_get);
MOD_PUBLIC int Lib_IccOpen(unsigned char slot, unsigned char *atr);
MOD_PUBLIC int Lib_IccSequeueCommand(unsigned char slot, unsigned char *capdu, int len, unsigned char *rapdu, int *rlen);
MOD_PUBLIC int Lib_IccCommand(unsigned char slot, c_apdu_t *capdu, r_apdu_t *rapdu);
MOD_PUBLIC int Lib_IccClose(unsigned char slot);

MOD_PUBLIC int Lib_IccSuspend(void);
MOD_PUBLIC int Lib_IccResume(void);
MOD_PUBLIC int sc_hw_self_check(void);

#endif //__IFMICC_API_H__
