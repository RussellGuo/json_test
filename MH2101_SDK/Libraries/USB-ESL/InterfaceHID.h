#ifndef __INTERFACEHID_H__
#define __INTERFACEHID_H__

#include "USBInterface.h"

// Default Endpoint Config Start
// Can be overrode after InterfaceConstractor(interface) and before interface->Init()
#ifndef HID_RX_EP
#define HID_RX_EP 3
#endif

#ifndef HID_TX_EP
#define HID_TX_EP 3
#endif

#ifndef HID_MAX_PACKET
#define HID_MAX_PACKET 64
#endif
// Default Endpoint Config End

// Default HID Vendor Report Size
#ifndef HID_VENDOR_REPORT_SIZE
#define HID_VENDOR_REPORT_SIZE 64
#endif

/* */
#define HID_VERSION (0x0111)

/* HID Class */
#define HID_CLASS             (3)
#define HID_SUBCLASS_NONE     (0)
#define HID_SUBCLASS_BOOT     (1)
#define HID_PROTOCOL_NONE     (0)
#define HID_PROTOCOL_KEYBOARD (1)
#define HID_PROTOCOL_MOUSE    (2)

/* Descriptors */
#define HID_DESCRIPTOR      (0x21)
#define HID_DESCRIPTOR_SIZE (0x09)
#define REPORT_DESCRIPTOR   (0x22)

/* Class requests */
#define GET_REPORT   (0x01)
#define GET_IDLE     (0x02)
#define GET_PROTOCOL (0x03)
#define SET_REPORT   (0x09)
#define SET_IDLE     (0x0A)
#define SET_PROTOCOL (0x0B)

/* HID Class Report Descriptor */
/* Short items: size is 0, 1, 2 or 3 specifying 0, 1, 2 or 4 (four) bytes */
/* of data as per HID Class standard */

/* Main items */
#define INPUT(size)          (0x80 | size)
#define OUTPUT(size)         (0x90 | size)
#define FEATURE(size)        (0xb0 | size)
#define COLLECTION(size)     (0xa0 | size)
#define END_COLLECTION(size) (0xc0 | size)

/* Global items */
#define USAGE_PAGE(size)       (0x04 | size)
#define LOGICAL_MINIMUM(size)  (0x14 | size)
#define LOGICAL_MAXIMUM(size)  (0x24 | size)
#define PHYSICAL_MINIMUM(size) (0x34 | size)
#define PHYSICAL_MAXIMUM(size) (0x44 | size)
#define UNIT_EXPONENT(size)    (0x54 | size)
#define UNIT(size)             (0x64 | size)
#define REPORT_SIZE(size)      (0x74 | size)
#define REPORT_ID(size)        (0x84 | size)
#define REPORT_COUNT(size)     (0x94 | size)
#define PUSH(size)             (0xa4 | size)
#define POP(size)              (0xb4 | size)

/* Local items */
#define USAGE(size)              (0x08 | size)
#define USAGE_MINIMUM(size)      (0x18 | size)
#define USAGE_MAXIMUM(size)      (0x28 | size)
#define DESIGNATOR_INDEX(size)   (0x38 | size)
#define DESIGNATOR_MINIMUM(size) (0x48 | size)
#define DESIGNATOR_MAXIMUM(size) (0x58 | size)
#define STRING_INDEX(size)       (0x78 | size)
#define STRING_MINIMUM(size)     (0x88 | size)
#define STRING_MAXIMUM(size)     (0x98 | size)
#define DELIMITER(size)          (0xa8 | size)

#define DATA_SHORT(value) LSB(value), MSB(value)

typedef struct InterfaceHIDStruct_s InterfaceHIDStruct;

extern bool InterfaceHIDConstractor(InterfaceHIDStruct* self);

struct InterfaceHIDStruct_s {
    USBInterfaceStruct base;
    union {
        EPConfigStruct InterfaceEPConfig[2];
        struct {
            EPConfigStruct RxEPConfig;
            EPConfigStruct TxEPConfig;
        };
    };

    uint32_t  HIDDescriptorData[ToUintSize(HID_DESCRIPTOR_SIZE)];
    uint32_t* ReportDescriptorData;
    uint32_t  ReportDescriptorSize;

    uint16_t Idle;
    uint8_t  Protocol;

    bool IsInputEPEnabled : 1;
    bool IsOutputEPEnabled : 1;

    volatile bool IsInputPending : 1;
    volatile bool IsOutputUpdated : 1;

    uint16_t InputReportSize;
    uint16_t OutputReportSize;

    uint32_t* InputReportData;
    uint32_t* OutputReportData;

    void (*Init)(InterfaceHIDStruct* self);
    bool (*PutInputReport)(InterfaceHIDStruct* self, uint8_t* inputReport, uint32_t inputSize);
    bool (*GetOutputReport)(InterfaceHIDStruct* self, uint8_t* outputReport, uint32_t outputSize);

    // Application Callback
    bool (*SetReportCallback)(InterfaceHIDStruct* self, uint8_t* outputReport, uint32_t inputSize);
    bool (*GetReportCallback)(InterfaceHIDStruct* self, uint8_t* inputReport, uint32_t outputSize);
    void (*SetIdleCallback)(InterfaceHIDStruct* self);
    void (*SetProtocolCallback)(InterfaceHIDStruct* self);
};

#endif
