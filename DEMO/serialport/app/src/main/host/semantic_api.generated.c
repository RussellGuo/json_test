
/* 别编辑，自动生成的代码
   详情请参见同名.h文件

 */

#include "semantic_api.generated.h"

// 以下是一些带'weak'属性的子服务程序，是一个占位性质的实现。真实的实现需要其它地方做，而且不能带weak属性。这个属性表示“同名的有正式定义的时候用正式定义，没有的请用我”
// 这些weak函数只简单地返回'本函数并未真实实现'

__attribute__((weak)) remote_call_err_code remote_call_service_for_login(const login_req *login_req_obj, login_res *login_res_obj) {
    (void)login_req_obj;
    (void)login_res_obj;
    return remote_call_err_code_no_impl;
}

__attribute__((weak)) remote_call_err_code remote_call_service_for_logout(const logout_req *logout_req_obj, logout_res *logout_res_obj) {
    (void)logout_req_obj;
    (void)logout_res_obj;
    return remote_call_err_code_no_impl;
}

// 以下是事件函数，可直接使用

bool send_remote_event_for_key(const key_evt *key) {
    bool ret;
    from_mcu from_mcu_obj = from_mcu_init_zero;
    from_mcu_obj.which_res = from_mcu_key_tag;
    from_mcu_obj.res.key = *key;
    ret = send_remote_res(&from_mcu_obj);
    return ret;
}

bool send_remote_event_for_log(const log_evt *log) {
    bool ret;
    from_mcu from_mcu_obj = from_mcu_init_zero;
    from_mcu_obj.which_res = from_mcu_log_tag;
    from_mcu_obj.res.log = *log;
    ret = send_remote_res(&from_mcu_obj);
    return ret;
}


// 以下是总处理入口函数
remote_call_err_code remote_call_service(const to_mcu *to_mcu_obj, from_mcu *from_mcu_obj) {
    remote_call_err_code err_code;

    switch(to_mcu_obj->which_req) {
        case to_mcu_login_tag:
            from_mcu_obj->which_res = from_mcu_login_tag;
            err_code = remote_call_service_for_login(&to_mcu_obj->req.login, &from_mcu_obj->res.login);
            break;

        case to_mcu_logout_tag:
            from_mcu_obj->which_res = from_mcu_logout_tag;
            err_code = remote_call_service_for_logout(&to_mcu_obj->req.logout, &from_mcu_obj->res.logout);
            break;

        default:
            err_code = remote_call_err_code_req_type_err;
            break;
    }
    return err_code;
}
