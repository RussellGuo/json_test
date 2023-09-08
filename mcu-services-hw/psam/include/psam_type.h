#ifndef __SC_TYPE_H__
#define __SC_TYPE_H__

#include "base.h"
#include "psam_queue.h"
#include "psam_config.h"

#define MS_PER_SEC      1000    /* 1000 ms per second */

#define	APDU_MAX_SIZE   512u

#define ETU_CLK_372     372u
#define ETU_CLK_186     186u
#define ETU_CLK_93      93u

typedef enum {
    SCI_MODE_ASYNC = 0,
    SCI_MODE_SYNC,
} sci_mode_t;

typedef enum
{
    ICC_SPEC_EMV = 0,
    ICC_SPEC_ISO7816,
    ICC_SPEC_CITY,
} sc_spec_t;

typedef enum
{
    ICC_VOLT_5V = 0,
    ICC_VOLT_1P8V,
    ICC_VOLT_3P3V,
} sc_volt_t;

enum {
    SC_DISABLE = 0,
    SC_ENABLE,
};

enum
{
    ICC_PPS_UNSUP = 0,
    ICC_PPS_SUPPORT,
};

enum
{
    ICC_CLKSTOP_UNSUP = 0,
    ICC_CLKSTOP_SUPPORT,
};

enum {
    ICC_CLK_STOP = 0,
    ICC_CLK_RUNING,
};

enum
{
    ICC_RESP_AUTO = 0,
    ICC_RESP_MANUAL,
};

enum
{
    ICC_DRVTYP_UNKNOWN = 0,
    ICC_DRVTYP_NONE,
    ICC_DRVTYP_TDA8023,
    ICC_DRVTYP_NCN8025,
};

enum
{
    ICC_ETU_NORMAL = 0,
    ICC_ETU_HALF,
};

enum
{
    ICC_CARD_ABSENT = 0,
    ICC_CARD_PRESENT,
};

enum
{
    ICC_STATE_INACTIVE = 0,
    ICC_STATE_POWEROFF,
    ICC_STATE_RESETING,
    ICC_STATE_PPS,
    ICC_STATE_IFS,
    ICC_STATE_EXCHANGE,
};

enum
{
    ICC_MODE_NEGOTIATED = 0,
    ICC_MODE_SPECIFIC,
};

enum
{
    ICC_DIRECT_IDLE = 0,
    ICC_DIRECT_RCV,
    ICC_DIRECT_XMT,
};

enum
{
    ICC_PROTOCOL_T0 = 0,
    ICC_PROTOCOL_T1,
};

enum
{
    ICC_FORMAT_DIRECT = 0,
    ICC_FORMAT_CONV,
};

enum
{
    ICC_RESET_COLD = 0,
    ICC_RESET_WARM,
};

typedef struct SC_DEV_T sc_t;

struct SC_HW_SIMU;
typedef struct SC_HW_SIMU sc_hw_simu_t;

struct SC_HW_ABSTRACT;
typedef struct SC_HW_ABSTRACT sc_hw_abs_t;

typedef struct IRQ_DESC_ST {
    unsigned char irq;
    volatile unsigned char completed;
    void (*thread)(sc_hw_abs_t *);
    volatile unsigned int data;
} irq_desc_t;

struct SC_HW_SIMU {
    volatile unsigned char parity;
    volatile unsigned char curbyte;
    volatile int byte_step;

    irq_desc_t etu_irq;
    irq_desc_t io_irq;
};

typedef struct SC_CFG {
    unsigned int drvtype : 4;   // 0: unknow,
    unsigned int spec : 3;      // 0: EMV, 1: ISO7816
    unsigned int volt : 2;      // 0: 5V,  1: 1.8V, 2: 3.3V
    unsigned int pps : 1;       // 0: unsupport, 1: support
    unsigned int clkstop : 1;   // 0: unsupport, 1: support  clock stop
    unsigned int resp : 1;      // 0: auto get response, 1: manual get response
    unsigned int f;
    unsigned int d;
} sc_cfg_t;

typedef struct SC_STATUS {
    volatile unsigned int terminal : 3;     // 0: IFM, 1: PSAM1
    volatile unsigned int state : 3;        // 0: inactive, 1: reset, 2: transfer, 3: powerdown
    volatile unsigned int present : 1;      // 0: absent, 1: present
    volatile unsigned int protocol : 1;     // 0: T0, 1: T1
    volatile unsigned int halfetu : 1;      // 0: Normal ETU, 1: Specical ETU
    volatile unsigned int mode : 1;         // 0: Negotiated, 1: Specific mode
    volatile unsigned int clkstop : 1;      // 0: clock continuous, 1: clock stop
    volatile unsigned int retrans_error_cnt : 4;    // T0: byte retrans times; T1: I block retrans times
    volatile int errno;
} sc_status_t;


typedef struct SC_PARAM {
    unsigned char protocol;     // 0: T0, 1: T1
    unsigned char format;       // 0: direct, 1: converse
    unsigned char ifsc;
    unsigned char ifsd;

    unsigned char nad;
    unsigned char isn;      //I block sequence number, for next xmt I block
    unsigned char rsn;      //R block sequence number, for next xmt R block

    unsigned char wwi;
    unsigned char cwi;
    unsigned char bwi;
    unsigned char wtx;

    int fi;
    int di;
    int D;

    unsigned int gt;
    unsigned int bgt;
    unsigned int atrwt;
    unsigned int wwt;
    unsigned int cwt;
    unsigned int bwt;

    volatile unsigned char lrc;
} sc_param_t;

struct SC_HW_ABSTRACT {
    sc_hw_simu_t *simu;
    queue_t *queue;
    sc_cfg_t *cfg;
    sc_status_t *status;
    sc_param_t *param;
};

struct SC_DEV_T {
    queue_t *queue;
    sc_cfg_t *cfg;
    sc_status_t *status;
    sc_param_t *param;
    sc_hw_abs_t *abs;
};

typedef struct{
       unsigned char Command[4];
       unsigned short Lc;
       unsigned char  DataIn[512];
       unsigned short Le;
} APDU_SEND;

typedef struct{
    unsigned short LenOut;
       unsigned char  DataOut[512];
       unsigned char  SWA;
       unsigned char  SWB;
} APDU_RESP;

typedef APDU_SEND c_apdu_t;
typedef APDU_RESP r_apdu_t;


typedef enum
{
    // Common
    SC_ERR_NONE             = 0,
    SC_ERR_FAIL             = (-1),
    SC_ERR_NOINIT           = (-2),
    SC_ERR_PARAM            = (-3),
    SC_ERR_HW               = (-4),
    SC_ERR_OVER_CURRENT     = (-5),
    SC_ERR_CARD_REMOVED     = (-6),

    //reset ATR
    SC_ERR_TS_INVALID       = (-7),
    SC_ERR_TA1_INVALID      = (-8),
    SC_ERR_TB1_INVALID      = (-9),
    SC_ERR_TC1_INVALID      = (-10),
    SC_ERR_TD1_INVALID      = (-11),
    SC_ERR_TA2_INVALID      = (-12),
    SC_ERR_TB2_INVALID      = (-13),
    SC_ERR_TC2_INVALID      = (-14),
    SC_ERR_TD2_INVALID      = (-15),
    SC_ERR_TD2_ABSENT       = (-16),
    SC_ERR_TA3_INVALID      = (-17),
    SC_ERR_TB3_INVALID      = (-18),
    SC_ERR_TB3_ABSENT       = (-19),
    SC_ERR_TC3_INVALID      = (-20),
    SC_ERR_CRC_UNSUPPORT    = (-21),
    SC_ERR_TCK_INVALID      = (-22),
    SC_ERR_PPSS_INVALID     = (-23),
    SC_ERR_PPS0_INVALID     = (-24),
    SC_ERR_PPS1_INVALID     = (-25),
    SC_ERR_PPS2_INVALID     = (-26),
    SC_ERR_PPS3_INVALID     = (-27),
    SC_ERR_PCK_INVALID      = (-28),

    //T0 exchange
    SC_ERR_SW1_INVALID      = (-29),
    SC_ERR_INS_INVALID      = (-30),
    SC_ERR_NACK_EXCEED      = (-31),

    // T1 exchange
    SC_ERR_NAD_INVALID      = (-32),
    SC_ERR_PCB_INVALID      = (-33),
    SC_ERR_LEN_INVALID      = (-34),
    SC_ERR_LRC_INVALID      = (-35),
    SC_ERR_BLK_IFS          = (-36),
    SC_ERR_BLK_RESYNC       = (-37),
    SC_ERR_BLK_ABORT        = (-38),
    SC_ERR_BLK_TIMES        = (-39),
    SC_ERR_BYTE_PARITY      = (-40),

    // MISC
    SC_ERR_TIME_VIOLATE     = (-41),
    SC_ERR_CARD_INACTIVE    = (-42),
    SC_ERR_EVENT_INVALID    = (-43),
    SC_ERR_APDU_INVALID     = (-44),
    SC_ERR_NO_STOP_BIT      = (-45),
    SC_ERR_BYTE_STEP        = (-46),
    SC_ERR_PWR_OFF          = (-47),
    SC_ERR_FOR_USER         = (-48),
    SC_ERR_SLOT             = (-49),
} sc_errno_t;

#endif //__SC_TYPE_H__
