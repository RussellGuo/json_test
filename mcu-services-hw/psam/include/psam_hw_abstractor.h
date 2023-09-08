#ifndef __SC_HW_ABSTRATOR_H__
#define __SC_HW_ABSTRATOR_H__

#include "makefun.h"
#include "psam_config.h"
#include "psam_type.h"

#define T0_BYTE_RETRANS_MAX     5u

#if 1//def SC_SIMU_ENABLE
typedef void(*sc_simu_cb_t)(sc_hw_abs_t *);
//MOD_LOCAL void sc_hw_simu_etu_pit_isr(void);
#endif
MOD_LOCAL unsigned char sc_byte_format_convert(unsigned char byte);
MOD_LOCAL int sc_hw_abs_init(sc_hw_abs_t *abs);
MOD_LOCAL int sc_hw_abs_detect(sc_hw_abs_t *abs);
MOD_LOCAL int sc_hw_abs_channel_select(int slot);
#ifdef SIMU_IO_INT_DEFECT
MOD_LOCAL int sc_hw_abs_wait_reveive(sc_hw_abs_t *abs);
#endif
MOD_LOCAL int sc_hw_abs_active(sc_hw_abs_t *abs, int type);
MOD_LOCAL int sc_hw_abs_rcved_setting(sc_hw_abs_t *abs);
MOD_LOCAL int sc_hw_abs_packet_transmit(sc_hw_abs_t *abs);
MOD_LOCAL int sc_hw_wait_xmt_complete(sc_hw_abs_t *abs);
MOD_LOCAL int sc_hw_abs_disactive(sc_hw_abs_t *abs);
MOD_LOCAL int sc_hw_abs_deinit(sc_hw_abs_t *abs);
MOD_LOCAL int sc_hw_card_status(sc_hw_abs_t *abs);
MOD_LOCAL void sc_uart_reset(void);

#endif //__IFMICC_HW_ABSTRATOR_H__
