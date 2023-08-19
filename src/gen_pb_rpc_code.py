import meta_info_from_pb

req_list, res_list, evt_list = meta_info_from_pb.req_list, meta_info_from_pb.res_list, meta_info_from_pb.evt_list

# 我们要生成代码啦！首先我们把请求响应对应的业务名提取出来，建立一个从func->(req信息，res信息)的元组
rpc_func_map = {}
# 下面的处理就是把类型名丢掉后缀当作func
for item in req_list:
    req = item[0][:-4]
    rpc_func_map[req] = item
# 请求好了，加上响应
for item in res_list:
    res = item[0][:-4]
    rpc_func_map[res] = (rpc_func_map[res], item)

evt_map = {}
for item in evt_list:
    evt = item[0][:-4]
    evt_map[evt] = item

del req_list, res_list, evt_list, item, req, res, evt

# 先输出一下头文件。目前还不知道文件名/路径，先打印到终端看看
print(
    '''
/* 别编辑，自动生成的代码
   生成命令是：。。。。。
 */
#include "remote_message.pb.h"
''')
for func in rpc_func_map:
    req_type_name, req_obj_name, req_number, req_type_is_dummy = rpc_func_map[func][0]
    res_type_name, res_obj_name, res_number, res_type_is_dummy = rpc_func_map[func][1]
    print("extern remote_call_err_code remote_call_service_for_%s(const %s *%s, %s *%s);" %
          (func, req_type_name, req_obj_name, res_type_name, res_obj_name))
    pass
for evt in evt_map:
    evt_type_name, evt_obj_name, evt_number, evt_type_is_dummy = evt_map[evt]
    print("extern bool send_remote_event_for_%s(const %s *%s);" %
          (evt, evt_type_name, evt_obj_name))
    pass

# 试一试C文件
print(
    '''
/* 别编辑，自动生成的代码
   生成命令是：。。。。。
 */
#include "上头那个.h"
''')
for func in rpc_func_map:
    req_type_name, req_obj_name, req_number, req_type_is_dummy = rpc_func_map[func][0]
    res_type_name, res_obj_name, res_number, res_type_is_dummy = rpc_func_map[func][1]
    print('''
__attribute__((weak)) remote_call_err_code remote_call_service_for_%s(const %s *%s_req_obj, %s *%s_res_obj) {
    (void)%s_req_obj;
    (void)%s_res_obj;
    return remote_call_err_code_no_impl;
}''' % (func, req_type_name, req_obj_name, res_type_name, res_obj_name, req_obj_name, res_obj_name))
    pass

for evt in evt_map:
    evt_type_name, evt_obj_name, evt_number, evt_type_is_dummy = evt_map[evt]
    print('''
bool send_remote_event_for_%s(const %s *%s) {
    bool ret;
    from_mcu from_mcu_obj = from_mcu_init_zero;
    from_mcu_obj.which_res = from_mcu_%s_tag;
    from_mcu_obj.res.%s = *%s;
    ret = send_remote_res(&from_mcu_obj);
    return ret;
}''' % (evt, evt_type_name, evt_obj_name, evt, evt_obj_name, evt_obj_name))

print('''
remote_call_err_code remote_call_service(const to_mcu *to_mcu_obj, from_mcu *from_mcu_obj) {
    remote_call_err_code err_code;
    switch(to_mcu_obj->which_req) {''')

for func in rpc_func_map:
    req_type_name, req_obj_name, req_number, req_type_is_dummy = rpc_func_map[func][0]
    res_type_name, res_obj_name, res_number, res_type_is_dummy = rpc_func_map[func][1]
    print('''
    case to_mcu_%s_tag:
        err_code = remote_call_service_for_%s(to_mcu_obj->req.%s, from_mcu_obj->req.%s);
        break;
'''%(func, func, req_obj_name, res_obj_name))

print('''
    default:
        err_code = remote_call_err_code_req_type_err;
        break;
    }
    return err_code;
}''')
