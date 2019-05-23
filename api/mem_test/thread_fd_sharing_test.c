#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


static void *thread_proc(void *arg)
{
    int *fd_ptr = arg;
    *fd_ptr = open("/dev/tty", 0);
    sleep(100);
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t threads;
    volatile int fd = -1;
    pthread_create(&threads, NULL, thread_proc, (void *)&fd);
    while (fd < 0) {
        fprintf(stderr, "fd is %d\n", fd);
        sleep(1);
    }
    fprintf(stderr, "fd is %d\n", fd);
    char buf[1];
    ssize_t read_count = read(fd, buf, sizeof (buf));
    if (read_count < 0) {
        perror("read");
    }
    fprintf(stderr, "result is %zd\n", read_count);

    /* Last thing that main() should do */
    pthread_exit(NULL);
    return 0;
}
