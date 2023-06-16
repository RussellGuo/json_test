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

static constexpr size_t TOTAL_SIZE = size_t(1) * 10 * 1024 * 1024 * 1024;

static void throw_runtime(const char *reson) {
    throw std::runtime_error(std::string(reson) + ": " + strerror(errno));
}

int main(int, char *[]) {
    int fd = open("mmap", O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd < 0) {
        throw_runtime("mmap failed");
    }
    auto ptr = mmap(nullptr, TOTAL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        throw_runtime("mmap failed");
    }
    auto ret = lseek(fd, TOTAL_SIZE - 1, SEEK_CUR);
    if (ret < 0) {
        throw_runtime("lseek");
    }
    ret = write(fd, "", 1);
    if (ret < 0) {
        throw_runtime("write");
    }
    close(fd);
    char *p = (char *)ptr;
    p[1024] = 1;
    printf("ptr: %p\n", ptr);
    return 0;
}
