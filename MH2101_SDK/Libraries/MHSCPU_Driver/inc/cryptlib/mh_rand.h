/* rand.h - header file for rand.c
 */

#ifndef __MH_RAND_H
#define __MH_RAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mh_crypt.h"




uint32_t mh_rand_check(void *rand, uint32_t bytes);

#define MH_RET_RAND_INIT                         (0x562F79AA)
#define MH_RET_RAND_PARAM_ERR                    (MH_RET_RAND_INIT + 1)
#define MH_RET_RAND_FAILURE                      (MH_RET_RAND_INIT + 2)
#define MH_RET_RAND_LEN_ERR                      (MH_RET_RAND_INIT + 3)
#define MH_RET_RAND_CHECK_FAILURE		         (MH_RET_RAND_INIT + 4)
#define MH_RET_RAND_CHECK_DATA_LENGTH_ERROR		 (MH_RET_RAND_INIT + 5)
#define MH_RET_RAND_SUCCESS             (MH_RET_RAND_INIT + 0X0100)
#define MH_RET_RAND_CHECK_SUCCESS		(MH_RET_RAND_SUCCESS)


#define MH_RAND_BUFFER_OK               ((uint16_t)(0xA5A5UL))

uint32_t MHRAND_Init(void *pu32Buff, uint32_t u32BuffSize);
uint32_t MHRAND_Trand(void *pvDst, uint32_t u32Len);
uint32_t MHRAND_Prand(void *pvDst, uint32_t u32Len);
uint32_t MHCRC_CalcBuff(uint16_t u16CrcInit, void *pvData, uint32_t u32Size);
uint32_t MHRAND_UpdatePSeed(void);

#ifdef __cplusplus
}
#endif

#endif


