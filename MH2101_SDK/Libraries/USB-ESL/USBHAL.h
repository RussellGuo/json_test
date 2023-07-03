#ifndef __USBHAL_H__
#define __USBHAL_H__

#include "USBESL.h"

// Internal Struct
typedef struct USBHALStruct_s USBHALStruct;

typedef enum USBSpeedConfigEnum_e USBSpeedConfigEnum;
typedef enum USBSpeedEnum_e       USBSpeedEnum;
typedef enum USBBusEventEnum_e    USBBusEventEnum;

typedef void (*HalSofCallbackType)(USBHALStruct* self);
typedef void (*BusCallbackType)(USBHALStruct* self, USBBusEventEnum reason);

#include "USBTarget.h"

#if CONFIG_USB_SUPPORT_CORE_ZOFFY
#include "USBCoreZoffy.h"
#endif // CONFIG_USB_SUPPORT_CORE_ZOFFY

typedef enum {
    USBPhyTypeFS    = 0,
    USBPhyTypeFSI2C = 1,
    USBPhyTypeUTMI  = 2,
    USBPhyTypeULPI  = 3,
} USBPhyTypeEnum;

enum USBSpeedConfigEnum_e {
    USBSpeedConfigHigh = 0,
    USBSpeedConfigFull = 1,
    USBSpeedConfigLow  = 2,
};

enum USBSpeedEnum_e {
    USBSpeedHigh = 0,
    USBSpeedFull = 1,
    USBSpeedLow  = 2,
};

enum USBBusEventEnum_e {
    USBBusResetEvent   = 0,
    USBBusSuspendEvent = 1,
    USBBusResumeEvent  = 2,
};

struct USBHALStruct_s {
    bool (*Init)(USBHALStruct* self, uint32_t usbBase);

    // Public functions Start
    void (*StartDevice)(USBHALStruct* self, USBDeviceStruct* device);
    void (*StopDevice)(USBHALStruct* self);
    void (*SetAddress)(USBHALStruct* self, uint8_t address);
    uint8_t (*GetAddress)(USBHALStruct* self);
    bool (*ConfigureDevice)(USBHALStruct* self);

    void (*SetTestMode)(USBHALStruct* self, uint8_t testMode);

    USBSpeedEnum (*GetCurrentSpeed)(USBHALStruct* self);

    void (*EndpointStartSetup)(USBHALStruct* self);
    // Start probe if host is sending IN token
    bool (*EndpointStartProbe)(USBHALStruct* self, uint8_t epIndex);
    EPStatus (*GetEPStatus)(USBHALStruct* self, uint8_t epIndex, EPDirect epDirect);
    void (*EndpointStall)(USBHALStruct* self, uint8_t epIndex, EPDirect epDirect, bool isStall);
    bool (*EndpointStartRead)(USBHALStruct* self, uint8_t epIndex, uint32_t* epBuffer, uint32_t bytesToRead);
    bool (*EndpointStartWrite)(USBHALStruct* self, uint8_t epIndex, uint32_t* epBuffer, uint32_t bytesToWrite);

    void (*Interrupt)(USBHALStruct* self);
    // Public functions End

    // Public Callback Start
    HalSofCallbackType SofCallback;
    BusCallbackType    BusCallback;
    // Public Callback End

    // Public Read Only Member Start
    uint8_t DeviceEndpointCount : 4;
    uint8_t HostChannelCount : 4;
    // Public Read Only Member End

    // Public Member Start
    // Configs
    bool IsDMAEnabled : 1;
    bool IsDeviceEnabled : 1;
    bool IsHostEnabled : 1;
    bool IsSRPEnabled : 1;
    bool IsHNPEnabled : 1;

    USBPhyTypeEnum PhyType : 2;

    USBSpeedConfigEnum SpeedConfig : 2;

    // Status
    bool IsHostMode : 1;

    USBSpeedEnum Speed : 2;
    // Public Member End

    // Private Member Start
    USBDeviceStruct* Device;

    EPStruct EP[USB_DEVICE_EP_COUNT][2];
    uint16_t IEPPendingMask;
    uint16_t OEPPendingMask;
    uint16_t IEPProbingMask;
    uint16_t OEPRetryMask;

    union {
        uint32_t USBBase;
#if CONFIG_USB_SUPPORT_CORE_ZOFFY
        USBCoreZoffyStruct Zoffy;
#endif // CONFIG_USB_SUPPORT_CORE_ZOFFY
    };
    // Private Member End
};

#endif
