#ifndef IPC_CMD_H
#define IPC_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include "poll_event_api.h"

typedef void (*subtask_proc_t)(void);

struct ipc_task_t;
typedef void (*subtask_notifiy_callback_t)(struct ipc_task_t *ipc_task, const uint8_t msg[], uint16_t len);
struct ipc_task_t {
    int ipc_fd;
    pid_t pid;
    subtask_notifiy_callback_t subtask_notifiy_callback;
};

bool start_subtask(struct ipc_task_t *ipc_task, subtask_proc_t proc, subtask_notifiy_callback_t subtask_notifiy_callback);

bool has_ipc_cmd_from_caller(int msec);
bool send_ipc_cmd(struct ipc_task_t *ipc_task, const void *msg, uint16_t msg_len);
bool send_ipc_reply(const void *msg, uint16_t msg_len);
int recv_ipc_cmd(void *msg, uint16_t max_msg_len);

// wait subtask finished
void stop_subtask(struct ipc_task_t *ipc_task);

#ifdef __cplusplus
} // extern "C"
#endif
#endif /*IPC_CMD_H*/
