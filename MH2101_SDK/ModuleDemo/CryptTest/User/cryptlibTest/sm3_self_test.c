#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "mh_sm3.h"
#include "crypt_debug.h"

void SM3_Test()
{
    uint32_t t;
    
    MHSM3_Context Ctx;
   
//    uint8_t buf0[64];
    uint8_t digest[32] = {0};
		
    uint8_t c_sm3_len = 16;
    uint8_t c_sm3_msg[] = {0x01, 0x02, 0x03,0x04, 0x05, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10};
    uint8_t c_sm3_hash[] = {0x25, 0xC9, 0xD6, 0x6E, 0xD5, 0x47, 0x71, 0x4E, 0x98, 0x1B, 0xC8, 0x25, 0xB9, 0x69, 
														0xCC, 0x71, 0xAC, 0xD3, 0x38, 0xF9, 0x0A, 0x4E, 0x0F, 0x75, 0xE7, 0x89, 0xEF, 0x71, 0xC0, 0x92, 0x16, 0x66};
		

    DBG_PRINT("\r\nTest the sm3 function.\n");
//    memset(buf0,'a',sizeof(buf0));
    memset(digest,0,32);
    MHSM3_Cal(digest, c_sm3_msg, c_sm3_len);										
    ouputRes("SM3 Update Hash: ", digest, sizeof(digest));
    t = (!memcmp(c_sm3_hash, digest, sizeof(digest)));
    DBG_PRINT("SM3 Calc Test: ");												
    r_printf(t, "\n");
														
														
    memset(digest,0,32);														
    MHSM3_Starts(&Ctx);
    MHSM3_Update(&Ctx,c_sm3_msg, c_sm3_len);
    MHSM3_Finish(&Ctx,digest);
														
    ouputRes("SM3 Update Hash: ", digest, sizeof(digest));
    t = (!memcmp(c_sm3_hash, digest, sizeof(digest)));
    DBG_PRINT("SM3 Update Test: ");												
    r_printf(t, "\n");	
//    while(1);
}