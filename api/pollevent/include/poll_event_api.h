
#ifndef POLL_EVENT_API_H
#define POLL_EVENT_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>

// 通用文件描述符多路IO的API描述

typedef uint64_t callback_arg_t;
// 文件描述符对应的设备/网络数据到达时的回调函数类型
// id是发生回调的文件描述符
// arg是当初登记的一个64bit数，多路IO系统帮助登记和传出，不负责解释
typedef void (*callback_t)(int id, uint64_t arg);


// event part

// 告知系统加入监控此文件描述符，并注册相关回调函数。回调时的arg参数就是这个arg。enabled则是是否立刻启动，建议这个值用true。
bool setPollEventFd(int fd, callback_t callback, uint64_t arg, bool enabled);
// 对于已经登记过的fd，启动或者不启动监视。enabled表示启动或不启动
bool enablePollEventFd(int fd, bool enabled);
// 不再关注此fd，从多路IO表中删除
bool delPollEventFd(int fd);

// 轮询一次全部设备，无事件时当前进程等待。有事件则醒来，并执行各路回调。
int PollEventSpinOnce(void);

// 供子线程用的通知调用。一旦调用，主线程的SpinOnce会返回。这个arg值可以传递给主线程。但主线程只会保留最后一个。所以不要依赖这个值。
bool PollEventThreadNotify(uint64_t arg);
// 供主线程调用的，在SpinOnce返回后，可以调用本过程来获得这类通知发生了多少次。次数由返回值表示；最后一次设定的arg值会放置在参数指针指定的地方。
// 不关心返回值的情况下，这个指针放NULL。
uint64_t PollEventFetchThreadNotifyCount(uint64_t *last_thread_notify_value_ptr);


#ifdef __cplusplus
} // extern "C"
#endif
#endif /*POLL_EVENT_API_H*/
