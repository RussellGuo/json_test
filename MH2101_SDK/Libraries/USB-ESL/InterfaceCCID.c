#include "InterfaceCCID.h"
#include "USBDevice.h"

typedef enum {
    CCIDCheckSlot        = BIT(0),
    CCIDCheckZeroLength  = BIT(1),
    CCIDCheckabRFU2      = BIT(2),
    CCIDCheckabRFU3      = BIT(3),
    CCIDCheckCardPresent = BIT(4),
    CCIDCheckAbort       = BIT(5),
    CCIDCheckActive      = BIT(6),
} CCIDCheckEnum;

static inline void CommandDecode(InterfaceCCIDStruct* self);

static void Init(InterfaceCCIDStruct* self) {
    USBInterfaceStruct* base = (USBInterfaceStruct*)self;

    util_assert(self->CommandBuffer != NULL);
    util_assert(self->ResponseBuffer != NULL);

    self->TxEPConfig.Buffer = self->ResponseBuffer;
    self->RxEPConfig.Buffer = self->CommandBuffer;

#ifdef CCID_SUPPORT_NOTIFICATION
    self->IntEPConfig.Buffer = self->ResponseBuffer;
#endif

    base->Init(base, CCID_CLASS, 0x00, 0x00, 2, self->InterfaceEPConfig);
}

static inline bool Send(InterfaceCCIDStruct* self, uint32_t* data, uint32_t length) {
    self->State       = CCIDStateSendResponse;
    USBHALStruct* hal = ((USBInterfaceStruct*)self)->Device->HAL;
    return hal->EndpointStartWrite(hal, self->TxEPConfig.Index, (uint32_t*)data, length);
}

static bool IEPCallback(InterfaceCCIDStruct* self, uint32_t* buffer, uint32_t bytesWritten) {
    switch (self->State) {
        case CCIDStateSendResponse:
            if (bytesWritten > 0 && bytesWritten % self->TxEPConfig.MaxPacket == 0) {
                Send(self, self->ResponseBuffer, 0);
                break;
            }
            self->State = CCIDStateIdle;
            break;
        default:
            break;
    }
    return true;
}

static bool OEPCallback(InterfaceCCIDStruct* self, uint32_t* buffer, uint32_t bytesRead) {
    switch (self->State) {
        case CCIDStateIdle:
            if (bytesRead >= CCID_HEADER_SIZE) {
                if ((self->Command->dwLength + CCID_HEADER_SIZE) != bytesRead) {
                    return false;
                }
                CommandDecode(self);
            }
            break;
        default:
            break;
    }
    USBHALStruct* hal = ((USBInterfaceStruct*)self)->Device->HAL;
    hal->EndpointStartRead(hal, self->RxEPConfig.Index, self->RxEPConfig.Buffer, self->RxEPConfig.MaxBuffer);
    return true;
}

#ifdef CCID_SUPPORT_NOTIFICATION
bool IntIEPCallback(InterfaceCCIDStruct* self, uint32_t* buffer, uint32_t bytesWritten) {
    // TODO
    return true;
}

static inline bool Notify(InterfaceCCIDStruct* self, uint8_t* message, uint16_t length) {
    USBHALStruct* hal = ((USBInterfaceStruct*)self)->Device->HAL;
    return hal->EndpointStartWrite(hal, self->IntEPConfig.Index, (uint32_t*)message, length);
}
#endif

static inline bool CommandNotSupport(InterfaceCCIDStruct* self) {
    self->Response->bmCommandStatus = CCIDCommandFailed;
    self->Response->bError          = CCID_ERROR_CMD_NOT_SUPPORTED;
    return false;
}

static inline bool CommandResult(InterfaceCCIDStruct* self, uint8_t bError) {
    if (!bError)
        return true;
    self->Response->bmCommandStatus = CCIDCommandFailed;
    self->Response->bError          = bError;
    return false;
}

/**
 * @brief  CommandCheck
 *         Checks the specific parameters requested by the function and update
 *          status accordingly. This function is called from all
 *          PC_to_RDR functions
 * @param  uint32_t checkItems : Parameter enum to be checked by calling function
 * @retval bool : if check passed
 */
static bool CommandCheck(InterfaceCCIDStruct* self, uint32_t checkItems) {
    /* Slot requested is more than supported by Firmware */
    if (self->Command->bSlot > self->ClassDescriptor.bMaxSlotIndex) {
        self->Response->bmICCStatus = CCIDCardNotPresent;
        return CommandResult(self, CCID_ERROR_BAD_SLOT);
    }

    if ((checkItems & (CCIDCheckCardPresent | CCIDCheckActive)) && self->GetCardStatusCallback) {
        CCIDCardStatusEnum status = self->GetCardStatusCallback(self, self->Command->bSlot);
        if (status) {
            self->Response->bmICCStatus = status;
            return CommandResult(self, status == CCIDCardNotPresent ? CCID_ERROR_ICC_MUTE : CCID_ERROR_HW_ERROR);
        }
    }

    if ((checkItems & CCIDCheckZeroLength) && self->Command->dwLength != 0)
        return CommandResult(self, CCID_ERROR_BAD_LENTGH);

    /* abRFU2 : Reserved for Future Use*/
    if ((checkItems & CCIDCheckabRFU2) && (self->Command->abRFU2[0] | self->Command->abRFU2[1]))
        return CommandResult(self, CCID_ERROR_BAD_ABRFU_2B);

    /* abRFU3 : Reserved for Future Use*/
    if ((checkItems & CCIDCheckabRFU3) && (self->Command->abRFU3[0] | self->Command->abRFU3[1] | self->Command->abRFU3[2]))
        return CommandResult(self, CCID_ERROR_BAD_ABRFU_3B);

    /* TODO: Abort
    if ((checkItems & CCIDCheckAbort)) {
        self->Response->bmCommandStatus = CCIDCommandFailed;
        self->Response->bmICCStatus     = CCIDCardPresentInactive;
        self->Response->bError = CCID_ERROR_CMD_ABORTED;
    }
    */

    return true;
}

/**
 * @brief  PC_to_RDR_IccPowerOn
 *         PC_TO_RDR_ICCPOWERON message execution, apply voltage and get ATR
 * @param  None
 * @retval None
 */
static void PC_to_RDR_IccPowerOn(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    if (!CommandCheck(self, CCIDCheckSlot | CCIDCheckZeroLength | CCIDCheckabRFU2 | CCIDCheckCardPresent | CCIDCheckAbort))
        return;
    /*
    Apply the ICC VCC fills the Response buffer with ICC ATR
    This Command is returned with RDR_to_PC_DataBlock();
    */
    if (!self->IccPowerOnCallback) {
        // Set the default ATR
        self->Response->dwLength  = 4;
        self->Response->abData[0] = 0x3B;
        self->Response->abData[1] = 0x60;
        self->Response->abData[2] = 0x00;
        self->Response->abData[3] = 0x00;
        return;
    }

    uint32_t dwLength = 0;
    if (CommandResult(self, self->IccPowerOnCallback(self, command->bSlot, command->bPowerSelect, response->abData, &dwLength)))
        response->dwLength = dwLength;
}

/**
 * @brief  PC_to_RDR_IccPowerOff
 *         Icc VCC is switched Off
 * @param  None
 * @retval None
 */
static void PC_to_RDR_IccPowerOff(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    /*  The response to this command message is the RDR_to_PC_SlotStatus response message. */
    if (!CommandCheck(self, CCIDCheckSlot | CCIDCheckZeroLength | CCIDCheckabRFU3))
        return;

    if (!self->IccPowerOffCallback)
        return;

    CommandResult(self, self->IccPowerOffCallback(self, command->bSlot));
}

/**
 * @brief  PC_to_RDR_GetSlotStatus
 *         Provides the Slot status to the host
 * @param  None
 * @retval None
 */
static void PC_to_RDR_GetSlotStatus(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    CommandCheck(self, CCIDCheckSlot | CCIDCheckZeroLength | CCIDCheckabRFU3 | CCIDCheckCardPresent);
}

/**
 * @brief  PC_to_RDR_XfrBlock
 *         Handles the Block transfer from Host.
 *         Response to this command message is the RDR_to_PC_DataBlock
 * @param  None
 * @retval None
 */
static void PC_to_RDR_XfrBlock(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    if (!CommandCheck(self, CCIDCheckSlot | CCIDCheckCardPresent | CCIDCheckActive | CCIDCheckabRFU3 | CCIDCheckAbort))
        return;

    /* Check amount of Data Sent by Host is > than memory allocated ? */
    if (command->dwLength > CCID_MAX_COMMAND_DATA_SIZE) {
        CommandResult(self, CCID_ERROR_BAD_DWLENGTH);
        return;
    }

    // no error if Callback not set
    if (!self->TransferBlockCallback)
        return;

    uint32_t dwLength = 0;
    if (CommandResult(self, self->TransferBlockCallback(self, command->bSlot, command->abData, command->dwLength, response->abData, &dwLength)))
        self->Response->dwLength = dwLength;
}

/**
 * @brief  PC_to_RDR_GetParameters
 *         Provides the ICC parameters to the host
 *         Response to this command message is the RDR_to_PC_Parameters
 * @param  None
 * @retval None
 */
static void PC_to_RDR_GetParameters(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    CommandCheck(self, CCIDCheckSlot | CCIDCheckZeroLength | CCIDCheckCardPresent | CCIDCheckabRFU3);
}

/**
 * @brief  PC_to_RDR_ResetParameters
 *         Set the ICC parameters to the default
 *         Response to this command message is the RDR_to_PC_Parameters
 * @param  None
 * @retval None
 */
static void PC_to_RDR_ResetParameters(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    if (!CommandCheck(self, CCIDCheckSlot | CCIDCheckZeroLength | CCIDCheckCardPresent | CCIDCheckabRFU3 | CCIDCheckActive))
        return;

    if (!self->SetParametersCallback) {
        CommandNotSupport(self);
        return;
    }

    CommandResult(self, self->SetParametersCallback(self, command->bSlot, NULL, NULL));
}

/**
 * @brief  PC_to_RDR_SetParameters
 *         Set the ICC parameters to the host defined parameters
 *         Response to this command message is the RDR_to_PC_Parameters
 * @param  None
 * @retval None
 */
static void PC_to_RDR_SetParameters(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    if (!CommandCheck(self, CCIDCheckSlot | CCIDCheckCardPresent | CCIDCheckabRFU2 | CCIDCheckActive))
        return;

    uint8_t bError = CCID_ERROR_OK;

    if (command->bProtocolNum == BPROTOCOL_NUM_T0) {
        /* for Protocol T=0 (bProtocolNum=0) (dwLength=00000005h) */
        if (command->dwLength != 5)
            bError = CCID_ERROR_BAD_LENTGH;
        /* For T0, Waiting Integer 0 supported */
        // else if (command->abProtocolDataStructure.bWaitingIntegerT0 != 0)
        //     bError = CCID_ERROR_BAD_WAITINGINTEGER;
    }
    else if (command->bProtocolNum == BPROTOCOL_NUM_T1) {
        /* for Protocol T=1 (bProtocolNum=1) (dwLength=00000007h) */
        if (command->dwLength != 7)
            bError = CCID_ERROR_BAD_LENTGH;
    }
    else {
        bError = CCID_ERROR_BAD_PROTOCOLNUM;
    }

    if (command->abProtocolDataStructure.bClockStop != DEFAULT_CLOCKSTOP)
        bError = CCID_ERROR_BAD_CLOCKSTOP;

    if (!CommandResult(self, bError))
        return;

    if (!self->SetParametersCallback) {
        // CommandNotSupport(self);
        return;
    }

    CommandResult(self, self->SetParametersCallback(self, command->bSlot, &command->abProtocolDataStructure, command->bProtocolNum));
}

/**
 * @brief  PC_to_RDR_Escape
 *         Execute the Escape command. This is user specific Implementation
 *         Response to this command message is the RDR_to_PC_Escape
 * @param  None
 * @retval None
 */
static void PC_to_RDR_Escape(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    if (!CommandCheck(self, CCIDCheckSlot | CCIDCheckCardPresent | CCIDCheckabRFU3 | CCIDCheckAbort | CCIDCheckActive))
        return;

    if (!self->ExecuteEscapeCallback) {
        CommandNotSupport(self);
        return;
    }

    uint32_t dwLength = 0;
    if (CommandResult(self, self->ExecuteEscapeCallback(self, command->bSlot, command->abData, command->dwLength, response->abData, &dwLength)))
        response->dwLength = dwLength;
}

/**
 * @brief  PC_to_RDR_IccClock
 *         Execute the Clock specific command from host
 *         Response to this command message is the RDR_to_PC_SlotStatus
 * @param  None
 * @retval None
 */
static void PC_to_RDR_IccClock(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    if (!CommandCheck(self, CCIDCheckSlot | CCIDCheckCardPresent | CCIDCheckabRFU2 | CCIDCheckZeroLength | CCIDCheckActive))
        return;

    /*
    bClockCommand
      * 00h restarts Clock
      * 01h Stops Clock in the state shown in the bClockStop
        field of the PC_to_RDR_SetParameters command
        and RDR_to_PC_Parameters message.
    */
    if (command->bClockCommand > 1) {
        CommandResult(self, CCID_ERROR_BAD_CLOCKCOMMAND);
        return;
    }

    if (!self->SetClockCallback) {
        CommandNotSupport(self);
        return;
    }

    CommandResult(self, self->SetClockCallback(self, command->bSlot, command->bClockCommand));
}

/**
 * @brief  PC_to_RDR_Abort
 *         Execute the Abort command from host, This stops all Bulk transfers
 *         from host and ICC
 *         Response to this command message is the RDR_to_PC_SlotStatus
 * @param  None
 * @retval None
 */
static void PC_to_RDR_Abort(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    if (!CommandCheck(self, CCIDCheckSlot | CCIDCheckabRFU3 | CCIDCheckZeroLength))
        return;
    // TODO: SetAbortCallback
    CommandNotSupport(self);
}

/**
 * @brief  PC_to_RDR_T0Apdu
 *         Execute the PC_TO_RDR_T0APDU command from host
 *         Response to this command message is the RDR_to_PC_SlotStatus
 * @param  None
 * @retval None
 */
static void PC_to_RDR_T0Apdu(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    if (!CommandCheck(self, CCIDCheckSlot | CCIDCheckCardPresent | CCIDCheckZeroLength | CCIDCheckAbort))
        return;
    /*
    Bit 0 is associated with field bClassGetResponse
    Bit 1 is associated with field bClassEnvelope
    Other bits are RFU.
    */
    if (command->bmChanges > 0x03) {
        CommandResult(self, CCID_ERROR_BAD_BMCHANGES);
        return;
    }

    if (!self->T0ApduCallback) {
        CommandNotSupport(self);
        return;
    }

    CommandResult(self, self->T0ApduCallback(self, command->bmChanges, command->bClassGetResponse, command->bClassEnvelope));
}

/**
 * @brief  PC_to_RDR_Mechanical
 *         Execute the PC_TO_RDR_MECHANICAL command from host
 *         Response to this command message is the RDR_to_PC_SlotStatus
 * @param  None
 * @retval None
 */
static void PC_to_RDR_Mechanical(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    if (!CommandCheck(self, CCIDCheckSlot | CCIDCheckCardPresent | CCIDCheckabRFU2 | CCIDCheckZeroLength))
        return;

    /*
    01h – Accept Card
    02h – Eject Card
    03h – Capture Card
    04h – Lock Card
    05h – Unlock Card
    */
    if (command->bFunction > 0x05) {
        CommandResult(self, CCID_ERROR_BAD_BFUNCTION_MECHANICAL);
        return;
    }

    if (!self->MechanicalCallback) {
        CommandNotSupport(self);
        return;
    }

    CommandResult(self, self->MechanicalCallback(self, command->bSlot, command->bFunction));
}

/**
 * @brief  PC_TO_RDR_SetDataRateAndClockFrequency
 *         Set the required Card Frequency and Data rate from the host.
 *         Response to this command message is the
 *         RDR_to_PC_DataRateAndClockFrequency
 * @param  None
 * @retval None
 */
static void PC_to_RDR_SetDataRateAndClockFrequency(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    if (!CommandCheck(self, CCIDCheckSlot | CCIDCheckCardPresent | CCIDCheckabRFU3))
        return;

    if (command->dwLength != 0x08) {
        CommandResult(self, CCID_ERROR_BAD_LENTGH);
        return;
    }

    if (!self->SetDataRateAndClockFrequencyCallback) {
        CommandNotSupport(self);
        return;
    }

    CommandResult(self, self->SetDataRateAndClockFrequencyCallback(self, command->bSlot, command->dwClockFrequency, command->dwDataRate));
}

/**
 * @brief  PC_TO_RDR_Secure
 *         Execute the Secure Command from the host.
 *         Response to this command message is the RDR_to_PC_DataBlock
 * @param  None
 * @retval None
 */
static void PC_to_RDR_Secure(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    if (!CommandCheck(self, CCIDCheckSlot | CCIDCheckCardPresent | CCIDCheckAbort))
        return;

    /* TPDU level & short APDU level, wLevelParameter is RFU, = 0000h */
    if (command->wLevelParameter != 0) {
        CommandResult(self, CCID_ERROR_BAD_LEVELPARAMETER);
        return;
    }

    CommandNotSupport(self);
}

/**
 * @brief  RDR_to_PC_DataBlock
 *         Provide the data block response to the host
 *         Response for PC_to_RDR_IccPowerOn, PC_to_RDR_XfrBlock
 * @param  uint8_t bError: code to be returned to the host
 * @retval None
 */
static void RDR_to_PC_DataBlock(InterfaceCCIDStruct* self) {
    self->Response->bMessageType    = RDR_TO_PC_DATABLOCK;
    self->Response->bChainParameter = 0;
}

/**
 * @brief  RDR_to_PC_SlotStatus
 *         Provide the Slot status response to the host
 *          Response for PC_to_RDR_IccPowerOff
 *                PC_to_RDR_GetSlotStatus
 *                PC_to_RDR_IccClock
 *                PC_to_RDR_T0APDU
 *                PC_to_RDR_Mechanical
 *         Also the device sends this response message when it has completed
 *          aborting a slot after receiving both the Class Specific ABORT request
 *          and PC_to_RDR_Abort command message.
 * @param  uint8_t bError: code to be returned to the host
 * @retval None
 */
static void RDR_to_PC_SlotStatus(InterfaceCCIDStruct* self) {
    /*
        bClockStatus =
            00h Clock running
            01h Clock stopped in state L
            02h Clock stopped in state H
            03h Clock stopped in an unknown state
            All other values are RFU.
    */
    self->Response->bMessageType = RDR_TO_PC_SLOTSTATUS;
    self->Response->dwLength     = 0;
    self->Response->bClockStatus = 0x00;
}

/**
 * @brief  RDR_to_PC_Parameters
 *         Provide the data block response to the host
 *         Response for PC_to_RDR_GetParameters, PC_to_RDR_ResetParameters
 *                      PC_to_RDR_SetParameters
 * @param  uint8_t bError: code to be returned to the host
 * @retval None
 */
static void RDR_to_PC_Parameters(InterfaceCCIDStruct* self, CCIDCommandStruct* command, CCIDResponseStruct* response) {
    self->Response->bMessageType = RDR_TO_PC_PARAMETERS;

    // send default parameter if Callback not set
    if (!self->GetParametersCallback) {
        response->bProtocolNum = BPROTOCOL_NUM_T0;
        response->dwLength     = LEN_PROTOCOL_STRUCT_T0;

        response->abProtocolDataStructure = (CCIDProtocolStruct) {
            .bmFindexDindex    = DEFAULT_FIDI,
            .bmTCCKST0         = DEFAULT_T01CONVCHECKSUM,
            .bGuardTimeT0      = DEFAULT_EXTRA_GUARDTIME,
            .bWaitingIntegerT0 = DEFAULT_WAITINGINTEGER,
            .bClockStop        = DEFAULT_CLOCKSTOP,
            .bIFSC             = DEFAULT_IFSC,
            .bNadValue         = DEFAULT_NAD,
        };
        return;
    }

    if (CommandResult(self, self->GetParametersCallback(self, command->bSlot, &response->abProtocolDataStructure, &response->bProtocolNum)))
        response->dwLength = response->bProtocolNum ? LEN_PROTOCOL_STRUCT_T1 : LEN_PROTOCOL_STRUCT_T0;
}

/**
 * @brief  RDR_to_PC_Escape
 *         Provide the Escaped data block response to the host
 *         Response for PC_to_RDR_Escape
 * @param  uint8_t bError: code to be returned to the host
 * @retval None
 */
static void RDR_to_PC_Escape(InterfaceCCIDStruct* self) {
    self->Response->bMessageType = RDR_TO_PC_ESCAPE;
    self->Response->bRFU         = 0;
}

/**
 * @brief  RDR_to_PC_DataRateAndClockFrequency
 *         Provide the Clock and Data Rate information to host
 *         Response for PC_TO_RDR_SetDataRateAndClockFrequency
 * @param  uint8_t bError: code to be returned to the host
 * @retval None
 */
static void RDR_to_PC_DataRateAndClockFrequency(InterfaceCCIDStruct* self) {
    self->Response->bMessageType = RDR_TO_PC_DATARATEANDCLOCKFREQUENCY;
    self->Response->bRFU         = 0;

    if (!self->GetDataRateAndClockFrequencyCallback) {
        // TODO: command not support
        return;
    }

    uint32_t dwClockFrequency, dwDataRate;
    if (self->GetDataRateAndClockFrequencyCallback(self, self->Command->bSlot, &dwClockFrequency, &dwDataRate)) {
        self->Response->dwClockFrequency = dwClockFrequency;
        self->Response->dwDataRate       = dwDataRate;
        return;
    }
}

#ifdef CCID_SUPPORT_NOTIFICATION
/**
 * @brief  RDR_to_PC_NotifySlotChange
 *         Interrupt message to be sent to the host, Checks the card presence
 *           status and update the buffer accordingly
 * @param  None
 * @retval None
 */
static void RDR_to_PC_NotifySlotChange(InterfaceCCIDStruct* self) {
    // TODO
}
#endif // CCID_SUPPORT_NOTIFICATION

static inline void CommandDecode(InterfaceCCIDStruct* self) {
    CCIDCommandStruct*  command  = self->Command;
    CCIDResponseStruct* response = self->Response;
    // Reset Response
    response->bSeq     = command->bSeq;
    response->bSlot    = command->bSlot;
    response->dwLength = 0;
    response->bStatus  = 0;
    response->bError   = CCID_ERROR_OK;

    // Decode bMessageType
    switch (command->bMessageType) {
        case PC_TO_RDR_ICCPOWERON:
            PC_to_RDR_IccPowerOn(self, command, response);
            RDR_to_PC_DataBlock(self);
            break;
        case PC_TO_RDR_ICCPOWEROFF:
            PC_to_RDR_IccPowerOff(self, command, response);
            RDR_to_PC_SlotStatus(self);
            break;
        case PC_TO_RDR_GETSLOTSTATUS:
            PC_to_RDR_GetSlotStatus(self, command, response);
            RDR_to_PC_SlotStatus(self);
            break;
        case PC_TO_RDR_XFRBLOCK:
            PC_to_RDR_XfrBlock(self, command, response);
            RDR_to_PC_DataBlock(self);
            break;
        case PC_TO_RDR_GETPARAMETERS:
            PC_to_RDR_GetParameters(self, command, response);
            RDR_to_PC_Parameters(self, command, response);
            break;
        case PC_TO_RDR_RESETPARAMETERS:
            PC_to_RDR_ResetParameters(self, command, response);
            RDR_to_PC_Parameters(self, command, response);
            break;
        case PC_TO_RDR_SETPARAMETERS:
            PC_to_RDR_SetParameters(self, command, response);
            RDR_to_PC_Parameters(self, command, response);
            break;
        case PC_TO_RDR_ESCAPE:
            PC_to_RDR_Escape(self, command, response);
            RDR_to_PC_Escape(self);
            break;
        case PC_TO_RDR_ICCCLOCK:
            PC_to_RDR_IccClock(self, command, response);
            RDR_to_PC_SlotStatus(self);
            break;
        case PC_TO_RDR_ABORT:
            PC_to_RDR_Abort(self, command, response);
            RDR_to_PC_SlotStatus(self);
            break;
        case PC_TO_RDR_T0APDU:
            PC_to_RDR_T0Apdu(self, command, response);
            RDR_to_PC_SlotStatus(self);
            break;
        case PC_TO_RDR_MECHANICAL:
            PC_to_RDR_Mechanical(self, command, response);
            RDR_to_PC_SlotStatus(self);
            break;
        case PC_TO_RDR_SETDATARATEANDCLOCKFREQUENCY:
            PC_to_RDR_SetDataRateAndClockFrequency(self, command, response);
            RDR_to_PC_DataRateAndClockFrequency(self);
            break;
        case PC_TO_RDR_SECURE:
            PC_to_RDR_Secure(self, command, response);
            RDR_to_PC_DataBlock(self);
            break;
        default:
            CommandNotSupport(self);
            RDR_to_PC_SlotStatus(self);
            break;
    }

    Send(self, self->ResponseBuffer, CCID_HEADER_SIZE + response->dwLength);
}

bool InterfaceCCIDConstractor(InterfaceCCIDStruct* self) {
    USBInterfaceConstractor((USBInterfaceStruct*)self);

    self->ClassDescriptor = (CCIDClassDescriptorStruct) {
        .bLength                = CCID_DESCRIPTOR_SIZE,
        .bDescriptorType        = CCID_DESCRIPTOR_TYPE,
        .bcdCCID                = CCID_VERSION,
        .bMaxSlotIndex          = 0x00,
        .bVoltageSupport        = 0x03,
        .dwProtocols            = 0x00000001,
        .dwDefaultClock         = DEFAULT_CLOCK_FREQ,
        .dwMaximumClock         = DEFAULT_CLOCK_FREQ,
        .bNumClockSupported     = 0x00,
        .dwDataRate             = DEFAULT_DATA_RATE,
        .dwMaxDataRate          = DEFAULT_DATA_RATE,
        .bNumDataRatesSupported = 0x00,
        .dwMaxIFSD              = 0x00000000,
        .dwSynchProtocols       = 0x00000000,
        .dwMechanical           = 0x00000000,
        .dwFeatures             = 0x00010038,
        .dwMaxCCIDMessageLength = MAX(CCID_MAX_COMMAND_BUFFER, CCID_MAX_RESPONSE_BUFFER),
        .bClassGetResponse      = 0x00,
        .bClassEnvelope         = 0x00,
        .wLcdLayout             = 0x0000,
        .bPINSupport            = 0x00,
        .bMaxCCIDBusySlots      = 0x01,
    };

    self->Init        = Init;
    self->IEPCallback = IEPCallback;
    self->OEPCallback = OEPCallback;

    self->TxEPConfig = (EPConfigStruct) {
        .Index     = CCID_TX_EP,
        .Direct    = EPDirectIn,
        .Type      = EPTypeBulk,
        .MaxPacket = CCID_MAX_PACKET,
        .MaxBuffer = CCID_MAX_RESPONSE_BUFFER,
        .Buffer    = NULL,
        .Callback  = (EPCallbackType)self->IEPCallback,
    };

    self->RxEPConfig = (EPConfigStruct) {
        .Index     = CCID_RX_EP,
        .Direct    = EPDirectOut,
        .Type      = EPTypeBulk,
        .MaxPacket = CCID_MAX_PACKET,
        .MaxBuffer = CCID_MAX_COMMAND_BUFFER,
        .Buffer    = NULL,
        .Callback  = (EPCallbackType)self->OEPCallback,
    };

#ifdef CCID_SUPPORT_NOTIFICATION
    self->IntIEPCallback = IntIEPCallback;

    self->IntEPConfig = (EPConfigStruct) {
        .Index     = CCID_INT_EP,
        .Direct    = EPDirectIn,
        .Type      = EPTypeInterrupt,
        .MaxPacket = CCID_MAX_PACKET,
        .MaxBuffer = CCID_MAX_RESPONSE_BUFFER,
        .Buffer    = NULL,
        .Callback  = (EPCallbackType)self->IntIEPCallback,
    };
#endif

    self->base.InterfaceExtendDescriptorSize = CCID_DESCRIPTOR_SIZE;
    self->base.InterfaceExtendDescriptor     = (uint8_t*)(&self->ClassDescriptor);

    return true;
}
