#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os
import platform

def usage():
    print("用法：%s pem_file_name pem_inc_file_name" % (sys.argv[0],))

if __name__ == '__main__':

    if len(sys.argv) != 3:
        usage()
        exit(1)


    try:
        pem_file_name = sys.argv[1]
        pem_inc_file_name = sys.argv[2]
        with open(pem_file_name, "r") as in_file:
            lines = in_file.readlines()

        begin_public_key_count = 0
        end_public_key_count = 0
        for line in lines:
            begin_public_key_count += "-BEGIN PUBLIC KEY-" in line
            end_public_key_count += "-END PUBLIC KEY-" in line
        if begin_public_key_count != 1 or end_public_key_count != 1:
            raise RuntimeError("public pem file not recognized")

        with open(pem_inc_file_name, "w") as out_file:
            begin_public_key_count = 0
            end_public_key_count = 0
            for line in lines:
                new_line = '    "' + line + '"'
                new_line = new_line.replace("\r", "\\r")
                new_line = new_line.replace("\n", "\\n")
                print(new_line, file=out_file)
                pass
            


        exit(0)
    except Exception as e:
        print(str(e))
        exit(1)