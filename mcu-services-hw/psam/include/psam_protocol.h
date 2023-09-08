#ifndef __SC_PROTOCOL_H__
#define __SC_PROTOCOL_H__

#include "psam_type.h"

#define RFU     0

#define ATR_BUF_SIZE    33u

#define ATR_ORDER       4u
#define ATR_HIST_SIZE   15u

#define ICC_ATR_WT      20160u  //20160
#define ICC_ATR_CWT     10080u
#define ICC_TS_WT       115//115-10//(42000/372=112.90)(43000/372=115.59)

#define ICC_DEF_CWI     10u
#define ICC_DEF_WWI     10u


#define ICC_DEF_GT      11

#define ICC_T0_BGT      16u
#define ICC_T1_BGT      22u

#define ICC_IFSD_SIZE   0xFE

#define ICC_BLOCK_RETRANS_MAX     3u

#define PROC_INS        (capdu->Command[1])
#define PROC_INVINS     (0xFF ^ (capdu->Command[1]))
#define PROC_NULL       0x60u

#define BLK_TYP_I_MASK  0x80u
#define BLK_TYP_R_MASK  0xE0u
#define BLK_TYP_S_MASK  0xC0u
//#define BLK_TYP_MASK    0xC0u
#define SN_INC(sn)      ((sn) = ((sn) ^ 0x01)&0x01 )

#define I_PCB           0x0u
#define I_RFU_MASK      0x1Fu
#define I_SN_BIT        (6)
#define I_M_BIT         (5)

#define I_SN(sn)        (((sn) & 0x01) << I_SN_BIT)
#define I_CHAIN_M       (0x01 << I_M_BIT)

#define R_PCB           0x80u
#define R_SN_BIT        (4)
#define R_FUNC_MASK     0x0Fu
#define R_SN(sn)        (((sn) & 0x01) << R_SN_BIT)

#define S_PCB           0xC0u
#define S_FUNC_MASK     0x1Fu
#define S_RESP_MASK     0x20u

#define S_COD_RESYNC    0x0u
#define S_COD_IFS       0x01u
#define S_COD_ABORT     0x02u
#define S_COD_WTX       0x03u


typedef enum
{
    TA_OPTION_BIT = 4,
    TB_OPTION_BIT = 5,
    TC_OPTION_BIT = 6,
    TD_OPTION_BIT = 7,
} tx_ind_bit_t;

typedef enum
{
    CMD_CASE_1 = 1,
    CMD_CASE_2,
    CMD_CASE_3,
    CMD_CASE_4,
} cmd_case_t;

typedef enum {
    T0_PACKET_DIS_TRANS = 0,
    T0_PACKET_EN_TRANS,
} t0_packet_trans_t;

typedef enum {
    T0_QUEUE_DIS_FLUSH = 0,
    T0_QUEUE_EN_FLUSH,
} t0_queue_flush_t;

typedef enum
{
    NAD = 0,
    PCB,
    LEN,
    INFO,
    S_LRC,
} t1_block_mem_t;

typedef enum
{
    R_COD_ACK = 0x0,
    R_COD_PRTY,
    R_COD_OTHER,
    R_COD_RES,
} r_block_cod_t;

typedef enum E_SC_EVENT
{
    EVENT_T0_START = 0,
    EVENT_T0_CMD,
    EVENT_T0_RCV_PROC,
    EVENT_T0_SPEC_PROC,
    EVENT_T0_INS,
    EVENT_T0_INVINS,
    EVENT_T0_END,   // 6

    EVENT_T1_START, // 7
    EVENT_T1_XMT_RCV,
    EVENT_T1_ORG_BLOCK,
    EVENT_T1_PCB_PARSE,
    EVENT_T1_S_BLOCK,
    EVENT_T1_I_BLOCK,
    EVENT_T1_R_BLOCK,
    EVENT_IFSD_REQUEST,
    EVENT_T1_ERR_HL,
    EVENT_T1_END,   // 16
} sc_event_t;

typedef struct ATR_ST
{
    unsigned char TS;
    unsigned char T0;
    unsigned char TA[ATR_ORDER];
    unsigned char TB[ATR_ORDER];
    unsigned char TC[ATR_ORDER];
    unsigned char TD[ATR_ORDER];
    unsigned char hist[ATR_HIST_SIZE];
    unsigned char TCK;
    unsigned char ta_flag;
    unsigned char tb_flag;
    unsigned char tc_flag;
    unsigned char td_flag;
    unsigned char len;
} icc_atr_t;

typedef struct T1_BLOCK_ST
{
    unsigned char nad;
    unsigned char pcb;
    unsigned char len;
    unsigned char info[APDU_MAX_SIZE+6];
    unsigned char lrc;
    unsigned char *pbuf;
    unsigned char last_i_block_len;
    int remain;
} t1_block_t;

MOD_LOCAL int sc_core_init(sc_t *sc, int terminal);
MOD_LOCAL int sc_core_active(sc_t *sc, unsigned char *atr);
MOD_LOCAL int sc_core_exchange(sc_t *sc, c_apdu_t *capdu, r_apdu_t *rapdu);
MOD_LOCAL int sc_core_disactive(sc_t *sc);
//void sc_transmit_t1_block(sc_t *sc, t1_block_t *block);
//int sc_receive_t1_block(sc_t *sc, t1_block_t *block);

MOD_LOCAL int sc_core_format_c_apdu(c_apdu_t *capdu, unsigned char *seq, int length);
MOD_LOCAL int sc_core_sequence_r_apdu(unsigned char *seq, int *length, r_apdu_t *rapdu);


#endif //__SC_PROTOCOL_H__

