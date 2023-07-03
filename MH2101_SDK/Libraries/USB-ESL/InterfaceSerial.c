#include "InterfaceSerial.h"
#include "USBDevice.h"

static void Init(InterfaceSerialStruct* self) {
    USBInterfaceStruct* base = (USBInterfaceStruct*)self;

    base->InterfaceExtendDescriptorSize = 0;
    base->Init(base, 0x0A, 0x00, 0x00, 2, self->InterfaceEPConfig);

    self->SendStartBlock = 0;
    self->SendBlockCount = 0;

    self->ProbeTimeout = 0;
    self->ProbePeriod  = 0;

    self->IsConnected = false;

#ifdef SERIAL_SUPPORT_SEND_BYTES
    CircularBufferConstractor(&self->SendCircularBuffer, 4);
    self->SendCircularBuffer.Init(&self->SendCircularBuffer, (uint8_t*)self->SendByteBuffer, self->SendByteBufferSize);
#endif // SERIAL_SUPPORT_SEND_BYTES

#ifdef SERIAL_SUPPORT_READ_BYTES
    CircularBufferConstractor(&self->ReadCircularBuffer, 0);
    self->ReadCircularBuffer.Init(&self->ReadCircularBuffer, (uint8_t*)self->ReadByteBuffer, self->ReadByteBufferSize);
#endif // SERIAL_SUPPORT_READ_BYTES
}

void StartProbe(InterfaceSerialStruct* self) {
    if (self->ProbeTimeout)
        return;

    self->ProbePeriod = 0;
    USBHALStruct* hal = ((USBInterfaceStruct*)self)->Device->HAL;
    if (hal->EndpointStartProbe(hal, self->TxEPConfig.Index)) {
        self->ProbeTimeout = self->TxEPConfig.Timeout * 3;
    }
}

static bool SendPush(InterfaceSerialStruct* self, uint32_t* buffer, uint32_t bytesToSend, void* handle, void (*popCallback)(BlockStruct* block)) {
    if (!self->IsConnected)
        return false;

    if (self->SendBlockCount >= SERIAL_TX_QUEUE_LENGTH)
        return false;

    __disable_irq(); // Mask IRQ

    BlockStruct* newBlock = &self->SendQueueBlock[(self->SendStartBlock + self->SendBlockCount) % SERIAL_TX_QUEUE_LENGTH];
    newBlock->Buffer      = buffer;
    newBlock->Size        = bytesToSend;
    newBlock->PopCallback = popCallback;
    newBlock->Handle      = handle;
    newBlock->Locked      = false;

    self->SendBlockCount++;

    __enable_irq(); // UnMask IRQ
    return true;
}

#ifdef SERIAL_SUPPORT_SEND_BYTES
static uint32_t SendPushByte(InterfaceSerialStruct* self, uint8_t* buffer, uint32_t maxPushSize, bool isPartialPush) {
    if (!self->IsConnected)
        return 0;
    uint32_t pushSize = self->SendCircularBuffer.Push(&self->SendCircularBuffer, buffer, maxPushSize, isPartialPush);
    return pushSize;
}

static void SentByte(BlockStruct* block) {
    ((CircularBufferStruct*)block->Handle)->EndPop(((CircularBufferStruct*)block->Handle), block->Size);
}

static uint32_t SendBufferSize(InterfaceSerialStruct* self, bool isFreeSize) {
    return isFreeSize ? CircularBufferFreeSize(&self->SendCircularBuffer) : CircularBufferUsedSize(&self->SendCircularBuffer);
}
#endif // SERIAL_SUPPORT_SEND_BYTES

#ifdef SERIAL_SUPPORT_READ_BYTES
static uint32_t ReadPopByte(InterfaceSerialStruct* self, uint8_t* buffer, uint32_t maxPopSize, bool isPartialPop) {
    // __disable_irq();
    uint32_t popSize = self->ReadCircularBuffer.Pop(&self->ReadCircularBuffer, buffer, maxPopSize, isPartialPop);
    // __enable_irq();
    return popSize;
}

static uint32_t ReadBufferSize(InterfaceSerialStruct* self, bool isFreeSize) {
    return isFreeSize ? CircularBufferFreeSize(&self->ReadCircularBuffer) : CircularBufferUsedSize(&self->ReadCircularBuffer);
}
#endif

static inline bool IsSendReady(InterfaceSerialStruct* self) {
    USBDeviceStruct* device = ((USBInterfaceStruct*)self)->Device;
    return device->IsConfigured(device) && device->HAL->GetEPStatus(device->HAL, self->TxEPConfig.Index, EPDirectIn) == EPCompleted;
}

static inline bool SendStartPop(InterfaceSerialStruct* self, USBHALStruct* hal) {
    if (!self->SendBlockCount)
        return false;

    BlockStruct* currentBlock = &self->SendQueueBlock[self->SendStartBlock];
    // block busy
    if (currentBlock->Locked)
        return false;

    bool writeResult = hal->EndpointStartWrite(hal, self->TxEPConfig.Index, currentBlock->Buffer, currentBlock->Size);
    if (writeResult) {
        self->SentSize       = 0;
        currentBlock->Locked = true;
    }
    return true;
}

static bool AsyncTask(InterfaceSerialStruct* self) {
    USBHALStruct* hal = ((USBInterfaceStruct*)self)->Device->HAL;

    if (self->ProbeTimeout) {
        if (!(--self->ProbeTimeout)) {
            if (self->IsConnected && self->ConnectCallback != NULL) {
                self->ConnectCallback(self, false);
                self->IsConnected = false;
            }
        }
    }
    else if (self->IsConnected && (++self->ProbePeriod >= self->TxEPConfig.Timeout)) {
        self->StartProbe(self);
    }

#ifdef SERIAL_SUPPORT_SEND_BYTES
    // Push send byte buffer task
    CircularBufferStruct* circular = &self->SendCircularBuffer;
    if ((SERIAL_TX_QUEUE_LENGTH - self->SendBlockCount >= 2) && circular->StartPop(circular, circular->Length, true, true)) {
        SendPush(self, (uint32_t*)(circular->Buffer + circular->PopOffset), circular->PopSize - circular->PopSplit, circular, SentByte);
        if (circular->PopSplit)
            SendPush(self, (uint32_t*)(circular->Buffer), circular->PopSplit, circular, SentByte);
    }
#endif // SERIAL_SUPPORT_SEND_BYTES

    // Send Task
    if (!IsSendReady(self))
        return false;

    return SendStartPop(self, hal);
}

static bool Sent(InterfaceSerialStruct* self, uint32_t* buffer, uint32_t bytesWritten) {
    if (!bytesWritten) {
        self->ProbeTimeout = 0;
        if (self->IsConnected)
            return true;
        self->IsConnected = true;
        if (self->ConnectCallback != NULL) {
            self->ConnectCallback(self, true);
        }
        return true;
    }

    if (!self->SendBlockCount)
        return true;

    self->SentSize += bytesWritten;

    BlockStruct* currentBlock = &self->SendQueueBlock[self->SendStartBlock];

    // Meano TODO: HAL will do this.
    USBHALStruct* hal = ((USBInterfaceStruct*)self)->Device->HAL;
    if (self->SentSize < currentBlock->Size) { // Only fired when block buffer is unaligned(4-byte)!
        return hal->EndpointStartWrite(hal, self->TxEPConfig.Index, (uint32_t*)((uint32_t)currentBlock->Buffer + self->SentSize),
                                       currentBlock->Size - self->SentSize);
    }

    // SendEndPop
    if (currentBlock->PopCallback) {
        currentBlock->PopCallback(currentBlock);
    }
    self->SendBlockCount--;
    self->SendStartBlock++;
    self->SendStartBlock %= SERIAL_TX_QUEUE_LENGTH;

    if (SendStartPop(self, hal))
        return true;

    // Auto send zero-length packet
    if (bytesWritten % self->TxEPConfig.MaxPacket == 0) {
        hal->EndpointStartWrite(hal, self->TxEPConfig.Index, NULL, 0);
    }

    return true;
}

static bool Read(InterfaceSerialStruct* self, uint32_t* buffer, uint32_t bytesRead) {
    if (self->ReadCallback) {
        self->ReadCallback(self, self->ReadBuffer, bytesRead);
    }

#ifdef SERIAL_SUPPORT_READ_BYTES
    uint32_t pushedSize = self->ReadCircularBuffer.Push(&self->ReadCircularBuffer, (uint8_t*)buffer, bytesRead, false);
    if (pushedSize < bytesRead) {
        printf_dbg("Readbuffer overflow, read %d, but pushed %d.\n", bytesRead, pushedSize);
        return false;
    }
#endif

    USBHALStruct* hal = ((USBInterfaceStruct*)self)->Device->HAL;
    hal->EndpointStartRead(hal, self->RxEPConfig.Index, self->ReadBuffer, self->RxEPConfig.MaxBuffer);

    return true;
}

bool InterfaceSerialConstractor(InterfaceSerialStruct* self) {
    USBInterfaceConstractor((USBInterfaceStruct*)self);

    self->Init       = Init;
    self->StartProbe = StartProbe;
    self->SendPush   = SendPush;

#ifdef SERIAL_SUPPORT_SEND_BYTES
    self->SendPushByte   = SendPushByte;
    self->SendBufferSize = SendBufferSize;
#endif // SERIAL_SUPPORT_SEND_BYTES
#ifdef SERIAL_SUPPORT_READ_BYTES
    self->ReadPopByte    = ReadPopByte;
    self->ReadBufferSize = ReadBufferSize;
#endif // SERIAL_SUPPORT_READ_BYTES

    self->base.SofCallback = (SofCallbackType)AsyncTask;

    // default RxEPConfig
    self->RxEPConfig = (EPConfigStruct) {
        .Index     = SERIAL_RX_EP,
        .Direct    = EPDirectOut,
        .Type      = EPTypeBulk,
        .MaxPacket = SERIAL_MAX_PACKET,
        .MaxBuffer = SERIAL_MAX_RX_SIZE,
        .Timeout   = SERIAL_RX_TIMEOUT,
        .Buffer    = self->ReadBuffer,
        .Callback  = (EPCallbackType)Read,
    };

    // default TxEPConfig
    self->TxEPConfig = (EPConfigStruct) {
        .Index     = SERIAL_TX_EP,
        .Direct    = EPDirectIn,
        .Type      = EPTypeBulk,
        .MaxPacket = SERIAL_MAX_PACKET,
        .MaxBuffer = SERIAL_MAX_PACKET,
        .Buffer    = self->SendBuffer,
        .Callback  = (EPCallbackType)Sent,
    };

    return true;
}
