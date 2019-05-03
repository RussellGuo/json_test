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

int main(int argc, char *argv[])
{
    switch (argc) {
    case 1:
    {
        const char *dir       = "/system";
        const char *key_file  = "/rsapub.key";
        const char *sign_file = "/system/sigature-all.bin";
        unsigned char sha256[SHA256_DIGEST_LENGTH];
        bool ret = gen_meta_digest_for_dir(dir, sha256);
        for(size_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            printf("%02x", sha256[i]);
        }
        printf("\n");
        fflush(stdout);

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
