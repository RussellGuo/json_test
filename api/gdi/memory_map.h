/*
 * memory_map.h
 */

#ifndef MEMORY_MAP_H_
#define MEMORY_MAP_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <limits.h>


class TMemoryMapFile {
public:
    TMemoryMapFile(const char *file_name): ptr(nullptr), size(0)  {
        ptr = memory_map_of_file(file_name);
    }

    ~TMemoryMapFile() {
        memory_unmap();
        ptr = nullptr;
    }
    const void *Memory() const { return ptr; }
    size_t Size() const { return size; }

    TMemoryMapFile(const TMemoryMapFile &) = delete;

private:
    void *ptr;
    size_t size;

    void *memory_map_of_file(const char *file_name)
    {
        struct stat sb;
        int fd = -1;
        void *ret = nullptr;

        /* prepare */
        fd = open(file_name, O_RDONLY);
        if (fd < 0) {
            perror("open file");
            return nullptr;
        }
        if (fstat(fd, &sb) == -1) {
            perror("Stat file");
            close(fd);
            return nullptr;
        }

        size = static_cast<size_t>(sb.st_size);
        //fprintf(stderr, "file '%s' size %u\n", path, *size);

        ret = mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
        if (ret == MAP_FAILED) {
            perror("memory map");
            close(fd);
            return nullptr;
        }
        close(fd);
        fd = -1;

        return ret;
    }
    bool memory_unmap()
    {
        if (ptr == nullptr) {
            return true;
        }

        if ( munmap(ptr, size) != 0) {
            perror("memory unmap");
            return false;
        }
        return true;
    }
};


#endif /* MEMORY_MAP_H_ */
