#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define LOG_TAG "libsuspendapi"
#include <cutils/log.h>
#include <cutils/android_reboot.h>
#include <cutils/properties.h>


#define EARLYSUSPEND_SYS_POWER_STATE "/sys/power/state"

static const char *pwr_state_mem = "mem";

int suspend_enable(void)
{
    int sPowerStatefd;
    char buf[80];
    int ret;

    sPowerStatefd = open(EARLYSUSPEND_SYS_POWER_STATE, O_RDWR);

    if (sPowerStatefd < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGW("Error opening %s: %s\n", EARLYSUSPEND_SYS_POWER_STATE, buf);
        return NULL;
    }

    ret = write(sPowerStatefd, pwr_state_mem, strlen(pwr_state_mem));
    if (ret < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error writing to %s: %s\n", EARLYSUSPEND_SYS_POWER_STATE, buf);
        goto err;
    }

    close(sPowerStatefd);
    ALOGV("set suspend enable done\n");

    return 0;

err:
    return ret;
}

int halt_system(bool reboot)
{
    int ret = property_set(ANDROID_RB_PROPERTY, reboot ? "reboot" : "shutdown");
    return ret;
}
