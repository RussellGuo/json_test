#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "mh_sm1.h"
#include "mh_rand.h"
#include "crypt_debug.h"


static const uint8_t cau8Plain[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
static const uint8_t cau8IV[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

static const uint8_t cu8Skey128[16] = {0x19, 0x1A, 0x4E, 0xF3, 0x67, 0xEC, 0xE2, 0x81, 0xC9, 0x03, 0xC4, 0x6C, 0x23, 0x33, 0x3C, 0x2A};
static const uint8_t cu8Ekey128[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
static const uint8_t cu8Akey128[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

static const uint8_t cu8Skey192[16] = {0xa2,0x91,0xb3,0xeb,0xa4,0xed,0x22,0x5f,0x1c,0xea,0xa6,0x07,0x62,0x8f,0xb2,0x36};
static const uint8_t cu8Ekey192[16] = {0x40,0xBB,0x12,0xDD,0x6A,0x82,0x73,0x86,0x7F,0x35,0x29,0xD3,0x54,0xB4,0xA0,0x26};
static const uint8_t cu8Akey192[16] = {0x0C,0x90,0xE1,0x5A,0x4F,0x92,0x36,0xB2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static const uint8_t cu8Skey256[16] = {0xa2,0x91,0xb3,0xeb,0xa4,0xed,0x22,0x5f,0x1c,0xea,0xa6,0x07,0x62,0x8f,0xb2,0x36};
static const uint8_t cu8Ekey256[16] = {0x40,0xBB,0x12,0xDD,0x6A,0x82,0x73,0x86,0x7F,0x35,0x29,0xD3,0x54,0xB4,0xA0,0x26};
static const uint8_t cu8Akey256[16] = {0x0C,0x90,0xE1,0x5A,0x4F,0x92,0x36,0xB2,0x12,0x34,0x56,0x78,0x12,0x34,0x56,0x78};


static void SM1Data128_Test()
{
    uint32_t u32KeyLen, t;
    uint8_t au8Key[32 + 16 + 16], au8IV[16];
    MH_SYM_CRYPT_CALL callSm1;
    uint8_t cu8Cipher128[16] = {0};
    uint8_t au8Out[16] = {0};
	uint32_t u32Ret = 0;
	
    memset(au8Key, 0, sizeof(au8Key));
    u32KeyLen = 0;
    memcpy(au8Key, cu8Skey128, 16);
    u32KeyLen += 16;
    memcpy(au8Key + u32KeyLen, cu8Ekey128, 16);
    u32KeyLen += 16;
    memcpy(au8Key + u32KeyLen, cu8Akey128, 16);

    //128bit ECB encrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher128;
    callSm1.u32OutLen = sizeof(cu8Cipher128);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    u32Ret = MHSM1_EncDec(&callSm1);
	if(MH_RET_SM1_SUCCESS != u32Ret)
	{
		DBG_PRINT("SM1Data128_Test ECB Enc Error!\n");
	}
    //128bit ECB decrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cu8Cipher128;
    callSm1.u32InLen = sizeof(cu8Cipher128);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    u32Ret = MHSM1_EncDec(&callSm1);
	if(MH_RET_SM1_SUCCESS != u32Ret)
	{
		DBG_PRINT("SM1Data128_Test ECB Dec Error!\n");
	}
	ouputRes("cau8Plain:", au8Out, sizeof(au8Out));
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher128)));
    r_printf(t, "SM1 128 ECB EncDecrypt Test\n");
	

    //128bit CBC encrypt
    memset(au8IV, 0, sizeof(au8IV));
	memcpy(au8IV, cau8IV, 16);
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher128;
    callSm1.u32OutLen = sizeof(cu8Cipher128);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    u32Ret = MHSM1_EncDec(&callSm1);
	if(MH_RET_SM1_SUCCESS != u32Ret)
	{
		DBG_PRINT("SM1Data128_Test CBC Enc Error!\n");
	}


    //128bit CBC decrypt
    memset(au8IV, 0, sizeof(au8IV));
	memcpy(au8IV, cau8IV, 16);
    callSm1.pu8In = (uint8_t *)cu8Cipher128;
    callSm1.u32InLen = sizeof(cu8Cipher128);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_128 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_8;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    u32Ret = MHSM1_EncDec(&callSm1);
	if(MH_RET_SM1_SUCCESS != u32Ret)
	{
		DBG_PRINT("SM1Data128_Test CBC Dec Error!\n");
	}
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher128)));
    r_printf(t, "SM1 128 CBC EncDecrypt Test\n");
}

static void SM1Data192_Test()
{
    uint32_t u32KeyLen, t;
    uint8_t au8Key[32 + 16 + 16], au8IV[16];//, au8Out[16];
    MH_SYM_CRYPT_CALL callSm1;
    uint8_t cu8Cipher192[16] = {0};
    uint8_t au8Out[16] = {0};
	uint32_t u32Ret = 0;
		
    u32KeyLen = 0;
    memcpy(au8Key, cu8Skey192, 16);
    u32KeyLen += 16;
    memcpy(au8Key + u32KeyLen, cu8Ekey192, 16);

    u32KeyLen += 16;
    memcpy(au8Key + u32KeyLen, cu8Akey192, 16);
	
	
    //192bit ECB encrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher192;
    callSm1.u32OutLen = sizeof(cu8Cipher192);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_192 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_10;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    u32Ret = MHSM1_EncDec(&callSm1);
	if(MH_RET_SM1_SUCCESS != u32Ret)
	{
		DBG_PRINT("SM1Data192_Test ECB Enc Error!\n");
	}

    //192bit ECB decrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cu8Cipher192;
    callSm1.u32InLen = sizeof(cu8Cipher192);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_192 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_10;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    u32Ret = MHSM1_EncDec(&callSm1);
	if(MH_RET_SM1_SUCCESS != u32Ret)
	{
		DBG_PRINT("SM1Data192_Test ECB Dec Error!\n");
	}
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher192)));
    r_printf(t, "SM1 192 ECB EncDecrypt Test\n");
    
	
    //192bit CBC encrypt
    memset(au8IV, 0, sizeof(au8IV));
	memcpy(au8IV, cau8IV, 16);
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher192;
    callSm1.u32OutLen = sizeof(cu8Cipher192);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_192 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_10;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    u32Ret = MHSM1_EncDec(&callSm1);
	if(MH_RET_SM1_SUCCESS != u32Ret)
	{
		DBG_PRINT("SM1Data192_Test CBC Enc Error!\n");
	}

    //192bit CBC decrypt
    memset(au8IV, 0, sizeof(au8IV));
	memcpy(au8IV, cau8IV, 16);
    callSm1.pu8In = (uint8_t *)cu8Cipher192;
    callSm1.u32InLen = sizeof(cu8Cipher192);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_192 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_10;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);

    u32Ret = MHSM1_EncDec(&callSm1);
	if(MH_RET_SM1_SUCCESS != u32Ret)
	{
		DBG_PRINT("SM1Data192_Test CBC Dec Error!\n");
	}
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher192)));
    r_printf(t, "SM1 192 CBC EncDecrypt Test\n");
}

static void SM1Data256_Test()
{
    uint32_t u32KeyLen, t;
    uint8_t au8Key[32 + 16 + 16], au8IV[16];//, au8Out[16];
    MH_SYM_CRYPT_CALL callSm1;
    uint8_t cu8Cipher256[16] = {0};
    uint8_t au8Out[16] = {0};
	uint32_t u32Ret = 0;

    u32KeyLen = 0;
    memcpy(au8Key, cu8Skey256, 16);
    u32KeyLen += 16;
    memcpy(au8Key + u32KeyLen, cu8Ekey256, 16);
    u32KeyLen += 16;
    memcpy(au8Key + u32KeyLen, cu8Akey256, 16);

    //256bit ECB encrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher256;
    callSm1.u32OutLen = sizeof(cu8Cipher256);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    u32Ret = MHSM1_EncDec(&callSm1);
	if(MH_RET_SM1_SUCCESS != u32Ret)
	{
		DBG_PRINT("SM1Data256_Test CBC Dec Error!\n");
	}

    //256bit ECB decrypt
    memset(au8IV, 0, sizeof(au8IV));
    callSm1.pu8In = (uint8_t *)cu8Cipher256;
    callSm1.u32InLen = sizeof(cu8Cipher256);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    u32Ret = MHSM1_EncDec(&callSm1);
	if(MH_RET_SM1_SUCCESS != u32Ret)
	{
		DBG_PRINT("SM1Data256_Test CBC Dec Error!\n");
	}
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher256)));
    r_printf(t, "SM1 256 ECB EncDecrypt Test\n");
	
	
	
    //256bit CBC encrypt
    memset(au8IV, 0, sizeof(au8IV));
	memcpy(au8IV, cau8IV, 16);
    callSm1.pu8In = (uint8_t *)cau8Plain;
    callSm1.u32InLen = sizeof(cau8Plain);
    callSm1.pu8Out = cu8Cipher256;
    callSm1.u32OutLen = sizeof(cu8Cipher256);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_ENCRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    u32Ret = MHSM1_EncDec(&callSm1);
	if(MH_RET_SM1_SUCCESS != u32Ret)
	{
		DBG_PRINT("SM1Data256_Test CBC Dec Error!\n");
	}

    //256bit CBC decrypt
    memset(au8IV, 0, sizeof(au8IV));
	memcpy(au8IV, cau8IV, 16);
    callSm1.pu8In = (uint8_t *)cu8Cipher256;
    callSm1.u32InLen = sizeof(cu8Cipher256);
    callSm1.pu8Out = au8Out;
    callSm1.u32OutLen = sizeof(au8Out);
    callSm1.pu8IV = au8IV;
    callSm1.pu8Key = au8Key;
    callSm1.u16Opt = MH_SM1_OPT_MODE_DECRYPT | MH_SM1_OPT_KEY_256 | MH_SM1_OPT_BLK_ECB | MH_SM1_OPT_ROUND_12;
    callSm1.u32Crc = MHCRC_CalcBuff(0xffff, &callSm1, sizeof(MH_SYM_CRYPT_CALL)-4);
    u32Ret = MHSM1_EncDec(&callSm1);
	if(MH_RET_SM1_SUCCESS != u32Ret)
	{
		DBG_PRINT("SM1Data256_Test CBC Dec Error!\n");
	}
    t = (!memcmp(cau8Plain, au8Out, sizeof(cu8Cipher256)));
    r_printf(t, "SM1 256 CBC EncDecrypt Test\n");
	
}

static void SM1_FixedTest(void)
{
    SM1Data128_Test();
    SM1Data192_Test();
    SM1Data256_Test();
}

void SM1_Test(void)
{
	SM1_FixedTest();
}

