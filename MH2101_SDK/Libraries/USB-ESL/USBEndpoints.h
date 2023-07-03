#ifndef __USBENDPOINTS_H__
#define __USBENDPOINTS_H__

// Dependent header, other headers are not needed

// Internal Struct
typedef struct EPConfigStruct_s EPConfigStruct;
typedef struct EPStruct_s       EPStruct;

// Enum Type Define
typedef enum EPType_e {
    EPTypeControl   = 0x00,
    EPTypeISO       = 0x01,
    EPTypeBulk      = 0x02,
    EPTypeInterrupt = 0x03,
} EPType;

typedef enum EPDirect_s {
    EPDirectOut = 0x00,
    EPDirectIn  = 0x01,
} EPDirect;

typedef enum EPStatus_s {
    EPCompleted = 0x00, // Transfer completed
    EPPending   = 0x01, // Transfer in progress
    EPRetrying  = 0x02, // Transfer retrying
    EPInvalid   = 0x03, // Invalid parameter
    EPStalled   = 0x04, // Endpoint stalled
    EPProbing   = 0x05, // Probing IN/OUT token
} EPStatus;

#define USB_TRANSFER_SETUP (0x80000000U)

// EPStruct: 20 bytes
struct EPStruct_s {
    EPStatus        Status : 8;     // Endpoint Status
    uint8_t         PendingTick;    // Endpoint Pending Tick
    bool            IsSetup;        // Is setup transfer
    uint32_t*       Buffer;         // Buffer Address
    uint32_t        TransferOffset; // Buffer Point
    uint32_t        TransferSize;   // Transfer Size
    EPConfigStruct* Config;         // Endpoint Init Config / Endpoint is Configured
};

// EPConfigStruct: 28 bytes
struct EPConfigStruct_s {
    uint8_t   Index;
    EPDirect  Direct : 4; // Endpoint Direction
    EPType    Type : 4;   // Endpoint Type : Control / Interrupt / Bulk / ISO
    uint8_t   Interval;   // Endpoint Low/Full-Speed Interval
    uint8_t   HSInterval; // Endpoint High-Speed Interval
    uint8_t   Timeout;    // Endpoint Transfer Timeout (Tick), EPDirectOut only
    uint16_t  MaxPacket;  // Endpoint Max Packet Size
    uint32_t  MaxBuffer;  // Endpoint Max Single Transfer Size
    uint32_t* Buffer;
    uint8_t*  ExtendDescriptor;

    USBInterfaceStruct* Interface;
    bool (*Callback)(USBInterfaceStruct* interface, uint32_t* buffer, uint32_t bytesTransfer);
};

#endif
