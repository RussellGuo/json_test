#!/bin/python3

import sys

def print_c_array(data, file=sys.stdout):
    for i in range(len(data)):
        print('   ' if i % 16 == 0 else ' ', end='', file=file)
        print('0x%02X,' % data[i],
              end='\n' if i % 16 == 15 else '', file=file)


def read_file_into_bytes(file_name):
    with open(file_name, "rb") as f:
        return f.read()
