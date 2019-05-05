/*
 * Copyright (c) 2019 Huaqin Telecom Technology Co., Ltd. All rights reserved.
 * Created on 2019-5-1
 * Author:Guo Qiang
 * Version: 1.0 alpha
 * Title: get a directory's digest recursively
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void calc_root_recursively(const char *root);
bool gen_meta_digest_for_dir(const char *dir, unsigned char sha256[SHA256_DIGEST_LENGTH], FILE *debug_file);

