#include "bitopt.h"
#include "psam_config.h"
#include "psam_hw_abstractor.h"
#include "psam_protocol.h"
#include <stdio.h>

static void sc_core_sequeue_atr(unsigned char *atr, icc_atr_t *atr_s)
{
    int i=  0;
    int len = 0;

    atr[len++] = atr_s->len;
    atr[len++] = atr_s->TS;
    atr[len++] = atr_s->T0;
    for (i=0; i<4; i++) {
        if(atr_s->ta_flag & (1<<i)) {
            atr[len++] = atr_s->TA[i];
        }

        if(atr_s->tb_flag & (1<<i)) {
            atr[len++] = atr_s->TB[i];
        }

        if(atr_s->tc_flag & (1<<i)) {
            atr[len++] = atr_s->TC[i];
        }

        if(atr_s->td_flag & (1<<i)) {
            atr[len++] = atr_s->TD[i];
        } else {
            break;
        }
    }
    memcpy(&atr[len], atr_s->hist, (atr_s->T0 & 0x0F));
    len += (atr_s->T0 & 0x0F);
    if (atr_s->len == len) {
        atr[len++] = atr_s->TCK;
    }
}

static int sc_core_parse_case(c_apdu_t *apdu)
{
    /* Case 1: No command data, no response data
     * Case 2: No command data, with response data
     * Case 3: With Command data, no response data
     * Case 4: With Command data, with response data
    */
    int casenum = CMD_CASE_1;

    if (apdu->Lc == 0) {
        if (apdu->Le == 0) {    //  Lc = 0, Le = 0
            casenum = CMD_CASE_1;
        } else {    // Lc = 0, Le != 0
            casenum = CMD_CASE_2;
        }
    } else {
        if (apdu->Le == 0) {    // Lc != 0, Le = 0
            casenum = CMD_CASE_3;
        } else {    // Lc ! = 0, Le != 0
            casenum = CMD_CASE_4;
        }
    }

    return casenum;
}

MOD_LOCAL int sc_core_format_c_apdu(c_apdu_t *capdu, unsigned char *seq, int length)
{
    if ((!capdu) || (length < 4)) {
        return SC_ERR_PARAM;
    }

    memcpy(capdu->Command, seq, 4);
    if (length == 4) {
        // Case 1
        capdu->Lc = 0x0;
        capdu->Le = 0x0;
    } else if (length == 5) {
        // Case 2
        capdu->Lc = 0x0;
        capdu->Le = (seq[4] == 0x0) ? 0x100 : seq[4];
    } else if ((5 + seq[4]) == length) {
        // Case 3
        capdu->Lc = seq[4];
        capdu->Le = 0x0;
        memcpy(capdu->DataIn, &seq[5], capdu->Lc);
    } else if ((5 + seq[4] + 1) == length){
        // Case 4
        capdu->Lc = seq[4];
        memcpy(capdu->DataIn, &seq[5], capdu->Lc);
        capdu->Le = (seq[length-1] == 0x0) ? 0x100 : seq[length-1];
    } else {
        return SC_ERR_PARAM;
    }
    return SC_ERR_NONE;
}

static int sc_core_sequence_c_apdu(unsigned char *seq, int *seqlen, c_apdu_t *apdu)
{
    unsigned char ch = 0;
    int casenum = 0;
    unsigned char *pseq = seq;

    if ((!pseq) || (!seqlen)) {
        return SC_ERR_PARAM;
    }

    memcpy(pseq, apdu->Command, 4);
    pseq += 4;
    casenum = sc_core_parse_case(apdu);
    switch (casenum) {
    case CMD_CASE_1: /*CMD Header*/
        break;

    case CMD_CASE_2: /*CMD Header + Le*/
        if (apdu->Le == 0x100) {
            ch = 0x0;
        } else {
            ch = (unsigned char)(apdu->Le);
        }
        *pseq++ = ch;
        break;

    case CMD_CASE_3: /*CMD Header + Lc + Data(Lc)*/
        ch = (unsigned char)(apdu->Lc);
        *pseq++ = ch;
        memcpy(pseq, apdu->DataIn, ch);
        pseq += ch;
        break;

    case CMD_CASE_4:/*CMD Header + Lc + Data(Lc) + Le*/
        ch = (unsigned char)(apdu->Lc);
        *pseq++ = ch;
        memcpy(pseq, apdu->DataIn, ch);
        pseq += ch;
        if (apdu->Le == 0x100) {
            ch = 0x0;
        } else {
            ch = (unsigned char)(apdu->Le);
        }
        *pseq++ = ch;
        break;

    default:
        break;

    }

    *seqlen = (int)(pseq - seq);
    return SC_ERR_NONE;
}

MOD_LOCAL int sc_core_sequence_r_apdu(unsigned char *seq, int *length, r_apdu_t *rapdu)
{
    unsigned char *pbuf = seq;

    if ((!pbuf) || (!length)) {
        return SC_ERR_PARAM;
    }

    memcpy(pbuf, rapdu->DataOut, rapdu->LenOut);
    pbuf += rapdu->LenOut;
    *pbuf++ = rapdu->SWA;
    *pbuf++ = rapdu->SWB;

    *length = (int)(pbuf - seq);
    return SC_ERR_NONE;
}

static void sc_transmit_block(sc_t *sc, unsigned char *xmt, int length)
{
    queue_xmt(sc->queue, xmt, length);
}

static int sc_receive_block(sc_t *sc, unsigned char *rcv, int length)
{
    int ret;

    ret = sc_hw_wait_xmt_complete(sc->abs);
    if (ret != SC_ERR_NONE) {
        sc->status->errno = ret;
        return ret;
    }

#ifdef SIMU_IO_INT_DEFECT
    sc_hw_abs_wait_reveive(sc->abs);
#endif

    ret = queue_rcv(sc->queue, rcv, length);
    if ((ret == SC_ERR_BYTE_PARITY) && (sc->param->protocol == ICC_PROTOCOL_T1)) {
        sc->status->errno = ret;
        ret = SC_ERR_NONE;
    }
    return ret;
}

static void sc_status_init(sc_status_t *status, int terminal)
{
    status->terminal = terminal;
    status->present = ICC_CARD_PRESENT;
    status->state = ICC_STATE_INACTIVE;
    status->mode = ICC_MODE_NEGOTIATED;
    status->clkstop = ICC_CLK_STOP;
    status->halfetu = ICC_ETU_NORMAL;
    status->retrans_error_cnt = T0_BYTE_RETRANS_MAX;
    status->errno = SC_ERR_NONE;
}

static void sc_active_setting(sc_t * sc)
{
    sc->status->errno = SC_ERR_NONE;
    sc->status->protocol = ICC_PROTOCOL_T1;
    sc->status->halfetu = ICC_ETU_NORMAL;
    sc->param->protocol = ICC_PROTOCOL_T0;
    sc->status->retrans_error_cnt = T0_BYTE_RETRANS_MAX;
    sc->param->format = ICC_FORMAT_DIRECT;
    sc->param->wwt = ICC_TS_WT;
    sc->param->atrwt = (ICC_ATR_WT + ATRWT_EXT_DELTA);  // use for atr wait time
    sc->param->cwt = (ICC_ATR_CWT + ATRCWT_EXT_DELTA);   // use for wait time
    sc->param->cwi = ICC_DEF_CWI;   // use for WI while atr
    sc->param->gt = ICC_DEF_GT;
    sc->param->fi = sc->cfg->f;
    sc->param->di = sc->cfg->d;
    sc->param->ifsd = ICC_IFSD_SIZE;

    if (sc->param->fi != 0)
        sc->param->D = ETU_CLK_372*sc->param->di/sc->param->fi;
    else
        sc->param->D = 1;
}

static void sc_param_setting(sc_t *sc)
{
    sc->status->state = ICC_STATE_EXCHANGE;
    sc->status->errno = SC_ERR_NONE;
    sc->param->nad = 0x0;
    sc->param->isn = 0x0;
    sc->param->rsn = 0x0;
    sc->param->lrc = 0x0;
    sc->status->protocol = sc->param->protocol;
    if(sc->param->protocol == ICC_PROTOCOL_T0) {
        sc->param->bgt = ICC_T0_BGT;
        sc->status->retrans_error_cnt = T0_BYTE_RETRANS_MAX;
    } else {
        sc->param->bgt = ICC_T1_BGT;
    }
    sc->param->wtx = 1;
}

static int sc_core_cfg_init(sc_t *sc)
{

    sc->cfg->drvtype = PSAM_ICC_DRVTYP;
    sc->cfg->spec = PSAM_CFG_SPEC;
    sc->cfg->volt = PSAM_CFG_VOLT;
    sc->cfg->pps = PSAM_CFG_PPS;
    sc->cfg->clkstop = PSAM_CFG_CLKSTOP;
    sc->cfg->resp = PSAM_CFG_RESP;
    sc->cfg->f = PSAM_DEF_ETU_CLK;
    sc->cfg->d = PSAM_DEF_ETU_D;

    return SC_ERR_NONE;
}

int sc_core_init(sc_t *sc, int terminal)
{
    queue_init(sc->queue);
    sc_status_init(sc->status, terminal);
    sc_core_cfg_init(sc);
    return SC_ERR_NONE;
}

static int sc_active_transfer(sc_t *sc, icc_atr_t *atr, unsigned char *atr_out)
{
    unsigned char len = 0;
    unsigned char ch = 0;
    unsigned char option = 0;
    int ret = 0;
    int order = 0;
    int is_tck_present = 0;
    unsigned char xor = 0;
    int i = 0;

    //TS
    ret = sc_receive_block(sc, &ch, 1);
    if (ret != SC_ERR_NONE) {
        return SC_ERR_FAIL;
    }

    if (ch == 0x3B) {
        atr->TS = 0x3B;
        sc->param->format = ICC_FORMAT_DIRECT;
    } else if (ch == 0x3F) {
        atr->TS = 0x3F;
        sc->param->format = ICC_FORMAT_CONV;
    } else if (sc_byte_format_convert(ch) == 0x3F) {
        atr->TS = 0x3F;
        sc->param->format = ICC_FORMAT_CONV;
    } else {
        sc_debug("Invalid TS: %02x\n", ch);
        sc->status->errno = SC_ERR_TS_INVALID;
        return SC_ERR_FAIL;
    }
    len++;

    //format convert should be done in low level
    //T0
    ret = sc_receive_block(sc, &atr->T0, 1);
    if (ret != SC_ERR_NONE) {
        return SC_ERR_FAIL;
    }
    len++;

    order = 0;
    option = (atr->T0 & 0xF0);
    while (option) {
        //TAi
        if (TEST_BIT(option, TA_OPTION_BIT)) {
            SET_BIT(atr->ta_flag, order);
            CLR_BIT(option, TA_OPTION_BIT);
            ret = sc_receive_block(sc, &atr->TA[order], 1);
            if (ret != SC_ERR_NONE) {
                return SC_ERR_FAIL;
            }
            len++;
        }
        //TBi
        if (TEST_BIT(option, TB_OPTION_BIT)) {
            SET_BIT(atr->tb_flag, order);
            CLR_BIT(option, TB_OPTION_BIT);
            ret = sc_receive_block(sc, &atr->TB[order], 1);
            if (ret != SC_ERR_NONE) {
                return SC_ERR_FAIL;
            }
            len++;
        }
        //TCi
        if (TEST_BIT(option, TC_OPTION_BIT)) {
            SET_BIT(atr->tc_flag, order);
            CLR_BIT(option, TC_OPTION_BIT);
            ret = sc_receive_block(sc, &atr->TC[order], 1);
            if (ret != SC_ERR_NONE) {
                return SC_ERR_FAIL;
            }
            len++;
        }
        //TDi
        if (TEST_BIT(option, TD_OPTION_BIT)) {
            SET_BIT(atr->td_flag, order);
            CLR_BIT(option, TD_OPTION_BIT);
            ret = sc_receive_block(sc, &atr->TD[order], 1);
            if (ret != SC_ERR_NONE) {
                return SC_ERR_FAIL;
            }
            len++;

            // It shall accept an ICC returning an ATR not containing TCK if T=0 only is indicated.
            // In all other cases, the terminal shall reject an ICC returning an ATR not containing TCK
            // , or containing an incorrect TCK.
            if ((atr->TD[order] & 0x0F) != 0) {
                is_tck_present = 1;
            }

            option = (atr->TD[order] & 0xF0);
            order++;
        }
    }

    // History bytes
    ch = atr->T0 & 0x0F;
    if(ch>0)
        ret = sc_receive_block(sc, atr->hist, ch);
    if (ret != SC_ERR_NONE) {
        return SC_ERR_FAIL;
    }
    len += ch;

    //TCK
    if (is_tck_present == 1) {
        ret = sc_receive_block(sc, &atr->TCK, 1);
        if (ret != SC_ERR_NONE) {
            return SC_ERR_FAIL;
        }
        len++;
    }
    atr->len = len;
    sc_hw_abs_rcved_setting(sc->abs);

    // tck error should to deactivate ,do not warm reset
    sc_core_sequeue_atr(atr_out, atr);
    if (is_tck_present == 1) {
        // TCK TS start, atr: len TS TO...
        for (i=1; i<atr->len; i++) {
            xor ^= atr_out[i+1];
        }
        if (xor != 0) {
            sc->status->errno = SC_ERR_TCK_INVALID;
            return SC_ERR_FAIL;
        }
    }

    DelayMs(2);

    return SC_ERR_NONE;
}

static const int g_fi_table[] = {372, 372, 558, 744, 1116, 1488, 1860, RFU,
                            RFU, 512, 768, 1024, 1536, 2048, RFU, RFU};
static const int g_di_table[] = {RFU, 1, 2, 4, 8, 16, 32, 64,
                            12, 20, RFU, RFU, RFU, RFU, RFU, RFU};

static int sc_atr_parse(sc_t *sc, icc_atr_t *atr_s, int type)
{
    int iExtN;

    //TA1
    if (TEST_BIT(atr_s->ta_flag, 0)) {
        if ((g_di_table[(atr_s->TA[0] & 0x0F)] == RFU) ||
            (g_fi_table[(atr_s->TA[0] & 0xF0) >> 4] == RFU)) {
            sc->status->errno = SC_ERR_TA1_INVALID;
            return SC_ERR_FAIL;
        }
        if ((sc->cfg->spec == ICC_SPEC_EMV) && (TEST_BIT(atr_s->ta_flag, 1))) {
            // TestCase4.3c 1702.DTS06~DTS11
            // If TA2 is absent, TA1 can be any value. Just used default values.
            // Else, TA1 should be in one of 0x11, 0x12 and 0x13.
            if ((atr_s->TA[0] < 0x11) || (atr_s->TA[0] > 0x13)) {
                sc->status->errno = SC_ERR_TA1_INVALID;
                return SC_ERR_FAIL;
            }
        }
    } else {
        sc->param->fi = sc->cfg->f;
        sc->param->di = sc->cfg->d;
    }

    //TA2
    if (TEST_BIT(atr_s->ta_flag, 1)) { //specific mode
        sc->status->mode = ICC_MODE_SPECIFIC;
        //Bit 8 indicates the ability for changing the negotiable/specific mode:
        if (TEST_BIT(atr_s->TA[1], 7)) {
            //unable to change if bit 8 is set to 1.  -- ISO7816
            // no detail description, do nothing now
        } else {
            //capable to change if bit 8 is set to 0.  -- ISO7816
            // no detail description, do nothing now
        }

        // Bits 7 and 6 are reserved for future use
        if (TEST_BIT(atr_s->TA[1], 6) || TEST_BIT(atr_s->TA[1], 5)) {
            sc->status->errno = SC_ERR_TA2_INVALID;
            return SC_ERR_FAIL;
        }

        //Bit 5 indicates the definition of the parameters F and D.
        if (TEST_BIT(atr_s->TA[1], 4)) {
            if (sc->cfg->spec == ICC_SPEC_EMV) {
                sc->status->errno = SC_ERR_TA2_INVALID;
                return SC_ERR_FAIL;
            }
            //If bit 5 is set to 1, then implicit values (not defined by the interface bytes) shall apply.
            sc->param->fi = sc->cfg->f;
            sc->param->di = sc->cfg->d;
        } else { //If bit 5 is set to 0, then the integers Fi and Di defined above by TA1 shall apply.
            if (TEST_BIT(atr_s->ta_flag, 0)) {
                sc->param->fi = g_fi_table[(atr_s->TA[0] & 0xF0) >> 4];
                sc->param->di = g_di_table[(atr_s->TA[0] & 0x0F)];
            } else {
                sc->param->fi = sc->cfg->f;
                sc->param->di = sc->cfg->d;
            }
        }

        if ((atr_s->TA[1] & 0x0F) == 1) {
            sc->param->protocol = ICC_PROTOCOL_T1;
        } else if ((atr_s->TA[1] & 0x0F) == 0) {
            sc->param->protocol = ICC_PROTOCOL_T0;
        } else {
            sc->status->errno = SC_ERR_TA2_INVALID;
            return SC_ERR_FAIL;
        }
    } else {
        if (sc->cfg->pps == ICC_PPS_SUPPORT) {
            // negotiable mode
            sc->param->fi = g_fi_table[(atr_s->TA[0] & 0xF0) >> 4];
            sc->param->di = g_di_table[(atr_s->TA[0] & 0x0F)];
        } else {
            sc->param->fi = sc->cfg->f;
            sc->param->di = sc->cfg->d;
        }
    }

    if (!(sc->param->fi % ETU_CLK_372)) {
        if (sc->param->fi != 0)
            sc->param->D = ETU_CLK_372*sc->param->di/sc->param->fi;
    } else {
        sc->param->D = 1;
    }

    //TB1
    if (TEST_BIT(atr_s->tb_flag, 0)) {
        if (sc->cfg->spec == ICC_SPEC_EMV) {
            /* Warm reset, can return any value, or not return */
            /* Cold reset, must return TB1=0x0 */
            if ((type == ICC_RESET_COLD) && (atr_s->TB[0] != 0x0)) {
                sc->status->errno = SC_ERR_TB1_INVALID;
                return SC_ERR_FAIL;
            }
        }
    } else {
        if ((sc->cfg->spec == ICC_SPEC_EMV) && (type == ICC_RESET_COLD)) {
            sc->status->errno = SC_ERR_TB1_INVALID;
            return SC_ERR_FAIL;
        }
    }

    //TB2
    if (TEST_BIT(atr_s->tb_flag, 1)) {
        if(sc->cfg->spec == ICC_SPEC_EMV) {
            sc->status->errno = SC_ERR_TB2_INVALID;
            return SC_ERR_FAIL;
        }
    }

    // TD1
    if (TEST_BIT(atr_s->td_flag, 0)) {
        if (sc->cfg->spec == ICC_SPEC_EMV) {
            if((atr_s->TD[0] & 0x0F) > 0x01) {
                sc->status->errno = SC_ERR_TD1_INVALID;
                return SC_ERR_FAIL;
            }
        } else if (sc->cfg->spec == ICC_SPEC_ISO7816) {
            if((atr_s->TD[0] & 0x0F) == 0x0F) {
                sc->status->errno = SC_ERR_TD1_INVALID;
                return SC_ERR_FAIL;
            }
        }

        if ((atr_s->TD[0] & 0x0F) == 0) {
            sc->param->protocol = ICC_PROTOCOL_T0;
        } else if ((atr_s->TD[0] & 0x0F) == 1) {
            sc->param->protocol = ICC_PROTOCOL_T1;
        }
    } else {
        sc->param->protocol = ICC_PROTOCOL_T0;
    }

    // TD2
    if (TEST_BIT(atr_s->td_flag, 1)) {
        if (sc->cfg->spec == ICC_SPEC_EMV) {
            if ((((atr_s->TD[1] & 0x0F) != 0x01) && ((atr_s->TD[1] & 0x0F) != 0x0E))
                    || (((atr_s->TD[1] & 0x0F) == 0x0E) && ((atr_s->TD[0] & 0x0F) == 1))) {
                sc->status->errno = SC_ERR_TD2_INVALID;
                return SC_ERR_FAIL;
            }
        }
    } else {
        if ((sc->cfg->spec == ICC_SPEC_EMV) &&
            (sc->param->protocol == ICC_PROTOCOL_T1)) {
            sc->status->errno = SC_ERR_TD2_ABSENT;
            return SC_ERR_FAIL;
        }
    }

    // TC1 is protocol releative, so it should be parsed after TDx.
    // TC1 encodes the extra guard time integer
    iExtN = 0;
    if (TEST_BIT(atr_s->tc_flag, 0)) {
        if (atr_s->TC[0] == 0xFF) {
            if (sc->param->protocol == ICC_PROTOCOL_T0) {
                sc->param->gt = 12;
            } else {
                sc->param->gt = 11;
                iExtN = -1;
            }
        } else {
            iExtN = atr_s->TC[0];
            sc->param->gt = 12 + atr_s->TC[0];
        }
    } else {
        sc->param->gt = 12;
    }

    // TC2 WWT
    if (TEST_BIT(atr_s->tc_flag, 1)) {
        if (sc->cfg->spec == ICC_SPEC_EMV) {
            if(atr_s->TC[1] != 0x0A) {
                sc->status->errno = SC_ERR_TC2_INVALID;
                return SC_ERR_FAIL;
            }
        }
        sc->param->wwi = atr_s->TC[1];
        sc->param->wwt = ((sc->param->D*(960*sc->param->wwi + 480)) + WWT_EXT_DELTA);
    } else {
        sc->param->wwi = ICC_DEF_WWI;
        sc->param->wwt = ((sc->param->D*(960*sc->param->wwi + 480)) + WWT_EXT_DELTA);
    }

    if (sc->param->protocol == ICC_PROTOCOL_T1) {
        // TA3 IFSC
        if (TEST_BIT(atr_s->ta_flag, 2)) {
            if ((atr_s->TA[2] < 0x10) || (atr_s->TA[2] == 0xFF)) {
                sc->status->errno = SC_ERR_TA3_INVALID;
                return SC_ERR_FAIL;
            }
            sc->param->ifsc = atr_s->TA[2];
        } else {
            sc->param->ifsc = 32;
        }

        // TB3
        // ISO7816:  CWI[3:0] : 0~15, default 13;  BWI[7:4] : 0~9, default 4
        // EMV:      CWI[3:0] : 0~5; BWI[7:4] : 0~4; reject TB3 absent
        if (TEST_BIT(atr_s->tb_flag, 2)) {
            sc->param->cwi = (atr_s->TB[2] & 0x0F);
            sc->param->bwi = ((atr_s->TB[2] >> 4) & 0x0F);
            if (sc->cfg->spec == ICC_SPEC_EMV) {
                if ((sc->param->cwi > 5) || (sc->param->bwi > 4)
                    || ((1 << sc->param->cwi) <= (iExtN + 1))) {
                    sc->status->errno = SC_ERR_TB3_INVALID;
                    return SC_ERR_FAIL;
                }
            }
        } else {
            if (sc->cfg->spec == ICC_SPEC_EMV) {
                sc->status->errno = SC_ERR_TB3_ABSENT;
                return SC_ERR_FAIL;
            } else if (sc->cfg->spec == ICC_SPEC_ISO7816) {
                sc->param->cwi = 13;
                sc->param->bwi = 4;
            }
        }
        sc->param->cwt = ((1 << sc->param->cwi) + 11 + 4 + CWT_EXT_DELTA);
        sc->param->bwt = ((sc->param->D*(1 << sc->param->bwi)*960) + 11 + BWT_EXT_DELTA);

        // TC3
        if (TEST_BIT(atr_s->tc_flag, 2)) {
            if (sc->cfg->spec == ICC_SPEC_EMV) {
                if (atr_s->TC[2] != 0) {
                    sc->status->errno = SC_ERR_TC3_INVALID;
                    return SC_ERR_FAIL;
                }
            } else if (sc->cfg->spec == ICC_SPEC_ISO7816) {
                if (atr_s->TC[2] == 0) {
                    // use LRC
                } else if (atr_s->TC[2] != 1) {
                    // use CRC
                } else {
                    sc->status->errno = SC_ERR_CRC_UNSUPPORT;
                    return SC_ERR_FAIL;
                }
            }
        }
    }

    return SC_ERR_NONE;
}

static int sc_pps_request(sc_t *sc, icc_atr_t *atr, unsigned char *option)
{
    int i = 0;
    int cnt = 0;
    unsigned char req[5] = {0};

    // PPSS
    req[i++] = 0xFF;

    // PPS0: option[6:4], T[3:0]
    req[i++] |= sc->param->protocol;

    //PPS1: F[7:4], D[3:0]
    if (TEST_BIT(atr->ta_flag, 0)) {
        req[i++] = atr->TA[0];
        SET_BIT(req[1], 4);
    }

    //PPS2: SPU, propose not use
    if (0) {
        req[i++] = 0x00;
        SET_BIT(req[1], 5);
    }

    // PPS3: reserved for future use
    if (0) {
        req[i++] = 0x00;
        SET_BIT(req[1], 6);
    }

    // PCK
    for (cnt=0; cnt<i; cnt++) {
        req[i] ^= req[cnt];
    }
    i++;
    *option = (req[1] & 0xF0);

    sc_transmit_block(sc, req, i);
    sc_hw_abs_packet_transmit(sc->abs);
    return 0;
}

static int sc_pps_response(sc_t *sc, icc_atr_t *atr, unsigned char option)
{
    int ret = SC_ERR_NONE;
    int i = 0;
    int cnt = 0;
    int pck = 0;
    unsigned char resp[5] = {0};

    // PPSS
    ret = sc_receive_block(sc, &resp[0], 1);
    if (ret != SC_ERR_NONE) {
        return SC_ERR_FAIL;
    }
    if (resp[i++] != 0xFF) {
        sc->status->errno = SC_ERR_PPSS_INVALID;
        return SC_ERR_FAIL;
    }

    // PPS0
    ret = sc_receive_block(sc, &resp[i++], 1);
    if (ret != SC_ERR_NONE) {
        return SC_ERR_FAIL;
    }
    if ((resp[1] & 0x0F) != sc->param->protocol) {
        sc->status->errno = SC_ERR_PPS0_INVALID;
        return SC_ERR_FAIL;
    }
    if (((!TEST_BIT(option, 4)) && TEST_BIT(resp[1], 4))
        || ((!TEST_BIT(option, 5)) && TEST_BIT(resp[1], 5))
        || ((!TEST_BIT(option, 6)) && TEST_BIT(resp[1], 6))) {
        // Not send PPS1 but response PPS1
        // Not send PPS2 but response PPS2
        // Not send PPS3 but response PPS3
        sc->status->errno = SC_ERR_PPS0_INVALID;
        return SC_ERR_FAIL;
    }

    // PPS1
    if ((TEST_BIT(option, 4)) && (TEST_BIT(resp[1], 4))) {
        //  Sent PPS1 and response PPS1
        ret = sc_receive_block(sc, &resp[i++], 1);
        if(ret != SC_ERR_NONE) {
            return SC_ERR_FAIL;
        }
        if(resp[2] != atr->TA[0]) {
            sc->status->errno = SC_ERR_PPS1_INVALID;
            return SC_ERR_FAIL;
        }
        sc->param->fi = g_fi_table[((resp[2]>>4) & 0x0F)];
        sc->param->di = g_di_table[(resp[2] & 0x0F)];
    } else {
        // No response PPS1, use default value
        sc->param->fi = sc->cfg->f;
        sc->param->di = sc->cfg->d;
    }

    //PPS2
    if ((TEST_BIT(option, 5)) && (TEST_BIT(resp[1], 5))) {
        //  Sent PPS1 and response PPS1
        ret = sc_receive_block(sc, &resp[i++], 1);
        if(ret != SC_ERR_NONE) {
            return SC_ERR_FAIL;
        }
    }

    // PPS3
    if ((TEST_BIT(option, 6)) && (TEST_BIT(resp[1], 6))) {
        //  Sent PPS1 and response PPS1
        ret = sc_receive_block(sc, &resp[i++], 1);
        if(ret != SC_ERR_NONE) {
            return SC_ERR_FAIL;
        }
    }

    // PCK
    ret = sc_receive_block(sc, &resp[i++], 1);
    if (ret != SC_ERR_NONE) {
        return SC_ERR_FAIL;
    }

    for (cnt=0; cnt<i; cnt++) {
        pck ^= resp[cnt];
    }
    if (pck != 0) {
        sc->status->errno = SC_ERR_PCK_INVALID;
        return SC_ERR_FAIL;
    }

    return SC_ERR_NONE;
}

static int sc_pps_exchange(sc_t *sc, icc_atr_t *atr)
{
    unsigned char option = 0;
    int ret = SC_ERR_NONE;

    sc->param->fi = sc->cfg->f;
    sc->param->di = sc->cfg->d;

    ret = sc_pps_request(sc, atr, &option);
    if (ret != SC_ERR_NONE) {
        return SC_ERR_FAIL;
    }

    ret = sc_pps_response(sc, atr, option);
    if (ret != SC_ERR_NONE) {
        return SC_ERR_FAIL;
    }
    sc_hw_abs_rcved_setting(sc->abs);
        return ret;
}

static int sc_core_t1_exhcange(sc_t *sc, c_apdu_t *capdu, r_apdu_t *rapdu, int start_ev);
static int sc_ifsd_process(sc_t *sc)
{
    return sc_core_t1_exhcange(sc, NULL, NULL, EVENT_IFSD_REQUEST);
}

static int sc_core_active_exchange(sc_t *sc, unsigned char *atr)
{
    int ret = SC_ERR_NONE;
    int type = ICC_RESET_COLD;
    int should_warm_rst = 1;
    icc_atr_t atr_s;

core_active:
    sc->status->errno = SC_ERR_NONE;
    if(sc->status->present == ICC_CARD_ABSENT) {
        sc->status->errno = SC_ERR_CARD_REMOVED;
        return SC_ERR_FAIL;
    }
    queue_flush(sc->queue);
    sc_active_setting(sc);
    memset(&atr_s, 0, sizeof(icc_atr_t));

    if ((sc->status->state == ICC_STATE_INACTIVE) ||
            (sc->status->state == ICC_STATE_POWEROFF)) {
        ret = sc_hw_abs_active(sc->abs, ICC_RESET_COLD);
    } else {
        ret = sc_hw_abs_active(sc->abs, ICC_RESET_WARM);
    }
    if(ret != SC_ERR_NONE) {
        return SC_ERR_FAIL;
    }

    sc->status->state = ICC_STATE_RESETING;
    ret = sc_active_transfer(sc, &atr_s, atr);
    if(ret != SC_ERR_NONE) {
        return SC_ERR_FAIL;
    }
    ret = sc_atr_parse(sc, &atr_s, type);
    if(ret != SC_ERR_NONE) {
        if (should_warm_rst) {
            should_warm_rst = 0;
            type = ICC_RESET_WARM;
            sc_errinfo("do warm reset, cause errno=%d \n", sc->status->errno);
            goto core_active;
        } else {
            return SC_ERR_FAIL;
        }
    }

    if((sc->cfg->pps == ICC_PPS_SUPPORT)
        && (sc->status->mode == ICC_MODE_NEGOTIATED)) {
        sc_debug("sc_pps_exchange \n");
        sc->status->state = ICC_STATE_PPS;
        sc->status->protocol = sc->param->protocol;
        ret = sc_pps_exchange(sc, &atr_s);
        if(ret != SC_ERR_NONE) {
            return SC_ERR_FAIL;
        }
    }

    return SC_ERR_NONE;
}

int sc_core_active(sc_t *sc, unsigned char *atr)
{
    int ret = 0;
    ret = sc_core_active_exchange(sc, atr);
    if (ret != SC_ERR_NONE) {
        sc_debug("sc_core_active_exchange, errno=%d\n", sc->status->errno);
        return ret;
    }
    sc_param_setting(sc);
    if (sc->param->protocol == ICC_PROTOCOL_T1) {
        sc_debug("sc_ifsd_process \n");
        ret = sc_ifsd_process(sc);
        if (ret != SC_ERR_NONE) {
            DelayMs(2);
            sc_errinfo("sc_ifsd_process, errno=%d\n", sc->status->errno);
            return SC_ERR_FAIL;
        }
    }

    return ret;
}

static void sc_transmit_t0_packet(sc_t *sc, unsigned char *seq, int len, t0_queue_flush_t flush, t0_packet_trans_t trans)
{
    if (flush == T0_QUEUE_EN_FLUSH) {
        queue_flush(sc->queue);
    }
    sc_transmit_block(sc, seq, len);

    if (trans == T0_PACKET_EN_TRANS) {
        sc_hw_abs_packet_transmit(sc->abs);
    }
}

static int sc_receive_t0_packet(sc_t *sc, unsigned char *seq, int len)
{
    return sc_receive_block(sc, seq, len);
}

static unsigned char g_cmd_get_resp[5] = {0x00, 0xC0, 0x00, 0x00, 0x00};

static int sc_core_t0_exhcange(sc_t *sc, c_apdu_t *capdu, r_apdu_t *rapdu)
{
    unsigned char sw1;
    unsigned char sw2;
    unsigned char sw1_bak;
    unsigned char sw2_bak;
    unsigned char sw_bak_flag;
    unsigned char processing;
    unsigned char casenum;
    unsigned char dlen;
    unsigned short sw;
    int ret;
    int event = EVENT_T0_START;
    int tx_remain;
    int rx_remain;
    unsigned char *pxmt = capdu->DataIn;
    unsigned char *prcv = rapdu->DataOut;

#if 0
    {
        int i;

        sc_debug("lc=%d, le=%d \n", capdu->Lc, capdu->Le);
        sc_debug("Command: ");
        for (i=0; i<4; i++) {
            sc_debug("%02x ", capdu->Command[i]);
        }
        sc_debug("\n");
        sc_debug("data: ");
        for (i=0; i<capdu->Lc; i++) {
            sc_debug("%02x ", capdu->DataIn[i]);
        }
        sc_debug("\n");
    }
#endif

    if ((capdu == NULL) || (rapdu == NULL)) {
        return SC_ERR_PARAM;
    }

    sw1= 0;
    sw2 = 0;
    sw1_bak = 0;
    sw2_bak = 0;
    dlen = 0;
    casenum = CMD_CASE_1;
    processing = 1;
    sw_bak_flag = 0;
    tx_remain = 0;
    rx_remain = 0;
    event = EVENT_T0_START;

    do {
        switch (event) {

        case EVENT_T0_START:
            sc->status->errno = SC_ERR_NONE;
            memset((char *)rapdu, 0, sizeof(r_apdu_t));
            casenum = sc_core_parse_case(capdu);
            event = EVENT_T0_CMD;
            break;

        case EVENT_T0_CMD:
            pxmt = capdu->DataIn;
            prcv = rapdu->DataOut;

            if (CMD_CASE_2 == casenum) {
                dlen = capdu->Le;
                tx_remain = 0;
            } else {
                dlen = capdu->Lc;
                tx_remain = dlen;
            }

            rx_remain = capdu->Le;
            event = EVENT_T0_RCV_PROC;
            sc_transmit_t0_packet(sc, capdu->Command, 4, T0_QUEUE_EN_FLUSH, T0_PACKET_DIS_TRANS);
            sc_transmit_t0_packet(sc, &dlen, 1, T0_QUEUE_DIS_FLUSH, T0_PACKET_EN_TRANS);
            processing = 1;
            break;

        case EVENT_T0_RCV_PROC:
            ret = sc_receive_t0_packet(sc, &sw1, 1);
            if (ret != SC_ERR_NONE) {
                event = EVENT_T0_END;
            } else {
                if (sw1 == PROC_NULL) {
                    // = 0x60
                    event = EVENT_T0_RCV_PROC;
                } else if (((sw1 & 0xF0) == 0x60) || ((sw1 & 0xF0) == 0x90)) {
                    // = 0x6X, 0x9X
                    event = EVENT_T0_SPEC_PROC;
                } else if (sw1 == PROC_INS) {
                    //= INS
                    event = EVENT_T0_INS;
                } else if (sw1 == PROC_INVINS) {
                    // = INVINS
                    event = EVENT_T0_INVINS;
                } else {
                    sc->status->errno = SC_ERR_SW1_INVALID;
                    event = EVENT_T0_END;
                }
            }
            //sc_debug("EVENT_T0_RCV_PROC, sw1=%02x, event=%d, casenum=%d \r\n", sw1, event, casenum);
            break;

        case EVENT_T0_INS:
            // Should prodedure all remaining data bytes, then reception a procedure byte
            processing = 0;
            if (casenum == CMD_CASE_2) {
                if (rx_remain != 0) {
                    ret = sc_receive_t0_packet(sc, prcv, rx_remain);
                    if (ret != SC_ERR_NONE) {
                        return SC_ERR_FAIL;
                    }
                    prcv += rx_remain;
                    rapdu->LenOut += rx_remain;
                    rx_remain = 0x0;
                    event = EVENT_T0_RCV_PROC;
                } else {
                    sc->status->errno = SC_ERR_SW1_INVALID;
                    event = EVENT_T0_END;
                }
            } else if (casenum > CMD_CASE_2) {
                if (tx_remain != 0) { ///@test_emv_l1_170222 case 1705
                    event = EVENT_T0_RCV_PROC;
                    sc_transmit_t0_packet(sc, pxmt, tx_remain, T0_QUEUE_EN_FLUSH, T0_PACKET_EN_TRANS);
                    pxmt += tx_remain;
                    tx_remain = 0x0;
                } else {
                    sc->status->errno = SC_ERR_SW1_INVALID;
                    event = EVENT_T0_END;
                }
            } else {
                sc->status->errno = SC_ERR_INS_INVALID;
                event = EVENT_T0_END;
            }
            break;

        case EVENT_T0_INVINS:
            // Should prodedure the next data byte, then reception a procedure byte
            processing = 0;
            if (casenum == CMD_CASE_2)  {
                if (rx_remain != 0) {
                    ret = sc_receive_t0_packet(sc, prcv, 1);
                    if (ret != SC_ERR_NONE) {
                        return SC_ERR_FAIL;
                    }
                    prcv++;
                    rapdu->LenOut += 1;
                    rx_remain--;
                    event = EVENT_T0_RCV_PROC;
                } else {
                    sc->status->errno = SC_ERR_SW1_INVALID;
                    event = EVENT_T0_END;
                }
            } else if (casenum > CMD_CASE_2) {
                if(tx_remain != 0) {
                    event = EVENT_T0_RCV_PROC;
                    sc_transmit_t0_packet(sc, pxmt, 1, T0_QUEUE_EN_FLUSH, T0_PACKET_EN_TRANS);
                    pxmt++;
                    tx_remain--;
                } else {
                    sc->status->errno = SC_ERR_SW1_INVALID;
                    event = EVENT_T0_END;
                }
            } else {
                sc->status->errno = SC_ERR_INS_INVALID;
                event = EVENT_T0_END;
            }
            break;

        case EVENT_T0_SPEC_PROC:
            ret = sc_receive_t0_packet(sc, &sw2, 1);
            if (ret != SC_ERR_NONE) {
                event = EVENT_T0_END;
            } else {
                sc_hw_abs_rcved_setting(sc->abs);
                rapdu->SWA = sw1;
                rapdu->SWB = sw2;
                sc_debug("sw1=%02x, sw2=%02x, casenum=%d \n", sw1, sw2, casenum);
                event = EVENT_T0_END;
                sc->status->errno = SC_ERR_NONE;
                if (sc->cfg->resp == ICC_RESP_AUTO) {
                    // auto get response by TTL
                    sw = ((sw1 << 8) | sw2);
                    if (sw == 0x9000) {
                        // Command success, should terimal!
                        event = EVENT_T0_END;
                    } else if (sw1 == 0x61) {
                        // Get response with P3 set to the minimum of Le and sw2.
                        memcpy(capdu->Command, g_cmd_get_resp, 4);
                        dlen = sw2;
                        rx_remain = dlen;
                        capdu->Le = dlen;
                        casenum = CMD_CASE_2;
                        event = EVENT_T0_RCV_PROC;
                        sc_transmit_t0_packet(sc, capdu->Command, 4, T0_QUEUE_EN_FLUSH, T0_PACKET_DIS_TRANS);
                        sc_transmit_t0_packet(sc, &dlen, 1, T0_QUEUE_DIS_FLUSH, T0_PACKET_EN_TRANS);
                    } else if (sw1 == 0x6C) {
                        // Retransmit last Command with P3 = sw2 (Licc).
                        dlen = sw2;
                        rx_remain = dlen;
                        capdu->Le = dlen;
                        casenum = CMD_CASE_2;
                        event = EVENT_T0_RCV_PROC;
                        sc_transmit_t0_packet(sc, capdu->Command, 4, T0_QUEUE_EN_FLUSH, T0_PACKET_DIS_TRANS);
                        sc_transmit_t0_packet(sc, &dlen, 1, T0_QUEUE_DIS_FLUSH, T0_PACKET_EN_TRANS);
                    } else if ((sw == 0x6281) || (sw == 0x6700) || (sw == 0x6A86) || (sw == 0x6F00)){
                        // An error condition occurs
                        event = EVENT_T0_END;
                    } else if ((casenum == 4) && (processing == 0)
                            && ((sw1 == 0x62) || (sw1 == 0x63) || ((sw1 & 0xF0) == 0x90))) {
                            sw1_bak = sw1;
                            sw2_bak = sw2;
                            sw_bak_flag = 1;
                            memcpy(capdu->Command, g_cmd_get_resp, 4);
                            dlen = 0x0;
                            rx_remain = 0x100;
                            capdu->Le = 0x100;
                            casenum = CMD_CASE_2;
                            event = EVENT_T0_RCV_PROC;
                            sc_transmit_t0_packet(sc, capdu->Command, 4, T0_QUEUE_EN_FLUSH, T0_PACKET_DIS_TRANS);
                            sc_transmit_t0_packet(sc, &dlen, 1, T0_QUEUE_DIS_FLUSH, T0_PACKET_EN_TRANS);
                    } else {
                        event = EVENT_T0_END;
                    }
                }
            }
            break;

        default:
            sc->status->errno = SC_ERR_EVENT_INVALID;
            event = EVENT_T0_END;
            break;

        }
    } while (event != EVENT_T0_END);

    if (sw_bak_flag) {
        rapdu->SWA = sw1_bak;
        rapdu->SWB = sw2_bak;
    }

    sc_debug("sc_core_t0_exhcange errno=%d \r\n", sc->status->errno);
    if (sc->status->errno == SC_ERR_NONE)
        return SC_ERR_NONE;
    else
        return SC_ERR_FAIL;
}

static void sc_iblock_packet(sc_t *sc, t1_block_t *block)
{
    block->nad = sc->param->nad;
    if (block->remain > sc->param->ifsc) {
        block->pcb = (I_PCB | I_SN(sc->param->isn) | I_CHAIN_M);
        block->len = sc->param->ifsc;
    } else {
        block->pcb = (I_PCB | I_SN(sc->param->isn));
        block->len = block->remain;
    }
    block->last_i_block_len = block->len;
    memcpy(block->info, block->pbuf, block->len);
    block->pbuf += block->len;
    block->remain -= block->len;
    SN_INC(sc->param->isn);
    //Lrc should be generated by CTRL or CTRL simulator
}

static void sc_rblock_packet(sc_t *sc, t1_block_t *block, int typ)
{
    block->nad = sc->param->nad;
    block->pcb = (R_PCB | R_SN(sc->param->rsn) | typ);
    block->len = 0;
    // R block have no info.
    //Lrc should be generated by CTRL or CTRL simulator
}

static void sc_sblock_packet(sc_t *sc, t1_block_t *block, int typ, unsigned char info)
{
    block->nad = sc->param->nad;
    block->pcb = (S_PCB | typ);
    block->len = 1;
    block->info[0] = info;
    //Lrc should be generated by CTRL or CTRL simulator
}

static int sc_t1_pcb_parse(t1_block_t *block)
{
    int event;

    if ((block->pcb & BLK_TYP_R_MASK) == R_PCB) {
        event = EVENT_T1_R_BLOCK;
    } else if((block->pcb & BLK_TYP_S_MASK) == S_PCB) {
        event = EVENT_T1_S_BLOCK;
    } else if((block->pcb & BLK_TYP_I_MASK) == I_PCB) {
        event = EVENT_T1_I_BLOCK;
    } else {
        event = EVENT_T1_ERR_HL;
    }

    return event;
}

static void sc_transmit_t1_block(sc_t *sc, t1_block_t *block);
static int sc_receive_t1_block(sc_t *sc, t1_block_t *block);
static int sc_sblock_deal(sc_t *sc, t1_block_t *rcv, t1_block_t *xmt)
{
    int event;
    t1_block_t temp_xmt;

    if( ((rcv->pcb & S_RESP_MASK)==0)
            && ((rcv->pcb & S_FUNC_MASK) == S_COD_ABORT) ) {
        ///@emv_l1_test_v43a_case 1792
        sc->status->errno = SC_ERR_BLK_ABORT;
        event = EVENT_T1_END;
    } else if (rcv->len != 0x01) {
        sc->status->errno = SC_ERR_LEN_INVALID;
        event = EVENT_T1_ERR_HL;
    } else if (rcv->pcb & S_RESP_MASK) {
        // S Response
        if (((xmt->pcb & BLK_TYP_S_MASK) == S_PCB)///@emv_l1_test_v43a_case 1770
                && ((xmt->pcb & S_RESP_MASK)==0)///@emv_l1_test_v43a_case 1808
                && ((rcv->pcb & S_FUNC_MASK) == S_COD_IFS)) {
            // Last block is S Request block
            if(rcv->info[0] != sc->param->ifsd) {
                sc->status->errno = SC_ERR_BLK_IFS;
                event = EVENT_T1_ERR_HL;
            } else {
                event = EVENT_T1_END;
            }
        } else {
            // Not support
            sc->status->errno = SC_ERR_PCB_INVALID;
            event = EVENT_T1_ERR_HL;
        }
    } else {
        // S Request
        if ((rcv->pcb & S_FUNC_MASK) == S_COD_IFS) {
            // Receive IFS request
            if (((xmt->pcb & BLK_TYP_S_MASK) == S_PCB) && ((xmt->pcb & S_RESP_MASK) == 0)) {
                ///@emv_l1_test_v43a_case 1804.12
                sc->status->errno = SC_ERR_BLK_IFS;
                event = EVENT_T1_ERR_HL;
            } else if ((rcv->info[0] > 0x0F) && (rcv->info[0] < 0xFF)) {
                memcpy(&temp_xmt, xmt, sizeof(t1_block_t));
                sc_sblock_packet(sc, xmt, (S_COD_IFS | S_RESP_MASK), rcv->info[0]);
                sc->param->ifsc = rcv->info[0];
                sc->status->retrans_error_cnt = ICC_BLOCK_RETRANS_MAX;
                event = EVENT_T1_XMT_RCV;
            } else {
                sc->status->errno = SC_ERR_BLK_IFS;
                event = EVENT_T1_ERR_HL;
            }
        } else if ((rcv->pcb & S_FUNC_MASK) == S_COD_WTX) {
            sc->param->wtx = rcv->info[0];
            memcpy(&temp_xmt, xmt, sizeof(t1_block_t));
            sc_sblock_packet(sc, xmt, S_COD_WTX | S_RESP_MASK, rcv->info[0]);
            event = EVENT_T1_XMT_RCV;
        } else if ((rcv->pcb & S_FUNC_MASK) == S_COD_ABORT) {
            sc->status->errno = SC_ERR_BLK_ABORT;
            event = EVENT_T1_END;
        } else {
            // RFU
            sc->status->errno = SC_ERR_PCB_INVALID;
            event = EVENT_T1_ERR_HL;
        }
    }

    if ( (event == EVENT_T1_XMT_RCV)
            && ((temp_xmt.pcb & BLK_TYP_I_MASK) == I_PCB) ) {
        sc_transmit_t1_block(sc, xmt);
        memcpy(xmt, &temp_xmt, sizeof(t1_block_t));
        memset(&temp_xmt, 0, sizeof(t1_block_t));
        event = sc_receive_t1_block(sc, rcv);
        sc->param->wtx = 1;
    }

    return event;
}

static int sc_iblock_deal(sc_t *sc, t1_block_t *rcv, t1_block_t *xmt, r_apdu_t *rapdu)
{
    int event;

    if ((rcv->len == 0) || (rcv->len == 0xFF)///@emv_l1_test_v43a_case 1770
            || (xmt->remain > 0)) {///@emv_l1_test_v43a_case 1781.15
        sc->status->errno = SC_ERR_PCB_INVALID;
        event = EVENT_T1_ERR_HL;
    } else if (((xmt->pcb & BLK_TYP_S_MASK) == S_PCB)
            && ((xmt->pcb & S_RESP_MASK) == 0) ) {///@emv_l1_test_v43a_case 1804
        sc->status->errno = SC_ERR_PCB_INVALID;
        event = EVENT_T1_ERR_HL;
    } else if (rcv->pcb & I_RFU_MASK) {
        sc->status->errno = SC_ERR_PCB_INVALID;
        event = EVENT_T1_ERR_HL;
    } else if (IF_BIT_SET(rcv->pcb, I_SN_BIT) != sc->param->rsn) {///@emv_l1_test_v43a_case 1770
        sc->status->errno = SC_ERR_PCB_INVALID;
        event = EVENT_T1_ERR_HL;
    } else if ( ((xmt->pcb & BLK_TYP_I_MASK) == I_PCB) &&
                    TEST_BIT(xmt->pcb, I_M_BIT)) {///@emv_l1_test_v43a_case 1776
        sc->status->errno = SC_ERR_PCB_INVALID;
        event = EVENT_T1_ERR_HL;
    } else if (TEST_BIT(rcv->pcb, I_M_BIT)) {///@emv_l1_test_v43a_case 1776
        memcpy(&rapdu->DataOut[rapdu->LenOut], rcv->info, rcv->len);
        rapdu->LenOut += rcv->len;
        SN_INC(sc->param->rsn);
        sc_rblock_packet(sc, xmt, R_COD_ACK);
        event = EVENT_T1_XMT_RCV;
        sc->status->retrans_error_cnt = ICC_BLOCK_RETRANS_MAX;
    } else {
        if (rcv->len == 0) {
            sc->status->errno = SC_ERR_LEN_INVALID;
            event = EVENT_T1_ERR_HL;
        } else {
            SN_INC(sc->param->rsn);
            memcpy(&rapdu->DataOut[rapdu->LenOut], rcv->info, rcv->len);
            rapdu->LenOut += rcv->len;
            rapdu->SWA = rapdu->DataOut[rapdu->LenOut-2];
            rapdu->SWB = rapdu->DataOut[rapdu->LenOut-1];
            rapdu->LenOut -= 2;
        }
        event = EVENT_T1_END;
    }

    return event;
}

static void sc_last_iblock_packet(sc_t *sc, t1_block_t *block)
{
    // Retransmit last I block
    block->pbuf -= block->last_i_block_len;//xmt->len;
    block->remain += block->last_i_block_len;//xmt->len;
    SN_INC(sc->param->isn);     //Really DEC 1
    sc_iblock_packet(sc, block);
}

static int sc_rblock_deal(sc_t *sc, t1_block_t *rcv, t1_block_t *xmt)
{
    int event;

    if (rcv->len != 0) { // R-block with LEN != 0
        sc->status->errno = SC_ERR_PCB_INVALID;
        event = EVENT_T1_ERR_HL;
    } else if (((xmt->pcb & BLK_TYP_I_MASK) == I_PCB)
                || ((xmt->pcb & BLK_TYP_R_MASK) == R_PCB)) {
        if ((rcv->pcb & R_FUNC_MASK) == R_COD_ACK) {
            if (IF_BIT_SET(rcv->pcb, R_SN_BIT) == sc->param->isn) {
                if (xmt->remain>0) {///@emv_l1_test_v43a_case 1769(I)/1776(R)
                    // Continue I block transmit
                    sc_iblock_packet(sc, xmt);
                    sc->status->retrans_error_cnt = ICC_BLOCK_RETRANS_MAX;
                    event = EVENT_T1_XMT_RCV;
                } else { ///@emv_l1_test_v43a_case 1770(I)
                    sc->status->errno = SC_ERR_PCB_INVALID;
                    event = EVENT_T1_ERR_HL;
                }
            } else {///@emv_l1_test_v43a_case 1769(I)/1774(R)
                // Retransmit last block
//                sc->status->retrans_error_cnt --;
                event = EVENT_T1_XMT_RCV;
            }
        } else if ( ((rcv->pcb & R_FUNC_MASK) == R_COD_PRTY)
                    || ((rcv->pcb & R_FUNC_MASK) == R_COD_OTHER) ) {
            if (IF_BIT_SET(rcv->pcb, R_SN_BIT) != sc->param->isn) {///@emv_l1_test_v43a_case 1769(I)/1771(R)
                if ( ((xmt->pcb & BLK_TYP_R_MASK) == R_PCB)
                        && ((xmt->pcb & R_FUNC_MASK) == R_COD_ACK)) {///@emv_l1_test_v43a_case 1778
                    // Retransmit last block
                    event = EVENT_T1_XMT_RCV;
                } else {
                    // Retransmit last I block
                    sc_last_iblock_packet(sc, xmt);
                    sc->status->retrans_error_cnt --;
                    if (sc->status->retrans_error_cnt>0) {
                        event = EVENT_T1_XMT_RCV;
                    } else { ///@emv_l1_test_v43a_case 1780/1786
                        sc->status->errno = SC_ERR_BLK_TIMES;
                        event = EVENT_T1_END;
                    }
                }
            } else {///@emv_l1_test_v43a_case 1772/1776/1777(R)
                if ((xmt->pcb & BLK_TYP_I_MASK) == I_PCB) {///@emv_l1_test_v43a_case 1771
                    sc->status->errno = SC_ERR_PCB_INVALID;
                    event = EVENT_T1_ERR_HL;
                } else {
                    // Retransmit last block
                    // sc->status->retrans_error_cnt --;///@emv_l1_test_v43a_case 1774
                    event = EVENT_T1_XMT_RCV;
                }

            }
        } else {
            sc->status->errno = SC_ERR_PCB_INVALID;
            event = EVENT_T1_ERR_HL;
        }
    }else if ((xmt->pcb & BLK_TYP_S_MASK) == S_PCB) {///@emv_l1_test_v43a_case 1797/1804
        sc->status->errno = SC_ERR_PCB_INVALID;
        event = EVENT_T1_ERR_HL;
    }else {
        sc->status->errno = SC_ERR_PCB_INVALID;
        event = EVENT_T1_ERR_HL;
    }

    return event;
}

static int sc_errblock_deal(sc_t *sc, t1_block_t *rcv, t1_block_t *xmt)
{
    int ret = 0;

    ( void )rcv;
    sc->status->retrans_error_cnt--;///@emv_l1_test_v43a_case 1781/1813
    if (sc->status->retrans_error_cnt != 0) {
        if ( ((xmt->pcb & BLK_TYP_S_MASK) == S_PCB)
                && ((xmt->pcb & S_RESP_MASK)==0)) {///@emv_l1_test_v43a_case 1797/1804
            // Retransmit last S block(IFSD)
            ret = EVENT_T1_XMT_RCV;
        } else if ( ((xmt->pcb & BLK_TYP_R_MASK) == R_PCB)) { ///@emv_l1_test_v43a_case 1770/1774/1778/1784
            // Retransmit last R block
            ret = EVENT_T1_XMT_RCV;
        }
        else {
            if ((sc->status->errno == SC_ERR_LRC_INVALID)
                    || (sc->status->errno == SC_ERR_BYTE_PARITY)) {
                sc_rblock_packet(sc, xmt, R_COD_PRTY);
            } else {
                sc_rblock_packet(sc, xmt, R_COD_OTHER);
            }
        }
        sc->status->errno = SC_ERR_NONE;
        ret = EVENT_T1_XMT_RCV;
    } else {
        sc->status->errno = SC_ERR_BLK_TIMES;
        ret = EVENT_T1_END;
    }

    return ret;
}

static void sc_transmit_t1_block(sc_t *sc, t1_block_t *block)
{
    queue_flush(sc->queue);
    // Not include LRC
    // Lrc will generate by HW Ctrl or Simulator
#if 0
    sc_transmit_block(sc, &block->nad, 1);
    sc_transmit_block(sc, &block->pcb, 1);
    sc_transmit_block(sc, &block->len, 1);
    sc_transmit_block(sc, block->pbuf, block->len);
#else
    sc_transmit_block(sc, (unsigned char *)block, (block->len + 3));
#endif
    sc_hw_abs_packet_transmit(sc->abs);
}

static int sc_receive_t1_block(sc_t *sc, t1_block_t *block)
{
    unsigned char lrc = 0;
    int i = 0;
    int ret = 0;

    ret = sc_receive_block(sc, &block->nad, 1);
    if (ret != SC_ERR_NONE) {
        return EVENT_T1_END;
    }

    ret = sc_receive_block(sc, &block->pcb, 1);
    if (ret != SC_ERR_NONE) {
        return EVENT_T1_END;
    }

    ret = sc_receive_block(sc, &block->len, 1);
    if (ret != SC_ERR_NONE) {
        return EVENT_T1_END;
    }

    ret = sc_receive_block(sc, block->info, block->len);
    if (ret != SC_ERR_NONE) {
        return EVENT_T1_END;
    }

    ret = sc_receive_block(sc, &block->lrc, 1);
    if (ret != SC_ERR_NONE) {
        return EVENT_T1_END;
    }
    sc_hw_abs_rcved_setting(sc->abs);

    if (sc->status->errno == SC_ERR_BYTE_PARITY) {
        return EVENT_T1_ERR_HL;
    }

    lrc ^= block->nad;
    lrc ^= block->pcb;
    lrc ^= block->len;
    lrc ^= block->lrc;

    for (i=0; i<block->len; i++) {
        lrc ^= block->info[i];
    }

    if (lrc != 0x0) {
        sc->status->errno = SC_ERR_LRC_INVALID;
        return EVENT_T1_ERR_HL;
    }

    //check nad, must be 0x0.
    if (block->nad != 0x0) {
        sc->status->errno = SC_ERR_NAD_INVALID;
        return EVENT_T1_ERR_HL;
    }

    return EVENT_T1_PCB_PARSE;
}

static int sc_core_t1_exhcange(sc_t *sc, c_apdu_t *capdu, r_apdu_t *rapdu, int start_ev)
{
    int event = start_ev;
    t1_block_t xmt_block;
    t1_block_t rcv_block;
    unsigned char seq_buf[APDU_MAX_SIZE+6];

#if 0
    if (capdu != NULL) {
        int i;

        sc_debug("lc=%d, le=%d \n", capdu->Lc, capdu->Le);
        sc_debug("Command: ");
        for (i=0; i<4; i++) {
            sc_debug("%02x ", capdu->Command[i]);
        }
        sc_debug("\n");
        sc_debug("data: ");
        for (i=0; i<capdu->Lc; i++) {
            sc_debug("%02x ", capdu->DataIn[i]);
        }
        sc_debug("\n");
    }
#endif

    if (rapdu) {
        memset(rapdu, 0, sizeof(r_apdu_t));
    }
    memset(&xmt_block, 0, sizeof(t1_block_t));
    memset(&rcv_block, 0, sizeof(t1_block_t));
    sc->status->retrans_error_cnt = ICC_BLOCK_RETRANS_MAX;
    do {
        switch(event) {
        case EVENT_T1_START:
            sc_core_sequence_c_apdu(seq_buf, &xmt_block.remain, capdu);
            xmt_block.pbuf = seq_buf;
            event = EVENT_T1_ORG_BLOCK;
            break;

        case EVENT_T1_ORG_BLOCK:
            sc_iblock_packet(sc, &xmt_block);
            event = EVENT_T1_XMT_RCV;
            break;

        case EVENT_IFSD_REQUEST:
            sc_sblock_packet(sc, &xmt_block, S_COD_IFS, sc->param->ifsd);
            event = EVENT_T1_XMT_RCV;
            break;

        case EVENT_T1_XMT_RCV:
//            if (sc->param->wtx > 1) {
//                sc->param->bwt *= sc->param->wtx;
//            }
            sc_transmit_t1_block(sc, &xmt_block);
            event = sc_receive_t1_block(sc, &rcv_block);
//            sc->param->bwt *= sc->param->wtx;///@emv_l1_test_v43a_case 1775
            sc->param->wtx = 1;
            break;

        case EVENT_T1_PCB_PARSE:
            event = sc_t1_pcb_parse(&rcv_block);
            break;

        case EVENT_T1_S_BLOCK:
            event = sc_sblock_deal(sc, &rcv_block, &xmt_block);
            break;

        case EVENT_T1_I_BLOCK:
            event = sc_iblock_deal(sc, &rcv_block, &xmt_block, rapdu);
            break;

        case EVENT_T1_R_BLOCK:
            event = sc_rblock_deal(sc, &rcv_block, &xmt_block);
            break;

        case EVENT_T1_ERR_HL:
            event = sc_errblock_deal(sc, &rcv_block, &xmt_block);
            break;

        default:
            sc->status->errno = SC_ERR_EVENT_INVALID;
            event = EVENT_T1_END;
            break;

        }
    } while (event != EVENT_T1_END);

    if (sc->status->errno == SC_ERR_NONE) {
        return SC_ERR_NONE;
    } else {
        return SC_ERR_FAIL;
    }
}

int sc_core_exchange(sc_t *sc, c_apdu_t *capdu, r_apdu_t *rapdu)
{
    int ret;

    if (sc->status->state != ICC_STATE_EXCHANGE) {
        sc->status->errno = SC_ERR_CARD_INACTIVE;
        return SC_ERR_FAIL;
    }

    if (sc->param->protocol == ICC_PROTOCOL_T0) {
        ret = sc_core_t0_exhcange(sc, capdu, rapdu);
    } else {
        ret =  sc_core_t1_exhcange(sc, capdu, rapdu, EVENT_T1_START);
    }

    if (ret != SC_ERR_NONE) {
        DelayMs(2);
        sc_errinfo("sc_core_exchange, errno=%d \n", sc->status->errno);
    }
    return ret;
}

int sc_core_disactive(sc_t *sc)
{
    queue_flush(sc->queue);
    sc_core_cfg_init(sc);
    return SC_ERR_NONE;
}

