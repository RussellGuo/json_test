#include "sign_verify.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/sha512.h"
#include "mbedtls/pk.h"

#define CHECK_RESULT \
    if (ret) {\
        break; \
    } else

/*
    用途： 签名验证。
    说明： 签名端需要对数据签名，签名用私钥， 规格是2048的RSA算法，采用PKSC1方案填充，摘要则使用SHA512
    参数rsa_public_key_pem_string，pem格式的公钥字串
    参数origin_data/origin_size，被签名对象以及长度
    参数sign_data， 签名本身
    返回值： true表示签名通过，反之反是
 */
bool sign_verify_sha512_rsa2048_pkcs1_padding(
    const uint8_t *rsa_public_key_pem_string,
    const uint8_t *origin_data,
    size_t origin_size,
    const uint8_t sign_data[RSA2048_RESULT_LEN]
)
{
    int ret;

    mbedtls_pk_context ctx_pk;
    mbedtls_rsa_context *rsa;
    uint8_t sha512_value[SHA512_RESULT_LEN] = {0};
    uint8_t rsa2048_decrypted[RSA2048_RESULT_LEN] = {0};

    mbedtls_pk_init(&ctx_pk);
    do {
        ret = mbedtls_pk_parse_public_key(&ctx_pk, rsa_public_key_pem_string, strlen((const char *)rsa_public_key_pem_string) + 1);
        CHECK_RESULT;

        rsa = (mbedtls_rsa_context*)ctx_pk.pk_ctx;
        mbedtls_rsa_set_padding(rsa, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA512);
        ret = mbedtls_rsa_check_pubkey(rsa);
        CHECK_RESULT;

        ret = mbedtls_rsa_public(rsa, sign_data, rsa2048_decrypted);
        CHECK_RESULT;

        ret = mbedtls_sha512_ret(origin_data, origin_size, sha512_value, false);
        CHECK_RESULT;

        ret = memcmp(rsa2048_decrypted + RSA2048_RESULT_LEN - SHA512_RESULT_LEN, sha512_value, SHA512_RESULT_LEN);
        CHECK_RESULT;
    } while (false);
    

    return ret == 0;
}
