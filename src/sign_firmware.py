# /bin/python

import sys
import getopt

import misc
import sign_verify


def print_usage_and_exit():
    print("error", file=sys.stderr)
    sys.exit(1)


def main():
    cmd_param = {
        'private': None,
        'public': None,
        'output': None,
        'executable': None,
        'inc': None
    }

    def check_and_set_variable(var, value):
        if cmd_param[var] is None:
            cmd_param[var] = value
        else:
            print_usage_and_exit()

    opts, args = getopt.getopt(sys.argv[1:], 'r:u:o:e:i', [
                               'private=', 'public=', 'output=', 'executable=', 'inc'])
    for opt_name, opt_value in opts:
        if opt_name in ('-r', '--private'):
            check_and_set_variable('private', opt_value)
        elif opt_name in ('-u', '--public'):
            check_and_set_variable('public', opt_value)
        elif opt_name in ('-o', '--output'):
            check_and_set_variable('output', opt_value)
        elif opt_name in ('-e', '--executable'):
            check_and_set_variable('executable', opt_value)
        elif opt_name in ('-i', '--inc'):
            check_and_set_variable('inc', True)

    private_key_pem_file_name = cmd_param['private']
    public_key_pem_file_name = cmd_param['public']
    output_firmware_filename = cmd_param['output']
    exec_bin_file_name = cmd_param['executable']
    save_inc_file = not not cmd_param['inc']

    if private_key_pem_file_name == None or public_key_pem_file_name == None or output_firmware_filename == None or exec_bin_file_name == None:
        print_usage_and_exit()


if __name__ == '__main__':
    try:
        main()
        sys.exit(0)
    except:
        pass
        sys.exit(1)
