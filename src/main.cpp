#include <assert.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <set>

#include "crc32c.h"
#include "openssl/md5.h"
#include "openssl/sha.h"

static constexpr size_t total_size = 1024UL * 1024 * 1024;

int main(int, char *[]) {
    uint8_t *data_ptr = new uint8_t[total_size];
    memset(data_ptr, 0, total_size);

    const auto value = crc32c_value(data_ptr, total_size);
    std::cout << "crc32c of 1GB-zero is " << value << std::endl;

    SHA_CTX sha_context;
    uint8_t sha_md[2048] = { 0 };
    bool sha_result = !!SHA1_Init(&sha_context) || !!SHA1_Update(&sha_context, data_ptr, total_size) || SHA1_Final(sha_md, &sha_context);
    std::cout << "sha1 of 1GB-zero return " << sha_result << std::endl;

    MD5_CTX md5_content;
    uint8_t md5_md[2048] = { 0 };
    bool md5_result = !!MD5_Init(&md5_content) || !!MD5_Update(&md5_content, data_ptr, total_size) || MD5_Final(md5_md, &md5_content);
    std::cout << "md4 of 1GB-zero return " << md5_result << std::endl;

    delete[] data_ptr;

    return 0;
}
