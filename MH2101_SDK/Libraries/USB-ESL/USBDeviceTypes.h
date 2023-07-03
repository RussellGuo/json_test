#ifndef __USBDEVICETYPES_H__
#define __USBDEVICETYPES_H__

// Dependent header, other headers are not needed

/* Standard requests */
#define GET_STATUS        (0)
#define CLEAR_FEATURE     (1)
#define SET_FEATURE       (3)
#define SET_ADDRESS       (5)
#define GET_DESCRIPTOR    (6)
#define SET_DESCRIPTOR    (7)
#define GET_CONFIGURATION (8)
#define SET_CONFIGURATION (9)
#define GET_INTERFACE     (10)
#define SET_INTERFACE     (11)

/* bmRequestType.dataTransferDirection */
#define HOST_TO_DEVICE (0)
#define DEVICE_TO_HOST (1)

/* bmRequestType.Type*/
#define STANDARD_TYPE ((uint8_t)0)
#define CLASS_TYPE    ((uint8_t)1)
#define VENDOR_TYPE   ((uint8_t)2)
#define RESERVED_TYPE ((uint8_t)3)

/* bmRequestType.Recipient */
#define DEVICE_RECIPIENT    (0)
#define INTERFACE_RECIPIENT (1)
#define ENDPOINT_RECIPIENT  (2)
#define OTHER_RECIPIENT     (3)

/* Descriptors */
#define DESCRIPTOR_TYPE(wValue)  (wValue >> 8)
#define DESCRIPTOR_INDEX(wValue) (wValue & 0xff)

/* Standard descriptor types */
#define DEVICE_DESCRIPTOR        (0x01)
#define CONFIGURATION_DESCRIPTOR (0x02)
#define STRING_DESCRIPTOR        (0x03)
#define INTERFACE_DESCRIPTOR     (0x04)
#define ENDPOINT_DESCRIPTOR      (0x05)
#define QUALIFIER_DESCRIPTOR     (0x06)

/* Class-Specific descriptor types */
#define CS_DEVICE_DESCRIPTOR        (0x21)
#define CS_CONFIGURATION_DESCRIPTOR (0x22)
#define CS_STRING_DESCRIPTOR        (0x23)
#define CS_INTERFACE_DESCRIPTOR     (0x24)
#define CS_ENDPOINT_DESCRIPTOR      (0x25)

/* Standard descriptor lengths */
#define DEVICE_DESCRIPTOR_LENGTH        (0x12)
#define CONFIGURATION_DESCRIPTOR_LENGTH (0x09)
#define INTERFACE_DESCRIPTOR_LENGTH     (0x09)
#define ENDPOINT_DESCRIPTOR_LENGTH      (0x07)
#define QUALIFIER_DESCRIPTOR_LENGTH     (0x0A)

/*string offset*/
#define STRING_OFFSET_LANGID         (0)
#define STRING_OFFSET_IMANUFACTURER  (1)
#define STRING_OFFSET_IPRODUCT       (2)
#define STRING_OFFSET_ISERIAL        (3)
#define STRING_OFFSET_ICONFIGURATION (4)
#define STRING_OFFSET_IINTERFACE     (5)

/* USB Specification Release Number */
#define USB_VERSION_2_0 (0x0200)
#define USB_VERSION_1_1 (0x0110)

/* Least/Most significant byte of short integer */
#define LSB(n) ((n)&0xff)
#define MSB(n) (((n)&0xff00) >> 8)

/* Convert physical endpoint number to descriptor endpoint number */
#define PHY_TO_DESC(endpoint) (((endpoint) >> 1) | (((endpoint)&1) ? 0x80 : 0))

/* bmAttributes in configuration descriptor */
/* C_RESERVED must always be set */
#define C_RESERVED      (1U << 7)
#define C_SELF_POWERED  (1U << 6)
#define C_REMOTE_WAKEUP (1U << 5)

/* bMaxPower in configuration descriptor */
#define C_POWER(mA) ((mA) / 2)

/* bmAttributes in endpoint descriptor */
#define E_CONTROL     (0x00)
#define E_ISOCHRONOUS (0x01)
#define E_BULK        (0x02)
#define E_INTERRUPT   (0x03)

/* For isochronous endpoints only: */
#define E_NO_SYNCHRONIZATION (0x00)
#define E_ASYNCHRONOUS       (0x04)
#define E_ADAPTIVE           (0x08)
#define E_SYNCHRONOUS        (0x0C)
#define E_DATA               (0x00)
#define E_FEEDBACK           (0x10)
#define E_IMPLICIT_FEEDBACK  (0x20)

/* Device status */
#define DEVICE_STATUS_SELF_POWERED  (1U << 0)
#define DEVICE_STATUS_REMOTE_WAKEUP (1U << 1)

/* Endpoint status */
#define ENDPOINT_STATUS_HALT (1U << 0)

/* Standard feature selectors */
#define DEVICE_REMOTE_WAKEUP (1)
#define ENDPOINT_HALT        (0)
#define TEST_MODE            (2)

/* Test Mode Selectors */
#define TEST_J            (1)
#define TEST_K            (2)
#define TEST_SE0_NAK      (3)
#define TEST_Packet       (4)
#define TEST_Force_Enable (5)

#ifndef CONFIG_USB_VERSION
#define CONFIG_USB_VERSION USB_VERSION_2_0
#endif

typedef struct USBDeviceStruct_s        USBDeviceStruct;
typedef struct USBSetupPacketStruct_s   USBSetupPacketStruct;
typedef struct USBSetupTransferStruct_s USBSetupTransferStruct;

typedef enum USBDeviceStateEnum_e USBDeviceStateEnum;
typedef enum USBDeviceEventEnum_e USBDeviceEventEnum;

typedef union USBDescriptorUnion_u USBDescriptorUnion;
typedef union USBAttributesUnion_u USBAttributesUnion;

struct USBSetupPacketStruct_s {
    struct RequestTypeStruct {
        uint8_t dataTransferDirection;
        uint8_t Type;
        uint8_t Recipient;
    } bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
};

struct USBSetupTransferStruct_s {
    USBSetupPacketStruct Packet;
    // Transfer part
    uint8_t* Buffer;
    uint16_t Length;
    uint8_t  Direction;
    bool     IsRequestLength;
    bool (*Callback)(USBDeviceStruct* self, USBSetupTransferStruct* transfer);
};

enum USBDeviceStateEnum_e {
    ATTACHED,
    POWERED,
    DEFAULT,
    ADDRESS,
    CONFIGURED,
    SUSPENDED,
};

enum USBDeviceEventEnum_e {
    USBDeviceUnknownEvent      = 0x00,
    USBDeviceStartEvent        = 0x01,
    USBDeviceAddressedEvent    = 0x02,
    USBDeviceConfiguredEvent   = 0x03,
    USBDeviceDeconfiguredEvent = 0x04,
    USBDeviceStopEvent         = 0x05,

    USBDeviceBusResetEvent   = 0x10,
    USBDeviceBusSuspendEvent = 0x11,
    USBDeviceBusResumeEvent  = 0x12,
};

union USBDescriptorUnion_u {
    uint32_t Device[ToUintSize(DEVICE_DESCRIPTOR_LENGTH)];
    uint32_t String[ToUintSize(CONFIG_USB_DEVICE_MAX_STRING_DESCRIPTOR_SIZE)];
    uint32_t Configuration[ToUintSize(CONFIG_USB_DEVICE_MAX_CONFIGURATION_DESCRIPTOR_SIZE)];
    uint32_t Qualifer[ToUintSize(QUALIFIER_DESCRIPTOR_LENGTH)];
    uint32_t Report[ToUintSize(CONFIG_USB_DEVICE_MAX_REPORT_DESCRIPTOR_SIZE)];
    uint32_t Vendor[ToUintSize(CONFIG_USB_DEVICE_MAX_VENDOR_DESCRIPTOR_SIZE)];
};

union USBAttributesUnion_u {
    uint8_t Data;
    struct {
        uint8_t Reserved : 5;
        uint8_t RemoteWakeup : 1;
        uint8_t SelfPowered : 1;
        uint8_t Reserved7 : 1;
    };
};

extern bool USBDeviceConstractor(USBDeviceStruct* self);
#endif
