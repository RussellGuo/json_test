#include "psam_config.h"
#include "psam_protocol.h"
#include "psam_hw_abstractor.h"
#include "psam_apis.h"

static unsigned char s_sc_init = false;
MOD_LOCAL unsigned char g_curslot = 0;
static sc_t g_sc[SC_TERMINAL_NUM];
#ifndef SC_SHARE_QUEUE
static queue_t g_sc_queue[SC_TERMINAL_NUM];
#else
static queue_t g_sc_queue;
#endif
static sc_cfg_t g_sc_cfg[SC_TERMINAL_NUM];
static sc_status_t g_sc_status[SC_TERMINAL_NUM];
static sc_param_t g_sc_param[SC_TERMINAL_NUM];
MOD_LOCAL sc_hw_abs_t g_sc_abs[SC_TERMINAL_NUM];
#if 1//def SC_SIMU_ENABLE
static sc_hw_simu_t g_sc_simu[SC_TERMINAL_NUM];
#endif

static int sc_module_init(void)
{
    int i = 0;
    for (i=0; i<SC_TERMINAL_NUM; i++) {
#ifndef SC_SHARE_QUEUE
        g_sc[i].queue = &g_sc_queue[i];
#else
        g_sc[i].queue = &g_sc_queue;
#endif
        g_sc[i].cfg = &g_sc_cfg[i];
        g_sc[i].status= &g_sc_status[i];
        g_sc[i].param = &g_sc_param[i];
        g_sc[i].abs = &g_sc_abs[i];

        g_sc[i].abs->queue = g_sc[i].queue;
        g_sc[i].abs->status = g_sc[i].status;
        g_sc[i].abs->param = g_sc[i].param;
        g_sc[i].abs->cfg = g_sc[i].cfg;
#if 1
        if ((i == SIMU1) || (i == SIMU2)) {
            g_sc[i].abs->simu = &g_sc_simu[i];
        } else {
             g_sc[i].abs->simu = NULL;
        }
#endif
        sc_core_init(&g_sc[i], i);
        sc_hw_abs_init(g_sc[i].abs);
    }
    g_curslot = 0;
    sc_hw_abs_channel_select(g_curslot);
    s_sc_init = true;
    return SC_ERR_NONE;
}

static int sc_detect(int slot)
{
    if (!s_sc_init) {
        return SC_ERR_NOINIT;
    }
    return sc_hw_abs_detect(g_sc[slot].abs);
}

static int sc_channel_select(unsigned char slot)
{
    int tmp_slot;

    if (!s_sc_init) {
        return SC_ERR_NOINIT;
    }

    tmp_slot = (slot & SLOT_NUM_BIT_MASK);
#if 1//psam only
    if (tmp_slot == 0x00) {
        tmp_slot = PSAM1;
    } else if (tmp_slot == 0x01) {
        tmp_slot = PSAM2;
    } else {
        return SC_ERR_SLOT;
    }
#endif

    if (tmp_slot < SC_TERMINAL_NUM) {
        g_curslot = tmp_slot;
        if (slot & EMV_SPEC_BIT_MASK) {
            g_sc[tmp_slot].cfg->spec = ICC_SPEC_EMV;
        } else if (slot & CITY_SPEC_BIT_MASK) {
            g_sc[tmp_slot].cfg->spec = ICC_SPEC_CITY;
        }  else {
            g_sc[tmp_slot].cfg->spec = ICC_SPEC_ISO7816;
        }

        if (slot & PPS_SUP_BIT_MASK) {
            g_sc[tmp_slot].cfg->pps = ICC_PPS_SUPPORT;
        } else {
            g_sc[tmp_slot].cfg->pps = ICC_PPS_UNSUP;
        }
        return sc_hw_abs_channel_select(tmp_slot);
    } else {
        return SC_ERR_SLOT;
    }
}

static int sc_reset(int slot, unsigned char *atr)
{
    int ret;

    if (!s_sc_init) {
        return SC_ERR_NOINIT;
    }
    ret = sc_core_active(&g_sc[slot], atr);
    if (ret != SC_ERR_NONE) {
        sc_hw_abs_disactive(g_sc[slot].abs);
        ret = g_sc[slot].status->errno;
    }
    return ret;
}

static int sc_format_exchange(int slot, c_apdu_t *capdu, r_apdu_t *rapdu)
{
    int ret;

    if (!s_sc_init) {
        return SC_ERR_NOINIT;
    }

    ret = sc_core_exchange(&g_sc[slot], capdu, rapdu);
    if (ret != SC_ERR_NONE) {
        sc_hw_abs_disactive(g_sc[slot].abs);
        ret = g_sc[slot].status->errno;
    }
    return ret;
}

static int sc_sequeue_exchange(int slot, unsigned char *seqcapdu, int len, unsigned char *seqrapdu, int *rlen)
{
    int ret;
    c_apdu_t capdu;
    r_apdu_t rapdu;

    if (!s_sc_init) {
        return SC_ERR_NOINIT;
    }

    ret = sc_core_format_c_apdu(&capdu, seqcapdu, len);
    if (ret != SC_ERR_NONE) {
        sc_hw_abs_disactive(g_sc[slot].abs);
        return ret;
    }
    ret = sc_format_exchange(slot, &capdu, &rapdu);
    if (ret == SC_ERR_NONE) {
        sc_core_sequence_r_apdu(seqrapdu, rlen, &rapdu);
    } else {
        sc_hw_abs_disactive(g_sc[slot].abs);
    }
    return ret;
}

static int sc_auto_resp_setting(int slot, unsigned char auto_get)
{
    if (!s_sc_init) {
        return SC_ERR_NOINIT;
    }

    if (auto_get) {
        g_sc[slot].cfg->resp = ICC_RESP_AUTO;
    } else {
        g_sc[slot].cfg->resp = ICC_RESP_MANUAL;
    }

    return SC_ERR_NONE;
}

static int sc_close(int slot)
{
    if (!s_sc_init) {
        return SC_ERR_NOINIT;
    }

    sc_core_disactive(&g_sc[slot]);
    return sc_hw_abs_disactive(g_sc[slot].abs);
}

static int sc_suspend(void)
{
    int i;

    if (!s_sc_init) {
        return SC_ERR_NOINIT;
    }

    for (i=0; i<SC_TERMINAL_NUM; i++) {
        sc_core_disactive(&g_sc[i]);
        sc_hw_abs_disactive(g_sc[i].abs);
        sc_hw_abs_deinit(g_sc[i].abs);
    }
    return SC_ERR_NONE;
}

static int sc_resume(void)
{
    int i;

    if (!s_sc_init) {
        return SC_ERR_NOINIT;
    }

    for (i=0; i<SC_TERMINAL_NUM; i++) {
        //sc_core_init(&g_sc[i], i);
        sc_hw_abs_init(g_sc[i].abs);
    }
    return SC_ERR_NONE;
}

static int s_SC_ReturnCode(int errcode)
{
    int ret;

    switch (errcode) {
    case SC_ERR_NONE:           ret = ERR_SC_SUCCESS; break;
    case SC_ERR_NOINIT:         ret = ERR_SC_NORESET; break;
    case SC_ERR_PARAM:          ret = ERR_SC_PARAERR; break;
    case SC_ERR_CARD_REMOVED:   ret = ERR_SC_CARDOUT; break;

    //reset ATR
    case SC_ERR_TS_INVALID:
    case SC_ERR_TA1_INVALID:
    case SC_ERR_TB1_INVALID:
    case SC_ERR_TC1_INVALID:
    case SC_ERR_TD1_INVALID:
    case SC_ERR_TA2_INVALID:
    case SC_ERR_TB2_INVALID:
    case SC_ERR_TC2_INVALID:
    case SC_ERR_TD2_INVALID:
    case SC_ERR_TD2_ABSENT:
    case SC_ERR_TA3_INVALID:
    case SC_ERR_TB3_INVALID:
    case SC_ERR_TB3_ABSENT:
    case SC_ERR_TC3_INVALID:
    case SC_ERR_CRC_UNSUPPORT:
    case SC_ERR_TCK_INVALID:    ret = ERR_SC_ATRERR; break;

    case SC_ERR_PPSS_INVALID:
    case SC_ERR_PPS0_INVALID:
    case SC_ERR_PPS1_INVALID:
    case SC_ERR_PPS2_INVALID:
    case SC_ERR_PPS3_INVALID:
    case SC_ERR_PCK_INVALID:    ret = ERR_SC_PPSERR; break;


    //T0 exchange
    case SC_ERR_SW1_INVALID:
    case SC_ERR_INS_INVALID:    ret = ERR_SC_PROTOCALERR; break;
    case SC_ERR_NACK_EXCEED:    ret = ERR_SC_PARERR; break;

    // T1 exchange
    case SC_ERR_NAD_INVALID:
    case SC_ERR_PCB_INVALID:
                                ret = ERR_SC_PROTOCALERR; break;
    case SC_ERR_LEN_INVALID:
    case SC_ERR_LRC_INVALID:
    case SC_ERR_BLK_IFS:
    case SC_ERR_BLK_RESYNC:
    case SC_ERR_BLK_ABORT:
    case SC_ERR_BLK_TIMES:
                                ret = ERR_SC_APDUERR; break;
    case SC_ERR_BYTE_PARITY:    ret = ERR_SC_PARERR; break;

    // MISC
    case SC_ERR_TIME_VIOLATE:   ret = ERR_SC_TIMEOUT; break;
    case SC_ERR_CARD_INACTIVE:  ret = ERR_SC_NORESET; break;
    case SC_ERR_APDU_INVALID:   ret = ERR_SC_PROTOCALERR; break;
    case SC_ERR_NO_STOP_BIT:    ret = ERR_SC_PROTOCALERR; break;
    case SC_ERR_PWR_OFF:        ret = ERR_SC_NORESET; break;
    case SC_ERR_SLOT:           ret = ERR_SC_SLOTERR; break;
    default:
                                ret = ERR_SC_NORESET; break;
    }
    return ret;
}

MOD_PUBLIC void Lib_IccGetVer(char* ver)
{
    strcpy(ver, SCIM_VERSION);
}

MOD_PUBLIC int s_IccInit(void)
{
    int ret;

    ret = sc_module_init();
    return s_SC_ReturnCode(ret);
}

MOD_PUBLIC unsigned char s_NowSlot(void)
{
    return g_curslot;
}

/********************************************
* Name: sc_api_channel_select
* Input: slot: card channel & reset param
*        tlv: reset fi/di param
* Output: None
* Return: 0: Success; -1: Fail
*********************************************/
MOD_PUBLIC int Lib_SelectSlot(unsigned char slot)
{
    int ret;

    ret = sc_channel_select(slot);
    return s_SC_ReturnCode(ret);
}

/********************************************
* Name: sc_api_detect
* Input: None
* Output: None
* Return: 0: Card present; -2106: Card absent
*********************************************/
MOD_PUBLIC int Lib_IccCheck(unsigned char slot)
{
    int ret;

    ret = sc_detect(slot);
    return s_SC_ReturnCode(ret);
}

/********************************************
* Name: sc_api_auto_resp_setting
* Input: auto_get: whether auto get response by TLL
* Output: None
* Return: 0: Success; -1: Fail
*********************************************/
MOD_PUBLIC int Lib_IccSetAutoResp(unsigned char slot, unsigned char auto_get)
{
    int ret;

    ret = sc_auto_resp_setting(slot, auto_get);
    return s_SC_ReturnCode(ret);
}

/********************************************
* Name: sc_api_reset
* Input: None
* Output: atr: answer to reset data
*         atr[0] stores the length of atr data
* Return: 0: Success; -1: Fail
*********************************************/
MOD_PUBLIC int Lib_IccOpen(unsigned char slot, unsigned char *atr)
{
    int ret;

    ret = sc_reset(slot, atr);
    return s_SC_ReturnCode(ret);
}

/********************************************
* Name: sc_api_sequeue_apdu_exchange
* Input: capdu: command apdu;
*        len: length of capdu
* Output: rapdu: response data of apdu
*         rlen: length of rapdu data
* Return: 0: Success; -1: Fail
*********************************************/
MOD_PUBLIC int Lib_IccSequeueCommand(unsigned char slot, unsigned char *capdu, int len, unsigned char *rapdu, int *rlen)
{
    int ret;

    ret = sc_sequeue_exchange(slot, capdu, len, rapdu, rlen);
    return s_SC_ReturnCode(ret);
}

/********************************************
* Name: sc_api_format_apdu_exchange
* Input: capdu: command apdu in structure
* Output: rapdu: response data in structure
* Return: 0: Success; -1: Fail
*********************************************/
MOD_PUBLIC int Lib_IccCommand(unsigned char slot, c_apdu_t *capdu, r_apdu_t *rapdu)
{
    int ret;

    ret = sc_format_exchange(slot, capdu, rapdu);
    return s_SC_ReturnCode(ret);
}

/********************************************
* Name: sc_api_close
* Input: None
* Output: None
* Return: 0: Success; -1: Fail
*********************************************/
MOD_PUBLIC int Lib_IccClose(unsigned char slot)
{
    int ret;

    ret = sc_close(slot);
    return s_SC_ReturnCode(ret);
}

MOD_PUBLIC int Lib_IccSuspend(void)
{
    int ret;
#ifdef MEMIC
    // It's hardware share between ICC and MEMIC.
    memic_suspend();
#endif
    ret = sc_suspend();
    return s_SC_ReturnCode(ret);
}

MOD_PUBLIC int Lib_IccResume(void)
{
    int ret;

    ret = sc_resume();
#ifdef MEMIC
    memic_resume();
#endif
    return s_SC_ReturnCode(ret);
}

MOD_PUBLIC int sc_hw_self_check(void)
{
    return s_SC_ReturnCode(SC_ERR_NONE);
}

