#if CONFIG_USB_SUPPORT_CORE_ZOFFY

#ifndef CONFIG_USBCORE_ZOFFY_SUPPORT_DMA
#define CONFIG_USBCORE_ZOFFY_SUPPORT_DMA 0
#endif

// Endpoints shared RX FIFO, RX FIFO size is determined by the MRPS(Max RX Packet Size)
#ifndef CONFIG_USBCORE_ZOFFY_MAX_RX_PACKET_SIZE
#error "Must define CONFIG_USBCORE_ZOFFY_MAX_RX_PACKET_SIZE in USBTarget<MCU>.h or USBVendorConfig.h(1st priority)"
#endif

// USB max endpoint count
#ifndef CONFIG_USBCORE_ZOFFY_EP_COUNT
#error "Must define CONFIG_USBCORE_ZOFFY_EP_COUNT in USBTarget<MCU>.h"
#endif

// CONFIG_USBCORE_ZOFFY_MAX_RX_FIFO_SIZE = 92 + MRPS    (156 <MRPS 64>; 220 <MRPS 128>; 348 <MRPS 256>)
#define USBCORE_ZOFFY_MAX_RX_FIFO_SIZE (((5 * 1 + 8) + ((CONFIG_USBCORE_ZOFFY_MAX_RX_PACKET_SIZE / 4) + 1) + (2 * CONFIG_USBCORE_ZOFFY_EP_COUNT) + 1) * 4)

#if USB_DEVICE_EP_COUNT < CONFIG_USBCORE_ZOFFY_EP_COUNT
#undef USB_DEVICE_EP_COUNT
#define USB_DEVICE_EP_COUNT CONFIG_USBCORE_ZOFFY_EP_COUNT
#endif

typedef struct USBZoffyStruct_s     USBZoffyStruct;
typedef struct USBCoreZoffyStruct_s USBCoreZoffyStruct;

struct USBCoreZoffyStruct_s {
    USBZoffyStruct* USB;

    uint16_t MaxFifoSize;
    uint16_t UsedFifoSize;
    uint8_t  TxFifoIndex : 4;

    bool IsDMASupported : 1;
    bool IsDedicatedTFifo : 1;

    bool IsHostSupported : 1;
    bool IsDeviceSupported : 1;
    bool IsSRPSupported : 1;
    bool IsHNPSupported : 1;

    uint16_t OEPSplitMask;
    uint16_t OEPToDisable;
};

extern bool USBCoreZoffyConstractor(USBHALStruct* self);

#endif // CONFIG_USB_SUPPORT_CORE_ZOFFY
