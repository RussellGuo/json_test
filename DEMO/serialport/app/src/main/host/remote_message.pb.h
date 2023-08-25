//
// Created by ubuntu on 2023/8/25.
//
#include "pb.h"
#ifndef SERIALPORT_REMOTE_MESSAGE_PB_H
#define SERIALPORT_REMOTE_MESSAGE_PB_H

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
/* 远程调用的错误码 */
typedef enum _remote_call_err_code {
    remote_call_err_code_ok = 0,
    remote_call_err_code_crc_err = 1, /* CRC错 */
    remote_call_err_code_req_type_err = 2, /* 不支持的请求类型 */
    remote_call_err_code_fmt_err = 3, /* 报文格式错（解码失败） */
    remote_call_err_code_no_impl = 4, /* 服务未实现（MCU的服务函数只有通用原型，没有实现代码） */
    remote_call_err_code_srv_not_ready = 5, /* 服务未启动，例如thread没启动起来 */
    remote_call_err_code_parm_err = 6, /* 服务参数错 */
    remote_call_err_code_timeout = 7, /* 服务端超时 */
    remote_call_err_code_comm_err = 8 /* 通讯错 */
} remote_call_err_code;

/* 三态真假值 */
typedef enum _boolean_t {
    boolean_t_failed = 0,
    boolean_t_succeeded = 1,
    boolean_t_unknown = 2
} boolean_t;

/* Struct definitions */
/* 在主机端-MCU端，模仿了登录流程，登录的消息结构如下 */
typedef struct _login_req {
    char username[25];
    char password[12];
} login_req;

/* 返回值的消息结构如下 */
typedef struct _login_res {
    boolean_t status;
} login_res;

/* 在主机端-MCU端，模仿了登出流程。请求内容如下 */
typedef struct _logout_req {
    int32_t dummy;
} logout_req;

/* 返回值消息结构如下 */
typedef struct _logout_res {
    boolean_t status;
} logout_res;

/* 事件示例1，按键 */
typedef struct _key_evt {
    int32_t key_code;
    bool key_action;
} key_evt;

/* 事件示例2，日志（日志归入事件是可以的，后者属于通用机制） */
typedef struct _log_evt {
    int32_t level;
    char msg[80];
} log_evt;

/* 从host发往mcu的消息的总类型 */
typedef struct _to_mcu {
    int32_t seq;
    pb_size_t which_req;
    union {
        login_req login;
        logout_req logout;
    } req;
} to_mcu;

/* 从mcu发到host的数据的总类型，包括req的响应，event和log。log目前算作一种event。 */
typedef struct _from_mcu {
    int32_t seq;
    remote_call_err_code err_code;
    pb_size_t which_res;
    union {
        login_res login;
        logout_res logout;
        key_evt key;
        log_evt log;
    } res;
} from_mcu;


#ifdef __cplusplus
extern "C" {
#endif

/* Helper constants for enums */
#define _remote_call_err_code_MIN remote_call_err_code_ok
#define _remote_call_err_code_MAX remote_call_err_code_comm_err
#define _remote_call_err_code_ARRAYSIZE ((remote_call_err_code)(remote_call_err_code_comm_err+1))

#define _boolean_t_MIN boolean_t_failed
#define _boolean_t_MAX boolean_t_unknown
#define _boolean_t_ARRAYSIZE ((boolean_t)(boolean_t_unknown+1))


#define login_res_status_ENUMTYPE boolean_t


#define logout_res_status_ENUMTYPE boolean_t




#define from_mcu_err_code_ENUMTYPE remote_call_err_code


/* Initializer values for message structs */
#define login_req_init_default                   {"", ""}
#define login_res_init_default                   {_boolean_t_MIN}
#define logout_req_init_default                  {0}
#define logout_res_init_default                  {_boolean_t_MIN}
#define key_evt_init_default                     {0, 0}
#define log_evt_init_default                     {0, ""}
#define to_mcu_init_default                      {0, 0, {login_req_init_default}}
#define from_mcu_init_default                    {0, _remote_call_err_code_MIN, 0, {login_res_init_default}}
#define login_req_init_zero                      {"", ""}
#define login_res_init_zero                      {_boolean_t_MIN}
#define logout_req_init_zero                     {0}
#define logout_res_init_zero                     {_boolean_t_MIN}
#define key_evt_init_zero                        {0, 0}
#define log_evt_init_zero                        {0, ""}
#define to_mcu_init_zero                         {0, 0, {login_req_init_zero}}
#define from_mcu_init_zero                       {0, _remote_call_err_code_MIN, 0, {login_res_init_zero}}

/* Field tags (for use in manual encoding/decoding) */
#define login_req_username_tag                   1
#define login_req_password_tag                   2
#define login_res_status_tag                     1
#define logout_req_dummy_tag                     1
#define logout_res_status_tag                    1
#define key_evt_key_code_tag                     1
#define key_evt_key_action_tag                   2
#define log_evt_level_tag                        1
#define log_evt_msg_tag                          2
#define to_mcu_seq_tag                           1
#define to_mcu_login_tag                         3
#define to_mcu_logout_tag                        4
#define from_mcu_seq_tag                         1
#define from_mcu_err_code_tag                    2
#define from_mcu_login_tag                       3
#define from_mcu_logout_tag                      4
#define from_mcu_key_tag                         201
#define from_mcu_log_tag                         202

/* Struct field encoding specification for nanopb */
#define login_req_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   username,          1) \
X(a, STATIC,   SINGULAR, STRING,   password,          2)
#define login_req_CALLBACK NULL
#define login_req_DEFAULT NULL

#define login_res_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    status,            1)
#define login_res_CALLBACK NULL
#define login_res_DEFAULT NULL

#define logout_req_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    dummy,             1)
#define logout_req_CALLBACK NULL
#define logout_req_DEFAULT NULL

#define logout_res_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UENUM,    status,            1)
#define logout_res_CALLBACK NULL
#define logout_res_DEFAULT NULL

#define key_evt_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    key_code,          1) \
X(a, STATIC,   SINGULAR, BOOL,     key_action,        2)
#define key_evt_CALLBACK NULL
#define key_evt_DEFAULT NULL

#define log_evt_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    level,             1) \
X(a, STATIC,   SINGULAR, STRING,   msg,               2)
#define log_evt_CALLBACK NULL
#define log_evt_DEFAULT NULL

#define to_mcu_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    seq,               1) \
X(a, STATIC,   ONEOF,    MESSAGE,  (req,login,req.login),   3) \
X(a, STATIC,   ONEOF,    MESSAGE,  (req,logout,req.logout),   4)
#define to_mcu_CALLBACK NULL
#define to_mcu_DEFAULT NULL
#define to_mcu_req_login_MSGTYPE login_req
#define to_mcu_req_logout_MSGTYPE logout_req

#define from_mcu_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    seq,               1) \
X(a, STATIC,   SINGULAR, UENUM,    err_code,          2) \
X(a, STATIC,   ONEOF,    MESSAGE,  (res,login,res.login),   3) \
X(a, STATIC,   ONEOF,    MESSAGE,  (res,logout,res.logout),   4) \
X(a, STATIC,   ONEOF,    MESSAGE,  (res,key,res.key), 201) \
X(a, STATIC,   ONEOF,    MESSAGE,  (res,log,res.log), 202)
#define from_mcu_CALLBACK NULL
#define from_mcu_DEFAULT NULL
#define from_mcu_res_login_MSGTYPE login_res
#define from_mcu_res_logout_MSGTYPE logout_res
#define from_mcu_res_key_MSGTYPE key_evt
#define from_mcu_res_log_MSGTYPE log_evt

extern const pb_msgdesc_t login_req_msg;
extern const pb_msgdesc_t login_res_msg;
extern const pb_msgdesc_t logout_req_msg;
extern const pb_msgdesc_t logout_res_msg;
extern const pb_msgdesc_t key_evt_msg;
extern const pb_msgdesc_t log_evt_msg;
extern const pb_msgdesc_t to_mcu_msg;
extern const pb_msgdesc_t from_mcu_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define login_req_fields &login_req_msg
#define login_res_fields &login_res_msg
#define logout_req_fields &logout_req_msg
#define logout_res_fields &logout_res_msg
#define key_evt_fields &key_evt_msg
#define log_evt_fields &log_evt_msg
#define to_mcu_fields &to_mcu_msg
#define from_mcu_fields &from_mcu_msg

/* Maximum encoded size of messages (where known) */
#define from_mcu_size                            108
#define key_evt_size                             13
#define log_evt_size                             92
#define login_req_size                           39
#define login_res_size                           2
#define logout_req_size                          11
#define logout_res_size                          2
#define to_mcu_size                              52

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //SERIALPORT_REMOTE_MESSAGE_PB_H
