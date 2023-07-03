/*
 *Copyright (C) 2018-2019,Megahunt Tech.Co,Ltd
 *All rights reserved
 *
 *FileName      :mh_sha.h
 *Description   :This file is a header file of sha alogrithm module, including 
 *               defination of secure hash alogrithm and secure hash rand copy alogrithm.
 *               and enum of SHA_ModeDef,return value macro of SHA.
 *            
 *Version       :1.1
 *Author        :Mick
 *Date          :2019.01.11
 *Modify        :none
 */

#ifndef __MH_SHA_H
#define __MH_SHA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "mh_crypt.h"


#define MH_RET_SHA_INIT                         (0x576C39AA)
#define MH_RET_SHA_PARAM_ERR                    ((MH_RET_SHA_INIT + 0x01))
#define MH_RET_SHA_BUSY                         ((MH_RET_SHA_INIT + 0x02))
#define MH_RET_SHA_MODE_ERR                     ((MH_RET_SHA_INIT + 0x03))
#define MH_RET_SHA_SUCCESS                      ((MH_RET_SHA_INIT + 0x0100))


typedef struct
{
    uint32_t au32Total[2];
    uint8_t au8Buffer[64];
    uint32_t au32State[5];
}MHSHA1_Context;

typedef struct
{
    uint32_t au32Total[2];
    uint8_t  au8Buffer[64];
    uint32_t au32State[8];
}MHSHA256_Context;

typedef struct
{
    uint32_t au32Total[2];
    uint8_t  au8Buffer[128];
    uint32_t au32State[16];
}MHSHA512_Context;

typedef enum
{
    MHSHA1 = 0,
    MHSHA224 = 1,
    MHSHA256 = 2,
    MHSHA384 = 3,
    MHSHA512 = 4
}MHSHA_MOD_TYPE;

uint32_t MHSHA1_Starts(MHSHA1_Context *pCtx);
uint32_t MHSHA1_Update(MHSHA1_Context *pCtx, const uint8_t *pu8Input, uint32_t u32InputLen);
uint32_t MHSHA1_Finish(MHSHA1_Context *pCtx, uint8_t au8Digest[20]);

uint32_t MHSHA224_Starts(MHSHA256_Context *pCtx);
uint32_t MHSHA224_Update(MHSHA256_Context *pCtx,const uint8_t *pu8Input,uint32_t u32Ilen);
uint32_t MHSHA224_Finish(MHSHA256_Context *pCtx, uint8_t au8Digest[28]);

uint32_t MHSHA256_Starts(MHSHA256_Context *pCtx);
uint32_t MHSHA256_Update(MHSHA256_Context *pCtx, const uint8_t *pu8Input, uint32_t u32InputLen);
uint32_t MHSHA256_Finish(MHSHA256_Context *pCtx, uint8_t au8Digest[32]);

uint32_t MHSHA384_Starts(MHSHA512_Context *pCtx);
uint32_t MHSHA384_Update(MHSHA512_Context *pCtx,const uint8_t *pu8Input,uint32_t u32Ilen);
uint32_t MHSHA384_Finish(MHSHA512_Context *pCtx, uint8_t au8Digest[64]);

uint32_t MHSHA512_Starts(MHSHA512_Context *pCtx);
uint32_t MHSHA512_Update(MHSHA512_Context *pCtx, const uint8_t *pu8Input, uint32_t u32InputLen);
uint32_t MHSHA512_Finish(MHSHA512_Context *pCtx, uint8_t au8Digest[64]);

/**
  * @method MH_Sha
  * @brief  Hash Alogrithm
  * @param  mode        :SHA mode: SHA_160/SHA_224/SHA_256/SHA_384/SHA_512
  * @param  output      :output data buffer
  * @param  input       :input data buffer
  * @param  ibytes      :size of input data
  * @retval             :SHA_TYPE_ERR or SHA_CRYPT_BUSY
  */
uint32_t MHSHA_Sha(MHSHA_MOD_TYPE ShaType, const uint8_t *pu8Input, uint32_t u32Ibytes, uint8_t *pu8Output, uint32_t u32Outlen);

#ifdef __cplusplus
}
#endif  

#endif
