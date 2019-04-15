#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>



static char *argv0;
void set_cmd_line_argv0(const char *cmd_line_argv0)
{
    if (argv0 == NULL || strlen(cmd_line_argv0) > strlen(argv0)) {
        return;
    }
    strcpy(argv0, cmd_line_argv0);
}

