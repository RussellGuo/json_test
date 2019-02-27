
#ifndef POLL_EVENT_API_H
#define POLL_EVENT_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint64_t callback_arg_t;
typedef void (*callback_t)(int id, uint64_t arg);


// event part

bool setPollEventFd(int fd, callback_t callback, uint64_t arg, bool enabled);
bool enablePollEventFd(int fd, bool enabled);
bool delPollEventFd(int fd);

int PollEventSpinOnce(void);
uint64_t PollEventFetchThreadNotifyCount(uint64_t *last_thread_notify_value_ptr);
bool PollEventThreadNotify(uint64_t arg);


#ifdef __cplusplus
} // extern "C"
#endif
#endif /*POLL_EVENT_API_H*/
