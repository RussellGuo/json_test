#ifndef __INTERFACECDC_H__
#define __INTERFACECDC_H__

#include "USBInterface.h"
#include "InterfaceSerial.h"

#define CDC_EXTEND_DESCRIPTOR_SIZE 0x13
#define CDC_LINE_CODING_SIZE       0x07
#define CDC_SET_LINE_CODING        0x20
#define CDC_GET_LINE_CODING        0x21
#define CDC_SET_CONTROL_LINE_STATE 0x22

// Control Line State bits
#define CLS_DTR (1 << 0)
#define CLS_RTS (1 << 1)

typedef struct InterfaceCDCStruct_s InterfaceCDCStruct;

extern bool InterfaceCDCConstractor(InterfaceCDCStruct* self, InterfaceSerialStruct* serial);

struct InterfaceCDCStruct_s {
    USBInterfaceStruct base;
    union {
        EPConfigStruct InterfaceEPConfig[1];
        struct {
            EPConfigStruct TxEPConfig;
        };
    };

    InterfaceSerialStruct* Serial;

    uint32_t ExtendDescriptorData[ToUintSize(CDC_EXTEND_DESCRIPTOR_SIZE)];
    union {
        // See PSTN120.pdf - 6.3.11 Table 17: Line Coding Structure
        struct {
            uint32_t DTERate;
            uint8_t  CharFormat;
            uint8_t  ParityType;
            uint8_t  DataBits;
        };
        uint32_t LineCodingData[ToUintSize(CDC_LINE_CODING_SIZE)];
    };

    void (*Init)(InterfaceCDCStruct* self);
    bool (*ClassRequestCallback)(InterfaceCDCStruct* self, USBSetupTransferStruct* transfer);

    // Application Callback
    void (*SetLineCodingCallback)(InterfaceCDCStruct* self, uint32_t baudrate, uint8_t stop, uint8_t parity, uint8_t bits);
};

#endif
