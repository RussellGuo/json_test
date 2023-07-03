#ifndef __INTERFACESETUP_H__
#define __INTERFACESETUP_H__

#include "USBInterface.h"

typedef struct InterfaceSetupStruct_s InterfaceSetupStruct;

extern bool InterfaceSetupConstractor(InterfaceSetupStruct* self);

struct InterfaceSetupStruct_s {
    USBInterfaceStruct base;
    union {
        EPConfigStruct InterfaceEPConfig[2];
        struct {
            EPConfigStruct RxEPConfig;
            EPConfigStruct TxEPConfig;
        };
    };

    void (*Init)(InterfaceSetupStruct* self);

    USBSetupTransferStruct Transfer;

    uint32_t SetupBufferIn[ToUintSize(64)];
    uint32_t SetupBufferOut[ToUintSize(64)];
};

#endif
