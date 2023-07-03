#ifndef __INTERFACESERIAL_H__
#define __INTERFACESERIAL_H__

#include "USBInterface.h"
#include "Block.h"
#include "CircularBuffer.h"

// Default Endpoint Config Start
// Can be overrode after InterfaceConstractor(interface) and before interface->Init()
#ifndef SERIAL_RX_EP
#define SERIAL_RX_EP 2
#endif

#ifndef SERIAL_TX_EP
#define SERIAL_TX_EP 2
#endif

#ifndef SERIAL_MAX_PACKET
#define SERIAL_MAX_PACKET 64
#endif

#ifndef SERIAL_MAX_RX_SIZE
#define SERIAL_MAX_RX_SIZE 1024
#endif

#ifndef SERIAL_RX_TIMEOUT
#define SERIAL_RX_TIMEOUT 3 // tick (1ms/tick)
#endif
// Default Endpoint Config End

#ifndef SERIAL_TX_QUEUE_LENGTH
#define SERIAL_TX_QUEUE_LENGTH 16
#endif

typedef struct InterfaceSerialStruct_s InterfaceSerialStruct;

extern bool InterfaceSerialConstractor(InterfaceSerialStruct* self);

struct InterfaceSerialStruct_s {
    USBInterfaceStruct base;
    union {
        EPConfigStruct InterfaceEPConfig[2];
        struct {
            EPConfigStruct RxEPConfig;
            EPConfigStruct TxEPConfig;
        };
    };

    // Public functions Start
    void (*Init)(InterfaceSerialStruct* self);

    // Application Push Buffer
    void (*StartProbe)(InterfaceSerialStruct* self);
    bool (*SendPush)(InterfaceSerialStruct* self, uint32_t* buffer, uint32_t bytesToSend, void* handle, void (*popCallback)(BlockStruct* block));

#ifdef SERIAL_SUPPORT_SEND_BYTES
    // Application Push Byte
    uint32_t (*SendPushByte)(InterfaceSerialStruct* self, uint8_t* buffer, uint32_t maxPushSize, bool isPartialPush);
    uint32_t (*SendBufferSize)(InterfaceSerialStruct* self, bool isFreeSize);
#endif // SERIAL_SUPPORT_SEND_BYTES
#ifdef SERIAL_SUPPORT_READ_BYTES
    uint32_t (*ReadPopByte)(InterfaceSerialStruct* self, uint8_t* buffer, uint32_t maxPopSize, bool isPartialPop);
    uint32_t (*ReadBufferSize)(InterfaceSerialStruct* self, bool isFreeSize);
#endif
    // public functions End

    // Public Callback Start
    // Application Read Callback
    void (*ReadCallback)(InterfaceSerialStruct* self, uint32_t* buffer, uint32_t bytesRead);
    void (*ConnectCallback)(InterfaceSerialStruct* self, bool isConnected);
    // Public Callback End

    // Send buffer is backup buffer, used when send data from flash / unaligned (4-byte) data
    uint32_t SendBuffer[ToUintSize(SERIAL_MAX_PACKET)];
    uint32_t ReadBuffer[ToUintSize(SERIAL_MAX_RX_SIZE)];

    BlockStruct SendQueueBlock[SERIAL_TX_QUEUE_LENGTH];
    uint32_t    SendStartBlock;
    uint32_t    SendBlockCount;
    uint32_t    SentSize;

#ifdef SERIAL_SUPPORT_SEND_BYTES
    uint32_t* SendByteBuffer;
    uint32_t  SendByteBufferSize;

    CircularBufferStruct SendCircularBuffer;
#endif // SERIAL_SUPPORT_SEND_BYTES

#ifdef SERIAL_SUPPORT_READ_BYTES
    uint32_t* ReadByteBuffer;
    uint32_t  ReadByteBufferSize;

    CircularBufferStruct ReadCircularBuffer;
#endif // SERIAL_SUPPORT_READ_BYTES

    volatile uint8_t ProbeTimeout;
    volatile uint8_t ProbePeriod;

    volatile bool IsConnected;
};

#endif
