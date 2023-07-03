#ifndef __INTERFACECCID_H__
#define __INTERFACECCID_H__

#include "USBInterface.h"

// Default Endpoint Config Start
// Can be overrode after InterfaceConstractor(interface) and before interface->Init()
#ifndef CCID_RX_EP
#define CCID_RX_EP 2
#endif

#ifndef CCID_TX_EP
#define CCID_TX_EP 2
#endif

#ifndef CCID_MAX_PACKET
#define CCID_MAX_PACKET 64
#endif

#ifdef CCID_SUPPORT_NOTIFICATION
#ifndef CCID_INT_EP
#define CCID_INT_EP 3
#endif
#endif // CCID_SUPPORT_NOTIFICATION
// Default Endpoint Config End

#ifdef CCID_MAX_BYTE_BUFFER
#error "CCID_MAX_BYTE_BUFFER is DEPRECATED, please use CCID_MAX_COMMAND/RESPONSE_BUFFER instead!"
#endif

#ifndef CCID_MAX_COMMAND_BUFFER
#define CCID_MAX_COMMAND_BUFFER 271
#endif

#ifndef CCID_MAX_RESPONSE_BUFFER
#define CCID_MAX_RESPONSE_BUFFER 271
#endif

#define CCID_CLASS                  (0x0B)
#define CCID_DESCRIPTOR_SIZE        (0x36)
#define CCID_DESCRIPTOR_TYPE        (0x21)
#define CCID_VERSION                (0x0110)
#define CCID_HEADER_SIZE            (10)
#define CCID_MAX_COMMAND_DATA_SIZE  (CCID_MAX_COMMAND_BUFFER - CCID_HEADER_SIZE)
#define CCID_MAX_RESPONSE_DATA_SIZE (CCID_MAX_RESPONSE_BUFFER - CCID_HEADER_SIZE)

// CCID message type Start
#define PC_TO_RDR_ICCPOWERON                   0x62
#define PC_TO_RDR_ICCPOWEROFF                  0x63
#define PC_TO_RDR_GETSLOTSTATUS                0x65
#define PC_TO_RDR_XFRBLOCK                     0x6F
#define PC_TO_RDR_GETPARAMETERS                0x6C
#define PC_TO_RDR_RESETPARAMETERS              0x6D
#define PC_TO_RDR_SETPARAMETERS                0x61
#define PC_TO_RDR_ESCAPE                       0x6B
#define PC_TO_RDR_ICCCLOCK                     0x6E
#define PC_TO_RDR_T0APDU                       0x6A
#define PC_TO_RDR_SECURE                       0x69
#define PC_TO_RDR_MECHANICAL                   0x71
#define PC_TO_RDR_ABORT                        0x72
#define PC_TO_RDR_SETDATARATEANDCLOCKFREQUENCY 0x73

#define RDR_TO_PC_DATABLOCK                 0x80
#define RDR_TO_PC_SLOTSTATUS                0x81
#define RDR_TO_PC_PARAMETERS                0x82
#define RDR_TO_PC_ESCAPE                    0x83
#define RDR_TO_PC_DATARATEANDCLOCKFREQUENCY 0x84

#define RDR_TO_PC_NOTIFYSLOTCHANGE 0x50
#define RDR_TO_PC_HARDWAREERROR    0x51

#define SLOT_ICC_PRESENT 0x01 /* LSb : (0b = no ICC present, 1b = ICC present) */
#define SLOT_ICC_CHANGE  0x02 /* MSb : (0b = no change, 1b = change) */
// CCID message type End

// CCID error Start
#define CCID_ERROR_OK      0x00
#define CCID_ERROR_UNKNOWN 0x81

/* Index of not supported / incorrect message parameter : 7Fh to 01h */
/* These Values are used for Return Types between Firmware Layers    */
/*
Failure of a command
The CCID cannot parse one parameter or the ICC is not supporting one parameter.
Then the Slot Error register contains the index of the first bad parameter as a
positive number (1-127). For instance, if the CCID receives an ICC command to
an unimplemented slot, then the Slot Error register shall be set to
‘5’ (index of bSlot field).
  */

#define CCID_ERROR_BAD_LENTGH               0x01
#define CCID_ERROR_BAD_SLOT                 0x05
#define CCID_ERROR_BAD_POWERSELECT          0x07
#define CCID_ERROR_BAD_PROTOCOLNUM          0x07
#define CCID_ERROR_BAD_CLOCKCOMMAND         0x07
#define CCID_ERROR_BAD_ABRFU_3B             0x07
#define CCID_ERROR_BAD_BMCHANGES            0x07
#define CCID_ERROR_BAD_BFUNCTION_MECHANICAL 0x07
#define CCID_ERROR_BAD_ABRFU_2B             0x08
#define CCID_ERROR_BAD_LEVELPARAMETER       0x08
#define CCID_ERROR_BAD_FIDI                 0x0A
#define CCID_ERROR_BAD_T01CONVCHECKSUM      0x0B
#define CCID_ERROR_BAD_GUARDTIME            0x0C
#define CCID_ERROR_BAD_WAITINGINTEGER       0x0D
#define CCID_ERROR_BAD_CLOCKSTOP            0x0E
#define CCID_ERROR_BAD_IFSC                 0x0F
#define CCID_ERROR_BAD_NAD                  0x10
#define CCID_ERROR_BAD_DWLENGTH             0x08 /* Used in PC_to_RDR_XfrBlock*/

// Table 6.2-2 Slot error register when bmCommandStatus = 1
#define CCID_ERROR_CMD_ABORTED                0xFF
#define CCID_ERROR_ICC_MUTE                   0xFE
#define CCID_ERROR_XFR_PARITY_ERROR           0xFD
#define CCID_ERROR_XFR_OVERRUN                0xFC
#define CCID_ERROR_HW_ERROR                   0xFB
#define CCID_ERROR_BAD_ATR_TS                 0xF8
#define CCID_ERROR_BAD_ATR_TCK                0xF7
#define CCID_ERROR_ICC_PROTOCOL_NOT_SUPPORTED 0xF6
#define CCID_ERROR_ICC_CLASS_NOT_SUPPORTED    0xF5
#define CCID_ERROR_PROCEDURE_BYTE_CONFLICT    0xF4
#define CCID_ERROR_DEACTIVATED_PROTOCOL       0xF3
#define CCID_ERROR_BUSY_WITH_AUTO_SEQUENCE    0xF2
#define CCID_ERROR_PIN_TIMEOUT                0xF0
#define CCID_ERROR_PIN_CANCELLED              0xEF
#define CCID_ERROR_CMD_SLOT_BUSY              0xE0
#define CCID_ERROR_CMD_NOT_SUPPORTED          0x00
// CCID error End

#define DEFAULT_FIDI            0x11 /* DEFAULT_FIDI_VALUE */
#define DEFAULT_T01CONVCHECKSUM 0x00
#define DEFAULT_EXTRA_GUARDTIME 0x00
#define DEFAULT_WAITINGINTEGER  0x0A
#define DEFAULT_CLOCKSTOP       0x00
#define DEFAULT_IFSC            0x20
#define DEFAULT_NAD             0x00

#define DEFAULT_DATA_RATE  115200
#define DEFAULT_CLOCK_FREQ 3600

/*
Offset=0 bmICCStatus 2 bit  0, 1, 2
    0 - An ICC is present and active (power is on and stable, RST is inactive)
    1 - An ICC is present and inactive (not activated or shut down by hardware error)
    2 - No ICC is present
    3 - RFU
Offset=0 bmRFU 4 bits 0 RFU
Offset=6 bmCommandStatus 2 bits 0, 1, 2
    0 - Processed without error
    1 - Failed (error code provided by the error register)
    2 - Time Extension is requested
    3 - RFU
  */
typedef enum {
    CCIDCardPresentActive   = 0x00,
    CCIDCardPresentInactive = 0x01,
    CCIDCardNotPresent      = 0x02,
} CCIDCardStatusEnum;

typedef enum {
    CCIDCommandNoError = 0x00,
    CCIDCommandFailed  = 0x01,
    CCIDCommandTimeout = 0x02,
} CCIDCommandStatusEnum;

/* defines for the CCID_CMD Layers */
#define SIZE_OF_ATR              33
#define LEN_RDR_TO_PC_SLOTSTATUS 10
#define LEN_PROTOCOL_STRUCT_T0   5
#define LEN_PROTOCOL_STRUCT_T1   7
#define LEN_MAX_PROTOCOL_STRUCT  7

#define BPROTOCOL_NUM_T0 0
#define BPROTOCOL_NUM_T1 1

/************************************************************************************/
/*   ERROR CODES for RDR_TO_PC_HARDWAREERROR Message : bHardwareErrorCode           */
/************************************************************************************/

#define HARDWAREERRORCODE_OVERCURRENT     0x01
#define HARDWAREERRORCODE_VOLTAGEERROR    0x02
#define HARDWAREERRORCODE_OVERCURRENT_IT  0x04
#define HARDWAREERRORCODE_VOLTAGEERROR_IT 0x08

// Add by link 2017.3.20
#define CCID_RECV_Q_SIZE            (10)
#define Q_NEXT(pos, size)           (((pos) + 1) % (size))
#define Q_LEN(head, tail, size)     (((head) < (tail) ? (tail) - (head) : (head) - (tail)) % (size))
#define Q_IS_FULL(head, tail, size) (((tail) + 1) % (size) == (head) % (size))
#define Q_EMPTY(head, tail, size)   ((head) % (size) == (tail) % (size))

typedef volatile enum {
    CCIDStateIdle,
    CCIDStateSendResponse,
    CCIDStateError,
} CCIDStateEnum;

#pragma pack(1)
typedef struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdCCID;
    uint8_t  bMaxSlotIndex;
    uint8_t  bVoltageSupport;
    uint32_t dwProtocols;
    uint32_t dwDefaultClock;
    uint32_t dwMaximumClock;
    uint8_t  bNumClockSupported;
    uint32_t dwDataRate;
    uint32_t dwMaxDataRate;
    uint8_t  bNumDataRatesSupported;
    uint32_t dwMaxIFSD;
    uint32_t dwSynchProtocols;
    uint32_t dwMechanical;
    uint32_t dwFeatures;
    uint32_t dwMaxCCIDMessageLength;
    uint8_t  bClassGetResponse;
    uint8_t  bClassEnvelope;
    uint16_t wLcdLayout;
    uint8_t  bPINSupport;
    uint8_t  bMaxCCIDBusySlots;
    uint16_t bPadding;
} CCIDClassDescriptorStruct;

typedef struct {
    uint8_t bmFindexDindex;
    union {
        struct {
            uint8_t bmTCCKST0;
            uint8_t bGuardTimeT0;
            uint8_t bWaitingIntegerT0;
        };
        struct {
            uint8_t bmTCCKST1;
            uint8_t bGuardTimeT1;
            uint8_t bWaitingIntegerT1;
        };
    };
    uint8_t bClockStop;
    uint8_t bIFSC;
    uint8_t bNadValue;
} CCIDProtocolStruct;

typedef struct {
    uint8_t  bMessageType; /* Offset = 0*/
    uint32_t dwLength;     /* Offset = 1, The length field (dwLength) is the length of the message not including the 10-byte header.*/
    uint8_t  bSlot;        /* Offset = 5*/
    uint8_t  bSeq;         /* Offset = 6*/
    union {
        // 6.1.1 PC_to_RDR_IccPowerOn
        struct {
            uint8_t bPowerSelect;
            uint8_t abRFU2[2];
        };
        // 6.1.2 PC_to_RDR_IccPowerOff
        // 6.1.3 PC_to_RDR_GetSlotStatus
        // 6.1.5 PC_to_RDR_GetParameters
        // 6.1.6 PC_to_RDR_ResetParameters
        // 6.1.13 PC_to_RDR_Abort
        uint8_t abRFU3[3];
        // 6.1.4 PC_to_RDR_XfrBlock
        // 6.1.8 PC_to_RDR_Escape
        // 6.1.11 PC_to_RDR_Secure
        struct {
            uint8_t  bBWI;
            uint16_t wLevelParameter;
            uint8_t  abData[CCID_MAX_COMMAND_DATA_SIZE];
            // For reference, the absolute maximum block size for a TPDU T=0 block is 260 bytes
            // (5 bytes command; 255 bytes data),
            // or for a TPDU T=1 block is 259 bytes,
            // or for a short APDU T=1 block is 261 bytes,
            // or for an extended APDU T=1 block is 65544 bytes.
        };
        // 6.1.7 PC_to_RDR_SetParameters
        struct {
            uint8_t bProtocolNum;
            uint8_t abRFU2_7[2];

            CCIDProtocolStruct abProtocolDataStructure;
        };
        // 6.1.9 PC_to_RDR_IccClock
        struct {
            uint8_t bClockCommand;
            uint8_t abRFU2_9[2];
        };
        // 6.1.10 PC_to_RDR_T0APDU
        struct {
            uint8_t bmChanges;
            uint8_t bClassGetResponse;
            uint8_t bClassEnvelope;
        };
        // 6.1.12 PC_to_RDR_Mechanical
        struct {
            uint8_t bFunction;
            uint8_t abRFU2_10[2];
        };
        // 6.1.14 PC_to_RDR_SetDataRateAndClockFrequency
        struct {
            uint8_t  abRFU3_14[3];
            uint32_t dwClockFrequency;
            uint32_t dwDataRate;
        };
    };

} CCIDCommandStruct;

typedef struct {
    uint8_t  bMessageType; /* Offset = 0*/
    uint32_t dwLength;     /* Offset = 1*/
    uint8_t  bSlot;        /* Offset = 5, Same as Bulk-OUT message */
    uint8_t  bSeq;         /* Offset = 6, Same as Bulk-OUT message */
    union {
        uint8_t bStatus; /* Offset = 7, Slot status as defined in § 6.2.6*/
        struct {
            uint8_t bmICCStatus : 2;
            uint8_t bmRFU : 4;
            uint8_t bmCommandStatus : 2;
        };
    };
    uint8_t bError; /* Offset = 8, Slot error  as defined in § 6.2.6*/
    union {
        // 6.2.1 RDR_to_PC_DataBlock
        // 6.2.4 RDR_to_PC_Escape
        struct {
            uint8_t bChainParameter;
            uint8_t abData[CCID_MAX_RESPONSE_DATA_SIZE];
        };
        // 6.2.2 RDR_to_PC_SlotStatus
        uint8_t bClockStatus;
        // 6.2.3 RDR_to_PC_Parameters
        struct {
            uint8_t bProtocolNum;

            CCIDProtocolStruct abProtocolDataStructure;
        };
        // 6.2.5 RDR_to_PC_DataRateAndClockFrequency
        struct {
            uint8_t  bRFU;
            uint32_t dwClockFrequency;
            uint32_t dwDataRate;
        };
    };
} CCIDResponseStruct;

typedef struct {
    uint8_t bMessageType;
    uint8_t bmSlotICCState;
} CCIDNotificationStruct;
#pragma pack()

typedef struct InterfaceCCIDStruct_s InterfaceCCIDStruct;

extern bool InterfaceCCIDConstractor(InterfaceCCIDStruct* self);

struct InterfaceCCIDStruct_s {
    USBInterfaceStruct base;
    union {
        EPConfigStruct InterfaceEPConfig[2];
        struct {
            EPConfigStruct RxEPConfig;
            EPConfigStruct TxEPConfig;
#ifdef CCID_SUPPORT_NOTIFICATION
            EPConfigStruct IntEPConfig;
#endif
        };
    };

    void (*Init)(InterfaceCCIDStruct* self);
    bool (*IEPCallback)(InterfaceCCIDStruct* self, uint32_t* buffer, uint32_t bytesWritten);
    bool (*OEPCallback)(InterfaceCCIDStruct* self, uint32_t* buffer, uint32_t bytesRead);

#ifdef CCID_SUPPORT_NOTIFICATION
    bool (*IntIEPCallback)(InterfaceCCIDStruct* self, uint32_t* buffer, uint32_t bytesWritten);
#endif

    CCIDCardStatusEnum (*GetCardStatusCallback)(InterfaceCCIDStruct* self, uint8_t slot);
    uint8_t (*IccPowerOnCallback)(InterfaceCCIDStruct* self, uint8_t slot, uint8_t bPowerSelect, uint8_t* atrData, uint32_t* atrLength);
    uint8_t (*IccPowerOffCallback)(InterfaceCCIDStruct* self, uint8_t slot);
    uint8_t (*TransferBlockCallback)(InterfaceCCIDStruct* self, uint8_t slot, uint8_t* readData, uint32_t readLength, uint8_t* sendData, uint32_t* sendLength);
    uint8_t (*SetParametersCallback)(InterfaceCCIDStruct* self, uint8_t slot, CCIDProtocolStruct* protocolDataToSet, uint8_t bPRotocolNum);
    uint8_t (*GetParametersCallback)(InterfaceCCIDStruct* self, uint8_t slot, CCIDProtocolStruct* protocolDataToGet, uint8_t* bProtocolNum);
    uint8_t (*ExecuteEscapeCallback)(InterfaceCCIDStruct* self, uint8_t slot, uint8_t* readData, uint32_t readLength, uint8_t* sendData, uint32_t* sendLength);
    uint8_t (*SetClockCallback)(InterfaceCCIDStruct* self, uint8_t slot, uint8_t bClockCommand);
    uint8_t (*T0ApduCallback)(InterfaceCCIDStruct* self, uint8_t bmChanges, uint8_t bClassGetResponse, uint8_t bClassEnvelope);
    uint8_t (*MechanicalCallback)(InterfaceCCIDStruct* self, uint8_t slot, uint8_t bFunction);
    uint8_t (*SetDataRateAndClockFrequencyCallback)(InterfaceCCIDStruct* self, uint8_t slot, uint32_t dwClockFrequency, uint32_t dwDataRate);
    uint8_t (*GetDataRateAndClockFrequencyCallback)(InterfaceCCIDStruct* self, uint8_t slot, uint32_t* dwClockFrequency, uint32_t* dwDataRate);

    CCIDStateEnum State;

    CCIDClassDescriptorStruct ClassDescriptor;

    union {
        CCIDCommandStruct* Command;
        uint32_t*          CommandBuffer;
    };

    union {
        CCIDResponseStruct* Response;
        uint32_t*           ResponseBuffer;
    };
};

#endif
