#ifndef __MH_CRYPT_H
#define __MH_CRYPT_H

#ifdef __cplusplus
 extern "C" {
#endif
     
#include <stdint.h>

#define MH_ROUND_DOWN(a,s)       ((uint32_t)(a)&~((s)-1))
#define MH_ROUND_UP(a,s)         (((uint32_t)(a)+((s)-1))&~((s)-1))

void MHCRYPT_SYMCITClear(void);

void MHCRYPT_ASYMCITClear(void);

void MHCRYPT_SetBusKey(uint32_t pu32Key[2]);

void MHCRYPT_CramClear(void);

void MHCRYPT_CramKeySet(uint32_t pu32Key[2]);

void MHCRYPT_CramKeyReset(void);

void MHCRYPT_SecureEnable(void);

void MHCRYPT_SecureDisable(void);

typedef struct
{
    uint8_t *pu8In;
    uint8_t *pu8Out;
    uint8_t *pu8IV;
    uint8_t *pu8IVout;
    uint8_t *pu8Key;
    uint32_t u32InLen;
    uint32_t u32OutLen;
    uint16_t u16Opt;
    uint32_t u32Crc;
} MH_SYM_CRYPT_CALL;


#define MH_SSF33_LOCK_EN    (BIT6)
#define MH_SM1_LOCK_EN      (BIT5)
#define MH_SM4_LOCK_EN      (BIT4)
#define MH_SM3_LOCK_EN      (BIT3)
#define MH_SM2_LOCK_EN      (BIT2)
#define MH_AES_LOCK_EN      (BIT1)     
#define MH_DES_LOCK_EN      (BIT0)

void MHCRYPT_LockEnable(uint32_t CryptLock);

#ifdef __cplusplus
}
#endif

#endif // __MH_CRYPT_H
