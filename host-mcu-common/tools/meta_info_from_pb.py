#!/usr/bin/python
# -*- coding: utf-8 -*-

'''从项目自定义的protobuf文件中提取pb的符号信息

 在运行前，要执行：
#    ../../nanopb/generator/protoc --python_out=. remote_message.proto
来生成pb的python版本。随后我们就能获取所需的信息了

我们的pb有如下约定： 
    host->mcu的请求/响应消息有复数个
        对于每个业务func而言，业务的请求消息命名为func_req，响应消息未func_res。
        所有的请求消息会放在to_mcu这个消息中，所有的响应消息会放在from_mcu中
        因为to_mcu/from_mcu可以承载各种func，所以会有 oneof 字段（名字分别是是req/res）
    除了上面说的func的请求/响应func外，从mcu发出的event也是一种类别。这种则是_evt结尾的类型
        evt类型也会出现在from_mcu的oneof字段中。具体可参见 remote_message.proto
正因为上述约定，所以我们可以利用约定从生成的python程序中提取出来请求/响应/事件的列表
三个列表，每个列表表项是个元组，包含类型名 成员对象名 内部编号 是否是哑类型
这几个列表会用于自动生成Java/C的处理程序
'''

import remote_message_pb2


def is_a_dummy_type(type_name):
    '''针对一个类型，判断其是否是哑的
    哑类型一定是自定义类型，其中只有一个字段dummy
    这种类型相当于void，只是占位符'''
    try:
        # 从内部实现中搜。或许跟具体的pb编译器版本有关
        type = remote_message_pb2.DESCRIPTOR.message_types_by_name[type_name]
        is_dummy = len(type.fields) == 1 and type.fields[0].name == 'dummy'
        return is_dummy
    except e:
        return False


def get_field_list(fields_inside_pb, type_name_filter):
    '''
    获取指定字段列表fields_inside_pb的每一个元素的信息，包括 类型名 对象名 编号 和是否哑类型
    type_name_filter是针对类型名的真值过滤器
    '''
    field_list = []
    for field in fields_inside_pb:
        # 针对每条信息
        type_name, field_name, number = field.message_type.name, field.name, field.number
        # 过滤
        is_matched = type_name_filter(type_name)
        if not is_matched:
            continue
        # 查出来是不是哑类型
        is_dummy = is_a_dummy_type(type_name)
        # 填入元组
        field_list.append((type_name, field_name, number, is_dummy))
    # 最后返回之
    return field_list

#请看顶部注释，我们把相关信息放入全局变量中
evt_list = get_field_list(
    remote_message_pb2._FROM_MCU.oneofs_by_name['res'].fields, lambda type_name: type_name.endswith('_evt'))
res_list = get_field_list(
    remote_message_pb2._FROM_MCU.oneofs_by_name['res'].fields, lambda type_name: type_name.endswith('_res'))
req_list = get_field_list(
    remote_message_pb2._TO_MCU.oneofs_by_name['req'].fields, lambda type_name: type_name.endswith('_req'))
pass

if __name__ == '__main__':
    print('本文件不可单独执行')
    exit(1)
