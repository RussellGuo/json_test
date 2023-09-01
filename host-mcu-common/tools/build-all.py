#!/usr/bin/python
# -*- coding: utf-8 -*-

'''
    自动创建protobuf相关文件的“类批处理程序”
    郭强 2023-08-30

    功能包括：
    1> 生成mcu端访问protobuf的C程序 (路径在pb_intermediate下)
       这是通过nanopb工具做的
    2> 生成mcu端语义层的重复代码程序 (路径也在pb_intermediate下)
       通过Python版pb文件的反射出来的语义信息，用程序生成的
    3> 生成Android端语义层的重复代码程序 (路径在java_intermediate下)
       通过Python版pb文件的反射出来的语义信息，用程序生成的
    protobuf定义文件在host-mcu-common/protobuf_source下，叫做remote_message.proto
    一个伴随程序在同一目录下同名程序，只是后缀是 .options，被nanopb使用，可产生更简单的代码

    本程序执行需要严格的目录结构（包括nano repo的位置）
    执行的起点在mcu仓库的根目录
'''

import sys
import os
import platform


def run_cmd(cmd):
    ret = os.system(cmd)
    if ret != 0:
        raise Exception("run command %s failed" % cmd)


def build_all():
    '''
    生成程序总入口
    '''

    # 先把各种目录找对
    pwd = os.getcwd()
    tools_dir = pwd + '/host-mcu-common/tools'
    protobuf_src_dir = pwd + '/host-mcu-common/protobuf_source'
    nanopb_output_dir = pwd + '/pb_intermediate'
    java_dir = pwd + '/java_intermediate'
    nanopb_repo_dir = pwd + '/../nanopb'

    # 检查一下目录对不对
    os.chdir(nanopb_output_dir)
    os.chdir(java_dir)
    os.chdir(nanopb_repo_dir)

    os.chdir(protobuf_src_dir)
    # 生成MCU C程序
    run_cmd("%s %s/generator/protoc  --nanopb_out=%s remote_message.proto" %
            (sys.executable, nanopb_repo_dir, nanopb_output_dir))
    # 生成python程序（其实这个程序并不会直接用，而是为了后面提取出来语义定义
    run_cmd("%s %s/generator/protoc  --python_out=%s remote_message.proto" %
            (sys.executable, nanopb_repo_dir, tools_dir))
    # 根据语义定义文件生成语义程序
    run_cmd("%s %s/gen_pb_rpc_code.py %s %s" %
            (sys.executable, tools_dir, nanopb_output_dir, java_dir))


if __name__ == '__main__':
    try:
        build_all()
        print("MCU/Java code generated ")
    except e as Exception:
        print(str(e))
        exit(1)

    pass
