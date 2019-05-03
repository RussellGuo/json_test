/*
 * Copyright (c) 2019 Huaqin Telecom Technology Co., Ltd. All rights reserved.
 * Created on 2019-5-1
 * Author:Guo Qiang
 * Version: 1.0 alpha
 * Title: get a directory's digest recursively and verity it's signature
 */

#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include <linux/limits.h>
#include <fcntl.h>
#include <errno.h>

#include <openssl/sha.h>

#include <assert.h>

#include "calc_dir_digest_recursively.h"
#include "../common/rsa_warpper.h"

bool check_signature(const unsigned char *hash, const char *signature_file, const char *pub_key)
{
    unsigned char  encrypted[4098];
    unsigned char decrypted[4098];
    int encrypted_length, decrypted_length;

    FILE *f_sign = fopen(signature_file, "rb");
    if (f_sign == NULL) {
        perror("fopen");
        return false;
    }
    encrypted_length = (int)fread(encrypted, 1, sizeof encrypted, f_sign);
    fclose(f_sign);
    if (encrypted_length != 256) {
        fprintf(stderr, "The encrypted length RSA 2048 should be 256 bytes\n");
        return false;
    }

    decrypted_length = public_decrypt(encrypted, (size_t)encrypted_length, pub_key, decrypted);
    if (decrypted_length != SHA256_DIGEST_LENGTH) {
        fprintf(stderr, "The decrypted length should be 32, a SHA256_DIGEST_LENGTH\n");
        exit(1);
    }
    for(int i = 0; i < decrypted_length; i++) {
        printf("%02x", decrypted[i]);
    }
    printf("\n");
    fflush(stdout);
    if (memcmp(hash, decrypted, SHA256_DIGEST_LENGTH) != 0) {
        fprintf(stderr, "decrypted text and the plain text are mismatch\n");
        exit(1);
    }
    return true;
}

int main(int argc, char *argv[])
{
    switch (argc) {
    case 1:
    {
        const char *dir       = "/system";
        const char *key_file  = "/rsapub.key";
        const char *sign_file = "/system/signature-all.bin";
        unsigned char sha256[SHA256_DIGEST_LENGTH];
        bool ret = gen_meta_digest_for_dir(dir, sha256);
        for(size_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            printf("%02x", sha256[i]);
        }
        printf("\n");
        fflush(stdout);

        bool is_ok = check_signature(sha256, sign_file, key_file);

        break;
    }
    case 2:
    {
        calc_root_recursively(argv[1]);
        break;
    }
    default:
        fprintf(stderr, "Usage: %s [dir]\n", argv[0]);
    }
    return 0;
}
