#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include <poll_event_api.h>

namespace {


void *thread_func(void *)
{
    for (uint64_t i = 0;; ++i) {
        fprintf(stderr, "Thread 1 executing, i:%u\r\n", unsigned(i));
        sleep(5);
        PollEventThreadNotify(i);
    }
    return nullptr;
}

}

void startDemoThread(void)
{
    pthread_t pthread;
    pthread_create(&pthread, nullptr, thread_func, nullptr);
}

