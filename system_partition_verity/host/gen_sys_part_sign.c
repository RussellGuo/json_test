/*
 * Copyright (c) 2019 Huaqin Telecom Technology Co., Ltd. All rights reserved.
 * Created on 2019-5-1
 * Author:Guo Qiang
 * Version: 1.0 alpha
 * Title: get a directory's digest recursively and genarates it's signature
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

int main(int argc, char *argv[])
{
    switch (argc) {
    case 1:
    case 2:
    {
        const char *dir = argc == 2 ? argv[1] : "out/target/product/sl8521e_3h10/system";
        calc_root_recursively(dir);
        break;
    }
    case 5:
    {
        const char *dir           = argv[1];
        const char *pri_key_file  = argv[2];
        const char *pub_key_file  = argv[3];
        const char *sign_file     = argv[4];

        unsigned char sha256[4098];
        memset(sha256, 0, sizeof (sha256));

        bool ret = gen_meta_digest_for_dir(dir, sha256);
        for(size_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            printf("%02x", sha256[i]);
        }
        printf("\n");

        unsigned char  encrypted[4098];
        unsigned char decrypted[4098];
        int encrypted_length, decrypted_length;

        encrypted_length= private_encrypt(sha256, SHA256_DIGEST_LENGTH, pri_key_file, encrypted);
        if (encrypted_length != 256) {
            fprintf(stderr, "The encrypted length RSA 2048 should be 256 bytes\n");
            exit(1);
        }

        decrypted_length = public_decrypt(encrypted, (size_t)encrypted_length, pub_key_file, decrypted);
        if (decrypted_length != SHA256_DIGEST_LENGTH) {
            fprintf(stderr, "The decrypted length should be 32, a SHA256_DIGEST_LENGTH\n");
            exit(1);
        }
        for(size_t i = 0; i < decrypted_length; i++) {
            printf("%02x", decrypted[i]);
        }
        printf("\n");
        if (memcmp(sha256, decrypted, SHA256_DIGEST_LENGTH) != 0) {
            fprintf(stderr, "decrypted text and the plain text are mismatch\n");
            exit(1);
        }

        FILE *f_sign = fopen(sign_file, "wb");
        if (f_sign == NULL) {
            perror("fopen");
            exit(1);
        }
        size_t written = fwrite(encrypted, 1, encrypted_length, f_sign);
        if (written != encrypted_length) {
            fprintf(stderr, "Writing the signature error\n");
            exit(1);
        }
        fclose(f_sign);

        break;
    }
    default:
        fprintf(stderr, "Usage: %s [dir] [ key-file-name] [signature-file]\n", argv[0]);
        exit(1);
    }

    return 0;
}
