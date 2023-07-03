#include "USBBSP.h"

static wchar_t USBUniqueSN[16] = {0};

void USBGenerateUniqueSN(wchar_t* uniqueSN, uint32_t maxLength)
{
    // Dynamic USB Device SN format:
    //   [interface functions]-[ChipSN sum]
    uint8_t usbSN[32] = {0};

    maxLength = MIN((maxLength - 1), sizeof(usbSN));

    uint8_t snIndex = 0;

#if CONFIG_USB_DEVICE_HID
    usbSN[snIndex++] = 'D';
#endif // CONFIG_USB_DEVICE_HID

    usbSN[snIndex++] = '-';

#if CONFIG_MHSCPU
    uint8_t chipSN[SYSCTRL_CHIP_SN_LEN];
    SYSCTRL_GetChipSN(chipSN);
#endif // CONFIG_MHSCPU

    uint8_t usnLength = MIN((maxLength - snIndex), 8);
    uint8_t shift     = 0;
    for (int i = 0; i < sizeof(chipSN) * 2; i++)
    {
        int usnIndex = i % usnLength + snIndex;
        shift += usbSN[usnIndex] + chipSN[i % sizeof(chipSN)];
        usbSN[usnIndex] = shift % 36;
    }

    for (int i = 0; i < snIndex; i++)
    {
        uniqueSN[i] = usbSN[i];
    }
    for (int i = snIndex; i < snIndex + usnLength; i++)
    {
        uniqueSN[i] = usbSN[i] + (usbSN[i] < 0x0A ? '0' : '7');
    }
}

#if CONFIG_MHSCPU

USBHALStruct    USBHAL;
USBDeviceStruct USBDevice;

void USBInterruptSetup(void)
{
    // Set the interrupt priority of USB Core
    NVIC_SetPriority(USB_ZOFFY_IRQ, 0);
    // Will enable when device connect
    NVIC_DisableIRQ(USB_ZOFFY_IRQ);
}

void USBSetup(void)
{
    USBInterruptSetup();

    // USBHAL Init
    USBHALStruct* hal = &USBHAL;
    USBCoreZoffyConstractor(hal);

    hal->PhyType     = USBPhyTypeFS;
    hal->SpeedConfig = USBSpeedConfigFull;


    hal->IsDMAEnabled = false;
    hal->Init(hal, USB_ZOFFY_BASE);

    // USBDevice Init
    USBDeviceStruct* device = &USBDevice;

    USBDeviceConstractor(device);

    device->Manufacturer           = L"Megahunt";
    device->Product                = L"USB-ESL Composite Device";
    device->ConfigurationName      = L"Default";
    device->MaxPower               = 100; // 100mA, Max 500mA
    device->Attributes.SelfPowered = true;

    // Generate unique SN for USB
    USBGenerateUniqueSN(USBUniqueSN, sizeof(USBUniqueSN) / sizeof(wchar_t));
    device->SerialNumber = USBUniqueSN;

    // Configure USBDevice's VendorID, ProductID and ProductRelease
    device->Init(device, hal, 0x0D28, 0xCCDD, 0x0001);



#if CONFIG_USB_DEVICE_HID
    HIDSetup(device);
#endif // CONFIG_USB_DEVICE_HID



    // USBDevice Start
    device->Start(device);
}

void USBStop(void)
{
    USBDeviceStruct* device = &USBDevice;
    device->Stop(device);
    USBTargetEnablePhy(&USBHAL, false);
    USBTargetEnableInterrupt(&USBHAL, false);
}

void USB_IRQHandler(void)
{
    USBHAL.Interrupt(&USBHAL);
}
#endif // CONFIG_MHSCPU


void USBLoop(void)
{

#if CONFIG_USB_DEVICE_HID
    HIDLoop();
#endif // CONFIG_USB_DEVICE_HID
}
