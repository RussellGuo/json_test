#include "sign_verify.h"

#include <stdio.h>

#include "mbedtls/sha512.h"
#include "mbedtls/pk.h"

static void sha512_demo(void);
static void rsa_demo(void);

void sign_verify_demo(void)
{
    sha512_demo();
    rsa_demo();
}

// mbedtls_pk_parse_public_key
static void sha512_demo(void)
{
    const unsigned char data[] = { '1', '2', '3', '4' };
    unsigned char result[64];
    int ret = mbedtls_sha512_ret(data, sizeof data, result, false);
    printf("sha512 for '1234': ret = %d, result is ", ret);
    for (int i = 0; i < sizeof result; i++) {
        printf("%02x", result[i]);
    }
    printf("\n\r");
}

static unsigned char pub_key_pem_string[] =
    "-----BEGIN PUBLIC KEY-----\r\n"
    "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxo1YWUCtl1RFCz9J9dq/\r\n"
    "mx7QFySWCQbnx8YmZfCdjKXLGiP5pJQ2++Ml1n6cUKasfgeJAF40tE68aI8hv1pW\r\n"
    "BTXFDW2BTnV0owdQQYGXqUBgOG/nBxNxlaqj6VynnXVq1IBZkVZ33VLFN2B8qbFf\r\n"
    "1gY/qzAprexOdagpZ9lMP5ksHbgS1EvohXFpEoBODBQnsZ9gFYsNbqEYMgMFFYfs\r\n"
    "Q6D/iKa9URqGfvNcMTfJmFv9sgOC4S6ZCsXyrJHZm+BRec6xy3zNY8JQtZ0sSDj8\r\n"
    "P5D2s0TYGSpPlBTe/PtLSTqzsozYePrMiPCxqFR1+G2pQPpTRv5Atq92e+HYjsJP\r\n"
    "JQIDAQAB\r\n"
    "-----END PUBLIC KEY-----\r\n"
;

void rsa_demo(void)
{
    mbedtls_pk_context ctx_pk;
    mbedtls_pk_init(&ctx_pk);
    int ret;
    ret = mbedtls_pk_parse_public_key(&ctx_pk, pub_key_pem_string, sizeof pub_key_pem_string);
    mbedtls_rsa_context *rsa = (mbedtls_rsa_context*)ctx_pk.pk_ctx;
    ret = 1;
}
