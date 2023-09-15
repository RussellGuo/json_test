#ifndef __SC_CONFIG_H__
#define __SC_CONFIG_H__
#include <stdio.h>

#define	SC_DEBUG
#undef SC_DEBUG

#ifdef SC_DEBUG
#define sc_debug     printf
#define sc_if_debug(a, fmt...)  if (a)  printf
#else
#define sc_debug(fmt...)
#define sc_if_debug(a, fmt...)
#endif

#define	SC_ERR_INFO
#undef SC_ERR_INFO

#ifdef SC_ERR_INFO
#define sc_errinfo    printf
#else
#define sc_errinfo(fmt...)
#endif

//#define SC_SHARE_QUEUE
#undef SC_SHARE_QUEUE

//#define USE_TS_INIT
#undef USE_TS_INIT

//#define SIMU_IO_INT_DEFECT
#undef SIMU_IO_INT_DEFECT

#define SIMU1       0
#define SIMU2       1


#if 1
#define PSAM1       SIMU1
#define PSAM2       SIMU2

#define SC_SIMU_ENABLE

#define SC_SIMU_NUM         2

#else
#error "sc_config undefined products"
#endif

#define SC_TERMINAL_NUM     SC_SIMU_NUM


#define DEF_RST_ETU         112u
#define ICC_TS_ETU          115u

// For MH2101 Timer Delta.
#define ATRCWT_EXT_DELTA    200u // For ATRCWT
#define ATRWT_EXT_DELTA     200u // For ATRWT
#define WWT_EXT_DELTA       150u // For WWT
#define BWT_EXT_DELTA       150u // For BWT
#define CWT_EXT_DELTA       2    // For CWT

// baudrate = icc_clk_hz/(fi/di)
// etu = 1/baudrate= (fi/di)/icc_clk_hz

#define PSAM_ICC_DRVTYP             ICC_DRVTYP_NCN8025
#define PSAM_CFG_SPEC               ICC_SPEC_ISO7816
#define PSAM_CFG_VOLT               ICC_VOLT_5V
#define PSAM_CFG_PPS                ICC_PPS_UNSUP
#define PSAM_CFG_CLKSTOP            ICC_CLKSTOP_UNSUP
#define PSAM_CFG_RESP               ICC_RESP_AUTO
#define PSAM_DEF_ETU_CLK            ETU_CLK_372
#define PSAM_DEF_ETU_D              1u
#define PSAM_DEF_SC_BAUDRATE        10752u // 3.75MHz // 9600u // 3.65MHz
#define PSAM_CLK_HZ                 4000000u //(PSAM_DEF_SC_BAUDRATE*PSAM_DEF_ETU_CLK)
#define PSAM_CLK_KHZ                (PSAM_CLK_HZ/1000u)

#define CTRL_ICC_DRVTYP             IFM_ICC_DRVTYP
#define CTRL_CFG_SPEC               IFM_CFG_SPEC
#define CTRL_CFG_VOLT               IFM_CFG_VOLT
#define CTRL_CFG_PPS                IFM_CFG_PPS
#define CTRL_CFG_CLKSTOP            IFM_CFG_CLKSTOP
#define CTRL_CFG_RESP               IFM_CFG_RESP
#define CTRL_DEF_ETU_CLK            IFM_DEF_ETU_CLK
#define CTRL_DEF_ETU_D              IFM_DEF_ETU_D
#define CTRL_CLK_HZ                 IFM_CLK_HZ
#define CTRL_CLK_KHZ                IFM_CLK_KHZ

#define SIMU_ICC_DRVTYP             PSAM_ICC_DRVTYP
#define SIMU_CFG_SPEC               PSAM_CFG_SPEC
#define SIMU_CFG_VOLT               PSAM_CFG_VOLT
#define SIMU_CFG_PPS                PSAM_CFG_PPS
#define SIMU_CFG_CLKSTOP            PSAM_CFG_CLKSTOP
#define SIMU_CFG_RESP               PSAM_CFG_RESP
#define SIMU_DEF_ETU_CLK            PSAM_DEF_ETU_CLK
#define SIMU_DEF_ETU_D              PSAM_DEF_ETU_D
#define SIMU_DEF_SC_BAUDRATE        PSAM_DEF_SC_BAUDRATE
#define SIMU_CLK_HZ                 PSAM_CLK_HZ
#define SIMU_CLK_KHZ                PSAM_CLK_KHZ


#ifdef SC_SIMU_ENABLE
#define SIMU_CLK_TMR_DEV            TIM_7
#define SIMU_ETU_TMR_DEV            TIM_5
#define SIMU_ETU_TMR_DEV_IRQn       TIM0_5_IRQn
#define SC_GP_ETU_DEV               TIM_1
#define SC_GP_ETU_DEV_IRQn          TIM0_1_IRQn
#else
#define SC_GP_ETU_DEV               TIM_1
#define SC_GP_ETU_DEV_IRQn          TIM0_1_IRQn
#endif

#endif //__SC_CONFIG_H__

