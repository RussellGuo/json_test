#include "sign_verify.h"

#include <stdio.h>

#include "mbedtls/sha512.h"

static void sha512_demo(void);

void sign_verify_demo(void)
{
    sha512_demo();
}

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
