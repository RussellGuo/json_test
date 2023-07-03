#ifndef __INTERFACEMSD_H__
#define __INTERFACEMSD_H__

#include "USBInterface.h"

// Default Endpoint Config Start
// Can be overrode after InterfaceConstractor(interface) and before interface->Init()
#ifndef MSD_RX_EP
#define MSD_RX_EP 1
#endif

#ifndef MSD_TX_EP
#define MSD_TX_EP 1
#endif

#ifndef MSD_MAX_PACKET
#define MSD_MAX_PACKET 64
#endif
// Default Endpoint Config End

#define DISK_OK       0x00
#define NO_INIT       0x01
#define NO_DISK       0x02
#define WRITE_PROTECT 0x04

#define CBW_Signature 0x43425355
#define CSW_Signature 0x53425355

// SCSI Commands
#define TEST_UNIT_READY        0x00
#define REQUEST_SENSE          0x03
#define FORMAT_UNIT            0x04
#define INQUIRY                0x12
#define MODE_SELECT6           0x15
#define MODE_SENSE6            0x1A
#define START_STOP_UNIT        0x1B
#define MEDIA_REMOVAL          0x1E
#define READ_FORMAT_CAPACITIES 0x23
#define READ_CAPACITY          0x25
#define READ10                 0x28
#define WRITE10                0x2A
#define VERIFY10               0x2F
#define READ_TOC               0x43
#define GET_EVENT_STATUS       0x4A
#define READ12                 0xA8
#define WRITE12                0xAA
#define MODE_SELECT10          0x55
#define MODE_SENSE10           0x5A

// MSC class specific requests
#define MSC_REQUEST_RESET       0xFF
#define MSC_REQUEST_GET_MAX_LUN 0xFE

typedef struct InterfaceMSDStruct_s InterfaceMSDStruct;

typedef enum StageEnum_e   StageEnum;
typedef struct CBWStruct_s CBWStruct;
typedef struct CSWStruct_s CSWStruct;

enum StageEnum_e {
    READ_CBW,    // wait a CBW
    STAGE_ERROR, // error
    PROCESS_CBW, // process a CBW request
    SEND_CSW,    // send a CSW
    WAIT_CSW,    // wait that a CSW has been effectively sent
};

enum StatusEnum_e {
    CSW_PASSED,
    CSW_FAILED,
    CSW_ERROR,
};

// CBWStruct: 31 bytes
struct CBWStruct_s {
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataLength;
    uint8_t  Flags;
    uint8_t  LUN;
    uint8_t  CBLength;
    uint8_t  CB[16];
} __PACKED;

// CSWStruct: 13 bytes
struct CSWStruct_s {
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataResidue;
    uint8_t  Status;
} __PACKED;

extern bool InterfaceMSDConstractor(InterfaceMSDStruct* self);

struct InterfaceMSDStruct_s {
    USBInterfaceStruct base;
    union {
        EPConfigStruct InterfaceEPConfig[2];
        struct {
            EPConfigStruct RxEPConfig;
            EPConfigStruct TxEPConfig;
        };
    };

    void (*Init)(InterfaceMSDStruct* self);
    bool (*Write)(InterfaceMSDStruct* self, uint32_t* writeBuffer, uint16_t writeLength);
    void (*Fail)(InterfaceMSDStruct* self);
    void (*SendCSW)(InterfaceMSDStruct* self);
    bool (*InfoTransfer)(InterfaceMSDStruct* self);

    union {
        CBWStruct CBW;
        uint32_t  CBWBuffer[ToUintSize(sizeof(CBWStruct))];
    };
    union {
        CSWStruct CSW;
        uint32_t  CSWBuffer[ToUintSize(sizeof(CSWStruct))];
    };

    // addr where will be read or written data
    uint32_t Addr;
    // length of a reading or writing
    uint32_t Length;

    uint32_t BlockSize;
    uint64_t BlockCount;
    uint64_t MemorySize;

    uint32_t* ReadBuffer;
    uint32_t* WriteBuffer;

    uint16_t ReadBufferSize;
    uint16_t WriteBufferSize;

    uint32_t* InquiryData;
    uint8_t   InquirySize;
    uint8_t   MaxLun[1];

    volatile StageEnum Stage : 8;

    // memory OK (after a memoryVerify)
    volatile bool MemOK : 1;
    // is pending read from ReadBuffer (for async read)
    volatile bool IsPendingRead : 1;

    bool (*CBWDecodeCallback)(InterfaceMSDStruct* self);
    bool (*OutProcessCallback)(InterfaceMSDStruct* self, uint8_t* buffer, uint32_t bytesRead);
    bool (*InProcessCallback)(InterfaceMSDStruct* self, uint8_t* buffer, uint32_t bytesWritten);

    bool (*DiskStatusCallback)(InterfaceMSDStruct* self);
    uint32_t (*DiskReadCallback)(InterfaceMSDStruct* self, uint8_t* buffer, uint32_t blockAddr, uint32_t blockSize);
    void (*DiskWriteCallback)(InterfaceMSDStruct* self, uint8_t* buffer, uint32_t blockAddr, uint32_t blockSize);
};

#endif
