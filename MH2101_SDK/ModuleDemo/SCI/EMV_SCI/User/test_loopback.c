#include <stdio.h>
#include "mhscpu_sci.h"
#include "iso7816_3.h"
#include "emv_errno.h"
#include "emv_hard.h"
#include "test_emv.h"
#include "test_loopback.h"
#include "uart.h"

void loop_back(uint8_t u8Slot)
{
    uint8_t u8Wait = 5, *pu8Str = "Class B(3.0V)";
    int32_t s32Vol, s32Len, s32Stat = 0;
    uint8_t  atr[65];
    ST_APDU_RSP     rsp;
    ST_APDU_REQ     apdu_req;

    s32Vol = VCC_3000mV;
    
    DBG_PRINT("EMV Lib version 0x%08x\r\n", iso7816_get_version());
    
    
#if 1    
    DBG_PRINT("\r\nchoose voltage:\r\n1:3V\r\n2:1.8V\r\nOther:3V\r\n");
    //while(~0UL == (s32Stat = uart_RecvChar()));
    s32Stat = '1';
    if ('1' == s32Stat)
    {
        s32Vol = VCC_3000mV;
        pu8Str = "Class B(3.0V)";
    }
    else if ('2' == s32Stat)
    {
        s32Vol = VCC_1800mV;
        pu8Str = "Class C(1.8V)";
    }
    else
    {
        s32Vol = VCC_3000mV;
        pu8Str = "Class B(3.0V)";
    }
    DBG_PRINT("Voltage %s\r\n",pu8Str);
#endif    
    //DBG_PRINT("Vol %d\n\n\n", s32Vol);
    while (1)
    {
        //ICBC will not drawn ICC in next test case.
        wait_ChangeNs(u8Wait);
        
        //Init emv_devs param.
        iso7816_device_init();
        while (0 != iso7816_detect(u8Slot))
        {
            DBG_PRINT("Wait insert ICC!\n");
            delay_Ms(500);
        }
        DBG_PRINT("Insert IC\n");
        if (0 == (s32Stat = iso7816_init(u8Slot, s32Vol | SPD_1X, atr)))
        {
            DBG_PRINT("iso7816_init finished!\n");
        }
        else
        {
            DBG_PRINT("iso7816_init failed %d!\n", s32Stat);
        }

        rsp.len_out = 0;
        while (0 == iso7816_detect(u8Slot) && 0 == s32Stat)
        {
            if ('b' == uart_RecvChar())
            {
                DBG_PRINT("Break loop!\n");
                break;
            }
            if (rsp.len_out < 4)
            {
                //INS equal 70 then deactivate card.
                if (rsp.len_out > 0 && 0x70 == rsp.data_out[1])
                {
                    DBG_PRINT("This test Case finished!\n");
                    break;
                }
                DBG_PRINT("Prepare to send select PSE!\n");
                memcpy(apdu_req.cmd, "\x00\xa4\x04\x00", 4);
                apdu_req.lc = 14;
                memcpy( apdu_req.data_in, "1PAY.SYS.DDF01", apdu_req.lc);
                apdu_req.le = 256;
                //print_hex("select", (void *)&apdu_req, 128);
            }
            else
            {
                //INS equal 70 then deactivate card.
                if (0x70 == rsp.data_out[1])
                {
                    DBG_PRINT("This test Case finished!\n");
                    break;
                }
                DBG_PRINT("Prepare to send loopback data!\n");
                //Copy next C-APDU from this R-APDU.
                memcpy(apdu_req.cmd, rsp.data_out, sizeof(apdu_req.cmd));
                
                //Len except (CLA INS P1 P2)
                apdu_req.lc = 0;
                apdu_req.le = 0;
                s32Len = rsp.len_out - sizeof(apdu_req.cmd);
                if (s32Len > 1 )
                {
                    apdu_req.lc = rsp.data_out[sizeof(apdu_req.cmd)];
                    memcpy(apdu_req.data_in, rsp.data_out + 1 + sizeof(apdu_req.cmd), apdu_req.lc);
                }
                //if case 2 or case 4
                if ((rsp.len_out == sizeof(apdu_req.cmd) + 1) || 
                    (apdu_req.lc > 0 && rsp.len_out > apdu_req.lc + sizeof(apdu_req.cmd) + 1))
                {
                    //Le is present.
                    apdu_req.le = rsp.data_out[rsp.len_out];
                    if (0 == apdu_req.le)
                    {
                        apdu_req.le = 256;
                    }
                }
            }
            delay_Ms(200);
            if (0 != (s32Stat = iso7816_exchange(u8Slot, AUTO_GET_RSP, &apdu_req, &rsp)))
            {
                DBG_PRINT("Exchange failed %d!\n", s32Stat);
                rsp.len_out = 0;
                break;
            }
        }
        
        //Deinit struct power down card and.
        iso7816_close(u8Slot);
        DBG_PRINT("Shut down ICC!\n");
    }

}
