
/* 别编辑，自动生成的代码
   生成命令是： gen_pb_rpc_code.py 目标C代码目录 目标Java代码目录

   semantic_api.generated.h，本文件包含了：
      1> host端请求/响应的语义层程序的总入口
      2> host端请求响应中，针对不同业务的各自的入口（其中.c中包含了一个 weak 版本的实现，使得整个程序在业务不全的时候也能编译和工作，但最终还是要有正规的C实现）
      3> MCU发送事件到host的各种evt的入口
    123都有一堆相似的代码，适合自动生成，可以节省人力并减少错误。宏也可以做到但是宏不容易调试

 */
#include "remote_message.pb.h"

// 来自host端的请求的总处理入口。在函数内会根据请求的名称派发到各自的服务函数
// 数据报层，收到报文，解出来to_mcu对象后，通过这个总入口调用各自的函数，之后会把from_mcu打包发回给host
// 返回值是各个子服务函数指定的，会打包在from_mcu后送回给host
remote_call_err_code remote_call_service(const to_mcu *to_mcu_obj, from_mcu *from_mcu_obj);


// 下面是请求/响应函数，针对不同的业务
remote_call_err_code remote_call_service_for_login(const login_req *login_req_obj, login_res *login_res_obj); //业务login的处理函数，被remote_call_service调用
remote_call_err_code remote_call_service_for_logout(const logout_req *logout_req_obj, logout_res *logout_res_obj); //业务logout的处理函数，被remote_call_service调用

bool send_remote_res(from_mcu *from_mcu_obj); // 本函数为所有事件函数提供服务

// 下面是事件函数
bool send_remote_event_for_key(const key_evt *key); // 发送事件到host的函数，供MCU代码调用。程序会打包一个from_mcu对象发送到host
bool send_remote_event_for_log(const log_evt *log); // 发送事件到host的函数，供MCU代码调用。程序会打包一个from_mcu对象发送到host
