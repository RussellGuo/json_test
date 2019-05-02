#define _GNU_SOURCE 1

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

#ifdef ANDROID
#include <private/android_filesystem_config.h>
#endif

void my_fs_config(const char *path, int dir, unsigned *uid, unsigned *gid, unsigned *mode)
{
    char full_path[PATH_MAX];
    uint64_t capabilities;
    sprintf(full_path, "/system/%s", path);
    fs_config(full_path, dir, uid, gid, mode, &capabilities);
}
