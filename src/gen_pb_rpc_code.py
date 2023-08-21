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


def print_mcu_c_code(dir):
    ''' 根据pb的内容生成mcu的C代码，输出路径在 dir，文件名是约定好的 semantic_api.[ch]'''
    h_file = open(dir + '/semantic_api.h', "w")
    c_file = open(dir + '/semantic_api.c', "w")

    # 先输出一下头文件。目前还不知道文件名/路径，先打印到终端看看
    print('''
/* 别编辑，自动生成的代码
   生成命令是： gen_pb_rpc_code.py 目标C代码目录 目标Java代码目录

   semantic_api.h，本文件包含了：
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
''', file=h_file)

    print("\n// 下面是请求/响应函数，针对不同的业务", file=h_file)
    for func in rpc_func_map:
        req_type_name, req_obj_name, req_number, req_type_is_dummy = rpc_func_map[func][0]
        res_type_name, res_obj_name, res_number, res_type_is_dummy = rpc_func_map[func][1]
        print("remote_call_err_code remote_call_service_for_%s(const %s *%s, %s *%s); //业务%s的处理函数，被remote_call_service调用" %
              (func, req_type_name, req_obj_name, res_type_name, res_obj_name, func), file=h_file)
        pass

    print("\n// 下面是事件函数", file=h_file)
    for evt in evt_map:
        evt_type_name, evt_obj_name, evt_number, evt_type_is_dummy = evt_map[evt]
        print("bool send_remote_event_for_%s(const %s *%s); // 发送事件到host的函数，供MCU代码调用。程序会打包一个from_mcu对象发送到host" %
              (evt, evt_type_name, evt_obj_name), file=h_file)
        pass

    # 试一试C文件
    print('''
/* 别编辑，自动生成的代码
   详情请参见同名.h文件

 */

#include "semantic_api.h"
''', file=c_file)

    print("// 以下是一些带'weak'属性的子服务程序，是一个占位性质的实现。真实的实现需要其它地方做，而且不能带weak属性。这个属性表示“同名的有正式定义的时候用正式定义，没有的请用我”", file=c_file)
    print("// 这些weak函数只简单地返回'本函数并未真实实现'\n", file=c_file)

    for func in rpc_func_map:
        req_type_name, req_obj_name, req_number, req_type_is_dummy = rpc_func_map[func][0]
        res_type_name, res_obj_name, res_number, res_type_is_dummy = rpc_func_map[func][1]
        print('''__attribute__((weak)) remote_call_err_code remote_call_service_for_%s(const %s *%s_req_obj, %s *%s_res_obj) {
    (void)%s_req_obj;
    (void)%s_res_obj;
    return remote_call_err_code_no_impl;
}
''' % (func, req_type_name, req_obj_name, res_type_name, res_obj_name, req_obj_name, res_obj_name), file=c_file)
        pass

    print("// 以下是事件函数，可直接使用", file=c_file)
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
}''' % (evt, evt_type_name, evt_obj_name, evt, evt_obj_name, evt_obj_name), file=c_file)

    print("\n\n// 以下是总处理入口函数", file=c_file)
    print('''remote_call_err_code remote_call_service(const to_mcu *to_mcu_obj, from_mcu *from_mcu_obj) {
    *from_mcu_obj = from_mcu_init_zero;
    remote_call_err_code err_code;

    switch(to_mcu_obj->which_req) {''', file=c_file, end='')

    for func in rpc_func_map:
        req_type_name, req_obj_name, req_number, req_type_is_dummy = rpc_func_map[func][0]
        res_type_name, res_obj_name, res_number, res_type_is_dummy = rpc_func_map[func][1]
        print('''
    case to_mcu_%s_tag:
        from_mcu_obj->which_res = from_mcu_%s_tag;
        err_code = remote_call_service_for_%s(&to_mcu_obj->req.%s, &from_mcu_obj->req.%s);
        break;
''' % (func, res_obj_name, func, req_obj_name, res_obj_name), file=c_file, end='')

    print('''
    default:
        err_code = remote_call_err_code_req_type_err;
        break;
    }
    return err_code;
}''', file=c_file)

    h_file.close()
    c_file.close()
    return


try:
    print_mcu_c_code(".")
    exit(0)
except Exception as e:
    print(str(e))
    exit(1)
