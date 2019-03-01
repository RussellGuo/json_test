#ifndef IPC_CMD_H
#define IPC_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "poll_event_api.h"

// 子任务的入口。进去后可以exec
typedef void (*subtask_proc_t)(void);

struct ipc_task_t;
//在主程序中，子程序有数据返回时的回调函数
// 第一个参数时注册的ipc_task上下文
// 后俩是数据和长度
typedef void (*subtask_notifiy_callback_t)(struct ipc_task_t *ipc_task, const uint8_t msg[], uint16_t len);

// IPC任务的上下文
// 主程序中，登记的ipc管道端和子进程的进程号，用于后续访问和结束任务
// 回调函数是在这个上下文里保存的
struct ipc_task_t {
    int ipc_fd;
    pid_t pid;
    subtask_notifiy_callback_t subtask_notifiy_callback;
};

// 开启一个IPC任务。这个是主程序调用的
bool start_subtask(struct ipc_task_t *ipc_task, subtask_proc_t proc);
// 发送数据给子进程。这个是主程序调用的
bool send_ipc_cmd(struct ipc_task_t *ipc_task, const void *msg, uint16_t msg_len);

// 子进程判断是否有新命令到达。msec是毫秒数，等待时间。可以设为0
bool has_ipc_cmd_from_caller(int msec);
// 汇报结果给主进程，只能在子进程中调用
bool send_ipc_reply(const void *msg, uint16_t msg_len);
// 接受来自主进程的命令；没有命令则休眠等待。
int recv_ipc_cmd(void *msg, uint16_t max_msg_len);

// 主进程关闭子进程的任务后，需要执行这个清除僵尸进程，并清除其它占用资源
// wait subtask finished
void stop_subtask(struct ipc_task_t *ipc_task);

#ifdef __cplusplus
} // extern "C"
#endif
#endif /*IPC_CMD_H*/
