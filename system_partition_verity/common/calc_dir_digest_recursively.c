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

#include "calc_dir_digest_recursively.h"
#include "my_fs_config.h"

static ssize_t sys_readlinkat (int __fd, const char *__restrict __path, char *__restrict __buf, size_t __len)
{
    ssize_t ret = syscall(SYS_readlinkat, __fd, __path, __buf, __len);
    return ret;
}


enum file_type_t {
    file_type_regular,
    file_type_link,
    file_type_dir,
    file_type_others
};

static enum file_type_t calc_hash_sha256_and_mode_info(const char full_path[], int dir_fd, const char *name, char sha256_hex_and_mode_info[], size_t sha256_hex_and_mode_info_max);

#define DIR_FILE_TAB_MAX 1000

static int strcmp_for_qsort(const void *v1, const void *v2)
{
    const char *const *str1 = v1, *const *str2 = v2;
    return strcmp(*str1, *str2);
}

static void calc_dir_recursively(int dir_fd, const char *path)
{
    char *dir_name_tab[DIR_FILE_TAB_MAX];
    size_t dir_name_tab_len = 0;

    DIR *dir = fdopendir(dir_fd);
    if (dir == NULL) {
        perror("fdopendir");
        exit(1);
    }

    memset(dir_name_tab, 0, sizeof (dir_name_tab));
    for (;;) {
        struct dirent *d;

        errno = 0;
        d = readdir(dir);
        if (d == NULL) {
            if (errno) {
                perror("readdir");
                exit(1);
            }
            break;
        }
        char *name = strdup(d->d_name);
        if (name == NULL) {
            fprintf(stderr, "Out of memory\n");
            exit(1);
        }
        if (d->d_type == DT_DIR) {
            if(strcmp(name,".") == 0 || strcmp(name,"..") == 0){
                continue;
            }
        }
        if (dir_name_tab_len > DIR_FILE_TAB_MAX - 1) {
            fprintf(stderr, "Out of memory\n");
            exit(1);
        }
        dir_name_tab[dir_name_tab_len++] = name;
    }
    qsort(dir_name_tab, dir_name_tab_len, sizeof (dir_name_tab[0]), strcmp_for_qsort);


    for (size_t i = 0; i < dir_name_tab_len; i++) {
        char *name = dir_name_tab[i];
        char child_path[PATH_MAX];
        char sha256_hex_and_mode_info[SHA256_DIGEST_LENGTH * 2 + 30];

        snprintf(child_path, sizeof(child_path), "%s%s%s", path ? path : "" , path? "/":"", name);

        enum file_type_t file_type = calc_hash_sha256_and_mode_info(child_path, dir_fd, name, sha256_hex_and_mode_info, sizeof (sha256_hex_and_mode_info));


        printf("%s %s\n", sha256_hex_and_mode_info, child_path);
        if(file_type == file_type_dir){
            int fd = openat(dir_fd, name, O_RDONLY);
            calc_dir_recursively(fd, child_path);
        }
        free(name);
    }

    closedir(dir);
    close(dir_fd);
}


void calc_root_recursively(const char *root)
{
    struct stat stat;
    if (lstat(root, &stat) < 0) {
        perror("lstat");
        exit(1);
    }
    if (!S_ISDIR(stat.st_mode)) {
        fprintf(stderr, "calc_root_recursively for a NON-directory\n");
        exit(1);
    }
    int fd = open(root, O_RDONLY|O_NONBLOCK);
    calc_dir_recursively(fd, NULL);
}


static void SHA256_UpdateWithCheck(SHA256_CTX *c, const void *data, size_t len)
{
    int ret = SHA256_Update(c, data, len);
    if (!ret) {
        fprintf(stderr, "SHA256_Update error\r\n");
        exit(1);
    }
}

static enum file_type_t calc_hash_sha256_and_mode_info(const char full_path[], int dir_fd, const char *name, char sha256_hex_and_mode_info[], size_t sha256_hex_and_mode_info_max)
{
    SHA256_CTX ctx;
    bool ret = !!SHA256_Init(&ctx);
    struct stat stat;
    const char *str_type = "ERR";
    enum file_type_t file_type = file_type_others;

    if (!ret) {
        fprintf(stderr, "SHA256_Init error\r\n");
        exit(1);
    }

    if(fstatat(dir_fd, name, &stat, AT_SYMLINK_NOFOLLOW) < 0) {
        perror("fstatat");
        exit(1);
    }

    // TODO: fit uid/gid

    switch (stat.st_mode & S_IFMT) {
    case S_IFDIR:
        file_type = file_type_dir;
        str_type = "dir";
        break;
    case S_IFLNK:
    {
        char link_path[PATH_MAX + 1];
        str_type = "lnk";
        file_type = file_type_link;
        ssize_t len = sys_readlinkat(dir_fd, name, link_path, PATH_MAX);
        if (len < 0) {
            perror("readlinkat");
            exit(1);
        }
        link_path[len] = '\0';
        SHA256_UpdateWithCheck(&ctx, link_path, (size_t)len);
        break;
    }
    case S_IFREG:
    {
        int fd = openat(dir_fd, name, O_RDONLY);
        if (fd < 0) {
            perror("openat");
            exit(1);
        }

        while (true) {
            unsigned char buf[1024];
            ssize_t len = read(fd, buf, sizeof buf);
            if (len < 0) {
                perror("read");
                exit(1);
            }
            if (len == 0) {
                break;
            }
            SHA256_UpdateWithCheck(&ctx, buf, (size_t)len);
        }
        close(fd);

        str_type = "reg";
        file_type = file_type_regular;
        break;
    }
    default:
        fprintf(stderr, "file's type should be one of directory/regular/link\n");
        exit(1);
    }

    {
        unsigned char sha256[SHA256_DIGEST_LENGTH];
        if (!SHA256_Final(sha256, &ctx)) {
            fprintf(stderr, "SHA256_Final error\r\n");
            exit(1);
        }
        for(size_t i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            sprintf(sha256_hex_and_mode_info + (i * 2), "%02x", sha256[i]);
        }
        sha256_hex_and_mode_info[SHA256_DIGEST_LENGTH * 2] = '\0';
    }

    // fit uid/gid
    {
        unsigned int mode = stat.st_mode;
        unsigned int uid = stat.st_uid;
        unsigned int gid = stat.st_gid;
        my_fs_config(full_path, file_type == file_type_dir, &uid, &gid, &mode);
        sprintf(sha256_hex_and_mode_info + SHA256_DIGEST_LENGTH * 2, " %06o %s %04u %04u", mode, str_type, uid, gid);
    }

    assert(sha256_hex_and_mode_info_max > strlen(sha256_hex_and_mode_info));

    return file_type;
}
