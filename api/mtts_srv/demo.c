#include "pcm_play.h"
#include "mtts_c.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    const char resource_dir[] = "/system/vendor/huaqin";
    bool ret = chdir(resource_dir);
    if (ret < 0) {
        perror("cd resource");
        exit(1);
    }
    tts_init();
    tts_play(false, argv[1] ?: "我是马首我是马首");
    return 0;
}
