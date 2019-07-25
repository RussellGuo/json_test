#ifndef SUSPEND_API_H
#define SUSPEND_API_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int suspend_enable(void);

// shutdown or reboot the system
// reboot == 0 causes shutdown, or else causes reboot
// return 0 means OK, return < 0 means failed.
int halt_system(bool reboot);

#ifdef __cplusplus
} // extern "C"
#endif
#endif /*SUSPEND_API_H*/
