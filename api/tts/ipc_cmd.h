#ifndef IPC_CMD_H
#define IPC_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

typedef void (*subtask_proc_t)(void);

// proc: subtask function entry
// ret value: true/false
// ipc_fd keeps for maintask if >= 0; < 0 if error occurred.
// pid is the ID of subtask
// ipc_fd and pid should be kept, they'll be used at stop_subtask
// a process will be fork after ipc_fd created
bool start_subtask(subtask_proc_t proc, int *ipc_fd, pid_t *pid);


bool has_msg_from_remote(int ipc_fd);
bool send_msg_to_remote(int ipc_fd, const void *msg, uint16_t msg_len, pid_t sub_pid);
int recv_msg_from_remote(int ipc_fd, void *msg, uint16_t max_msg_len);

// wait subtask finished
void stop_subtask(int ipc_fd, pid_t pid);

void enable_ipc_signal(bool enable);
#define SIG_MAIN_TO_SUBTASK SIGUSR1
#define SIG_SUB_TO_MAINTASK SIGUSR2

#ifdef __cplusplus
} // extern "C"
#endif
#endif /*IPC_CMD_H*/
