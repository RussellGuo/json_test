/*
 * Copyright (c) 2019 Huaqin Telecom Technology Co., Ltd. All rights reserved.
 * Created on 2019-5-2
 * Author:Guo Qiang
 * Version: 1.0 alpha
 * Title: RSA algorithm warpper
 */

#ifndef __RSA_WAPPER_H__
#define __RSA_WAPPER_H__

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define RSA_PADDING_MODE RSA_PKCS1_PADDING

inline static int private_encrypt(unsigned char *data, size_t data_len, const char *key, unsigned char *encrypted)
{
    FILE *f = fopen(key, "rb");
    if (f == NULL) {
        return -1;
    }
    RSA *rsa = PEM_read_RSAPrivateKey(f, NULL, NULL, NULL);
    fclose(f);

    if (rsa == NULL) {
        return -1;
    }

    int result = RSA_private_encrypt((int)data_len, data, encrypted, rsa, RSA_PADDING_MODE);
    return result;
}

inline static int public_decrypt(unsigned char *enc_data, size_t data_len, const char *key, unsigned char *decrypted)
{
    FILE *f = fopen(key, "rb");
    if (f == NULL) {
        return -1;
    }
    RSA *rsa = PEM_read_RSA_PUBKEY(f, NULL, NULL, NULL);
    fclose(f);

    if (rsa == NULL) {
        return -1;
    }

    int result = RSA_public_decrypt((int)data_len, enc_data, decrypted, rsa, RSA_PADDING_MODE);
    return result;
}

#endif // __RSA_WAPPER_H__
