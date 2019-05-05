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

#include <cutils/android_reboot.h>

#include "calc_dir_digest_recursively.h"
#include "../common/rsa_warpper.h"

void system_partition_mismatch_process(void)
{
#if !defined(USERDEBUG_BUILD)
    while(1)
    {
        int ret = mkdir("/cache/recovery/", S_IRWXU | S_IRWXG | S_IRWXO);
        if (-1 == ret && (errno != EEXIST)){
            perror("mkdir");
            sleep(10);
            continue;
        }
        int fd = open("/cache/recovery/command", O_WRONLY | O_CREAT, 0777);
        if(fd < 0){
            perror("open /cache/recovery/command for write");
            sleep(10);
            continue;
        }

        const char *data[] = { "--system_check_error\n", "--reason=system_check_error\n" , (const char *)NULL };
        bool wrote_all = true;
        for (const char **ptr = data ; *ptr != (const char *)NULL ; ptr++) {
            size_t len = strlen(*ptr);
            ssize_t wrote = write(fd, *ptr, len + 1);
            if ( (size_t)wrote != len + 1) {
                wrote_all = false;
                break;
            }
        }
        close(fd);

        if (!wrote_all) {
            sleep(10);
            continue;
        }
        sync();
        android_reboot(ANDROID_RB_RESTART2, 0, "recovery");
        sleep(10000000);
    }
#else
    exit(1);
#endif
}

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
        return false;
    }
    for(int i = 0; i < decrypted_length; i++) {
        fprintf(stderr, "%02x", decrypted[i]);
    }
    fprintf(stderr, "\n");
    if (memcmp(hash, decrypted, SHA256_DIGEST_LENGTH) != 0) {
        fprintf(stderr, "decrypted text and the plain text are mismatch\n");
        return false;
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

        int log_fd = open("/data/check_sys_part_sign.log", O_WRONLY|O_CREAT|O_TRUNC, 0777);
        if (log_fd < 0) {
            perror("creat log file");
        } else {
            if (dup2(log_fd, 2) < 0) {
                perror("dup");
            }
            close(log_fd);
            log_fd = -1;
        }

        bool ret = gen_meta_digest_for_dir(dir, sha256, stderr);
        for(size_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            fprintf(stderr, "%02x", sha256[i]);
        }
        fprintf(stderr, "\n");

        bool is_ok = check_signature(sha256, sign_file, key_file);
        fprintf(stderr, "\nCheck the system partition signature %s\n", is_ok ? "succeed" : "FAILED");
        fflush(stderr);
        if (is_ok) {
            return 0;
        }
        system_partition_mismatch_process();

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
