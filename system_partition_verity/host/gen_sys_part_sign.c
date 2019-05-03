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
    case 4:
    {
        const char *dir       = argv[1];
        const char *key_file  = argv[2];
        const char *sign_file = argv[3];
        unsigned char sha256[SHA256_DIGEST_LENGTH];
        bool ret = gen_meta_digest_for_dir(dir, sha256);
        for(size_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            printf("%02x", sha256[i]);
        }

        break;
    }
    default:
        fprintf(stderr, "Usage: %s [dir] [ key-file-name] [signature-file]\n", argv[0]);
        exit(1);
    }

    return 0;
}
