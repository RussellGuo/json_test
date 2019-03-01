
#ifndef TIMER_API_H
#define TIMER_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/timerfd.h>

// 定时器回调函数。第一个参数是定时器id，时createTimer或createSimpleTimer的返回值。
// 第二个参数是这个定时器从上次被回调后发生了多少次超时。不感兴趣的忽略掉即可。
typedef int timer_id_t;
typedef void (*timer_callback_t)(timer_id_t id, uint64_t timeout_count);

// 创建简单定时器。返回值大于0代表正常。小于0表示有问题
// msec是超时的毫秒数；one_shot是是否只执行一次的意思；callback则是超时时候的回调函数指针
timer_id_t createSimpleTimer(uint32_t msec, bool one_shot, timer_callback_t callback);
// 对于已经创建的timer，可以修改其超时时间和是否只执行一次。返回值表示是否成功。
bool modifySimpleTimer(timer_id_t timer_id, uint32_t msec, bool one_shot);
// 创建的定时器缺省是开启的。这个函数调用可以开关之。返回成功与否。
bool enableTimer(timer_id_t timer_id, bool enabled);
// 删除定时器。返回成功与否。
// one_shot定时器也不会自动删除，需要手工调用此函数。注意，回调函数中调用这个函数是安全的。
bool delTimer(timer_id_t timer_id);

// 这是创建定时器的全功能版本。需要一些预备知识，参见 http://man7.org/linux/man-pages/man2/timerfd_create.2.html
// clockid是类别，会被原样传递给timerfd_create的第一个参数
// flags是标记，会原样传递给timerfd_settime的第二个参数
// itimerspec是首次超时值和后续超时值。
// callback则是超时时候的回调函数指针，不用多说了...
// enabled是创建时是否即刻生效的意思。这个值可以用 enablePollEventFd 调用来后续变更
timer_id_t createTimer(clockid_t clockid, int flags, const struct itimerspec *itimerspec, timer_callback_t callback, bool enabled);
// 更改 标记和超时参数。
bool modifyTimer(timer_id_t timer_id, int flags, const struct itimerspec *itimerspec);



#ifdef __cplusplus
} // extern "C"
#endif
#endif /*TIMER_API_H*/
