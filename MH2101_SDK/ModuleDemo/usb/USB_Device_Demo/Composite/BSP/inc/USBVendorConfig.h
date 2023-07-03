#ifndef __USB_VENDOR_CONFIG_H__
#define __USB_VENDOR_CONFIG_H__

// define USB Target Chip
#if CONFIG_MHSCPU
#if CONFIG_MHSCPU_MH2101
#define CONFIG_USB_TARGET USB_TARGET_MH2101
#endif // CONFIG_MHSCPU_MH2101
#endif // CONFIG_MHSCPU

// set 1 to enalbe debug
#define CONFIG_DEBUG 0

/* USB Device Config Start                                                    */
// Change the default USB Version only for legacy device
// #define CONFIG_USB_VERSION USB_VERSION_1_1

// Max interface count
#define CONFIG_USB_DEVICE_MAX_INTERFACE_COUNT (4)

// USB Device Descriptor buffer size = max size of DEVICE_DESCRIPTOR_LENGTH, USB_DEVICE_MAX_STRING_DESCRIPTOR_SIZE,
// USB_DEVICE_MAX_CONFIGURATION_DESCRIPTOR_SIZE, USB_DEVICE_MAX_REPORT_DESCRIPTOR_SIZE and USB_DEVICE_MAX_VENDOR_DESCRIPTOR_SIZE
#define CONFIG_USB_DEVICE_MAX_STRING_DESCRIPTOR_SIZE        (128)
#define CONFIG_USB_DEVICE_MAX_CONFIGURATION_DESCRIPTOR_SIZE (256)
#define CONFIG_USB_DEVICE_MAX_REPORT_DESCRIPTOR_SIZE        (64)
#define CONFIG_USB_DEVICE_MAX_VENDOR_DESCRIPTOR_SIZE        (128)

/* USB Device Config End                                                      */

/* USB Interfaces Default Config Start                                        */
#if CONFIG_USB_DEVICE_VCP
// defined the interface->SendPush(...) queue length
#define SERIAL_TX_QUEUE_LENGTH 16

// defined the first level read buffer size of serial interface
#define SERIAL_MAX_RX_SIZE 1024

// defined to support send bytes buffer by application call the method of interface->SendPushByte(...)
#define SERIAL_SUPPORT_SEND_BYTES

// defined to support read bytes buffer by application call the method of interface->ReadPopByte(...)
#define SERIAL_SUPPORT_READ_BYTES

#if CONFIG_MHSCPU
#endif // CONFIG_MHSCPU
#endif // CONFIG_USB_DEVICE_VCP


#if CONFIG_USB_DEVICE_HID
// uncommon the definition to change the default HID Vendor Report Size (only for HID Vendor Demo)
// #define HID_VENDOR_REPORT_SIZE 512
#endif // CONFIG_USB_DEVICE_HID

// The following configs of endpoints that has been commented out are DEPRECATED!
// These configs can be overrode after InterfaceConstractor(interface) and before interface->Init()
// It is more recommanded to set these configs dynamically.
/* DEPRECATED CONFIGS of ENDPOINTS

#if CONFIG_USB_DEVICE_MSD
#define MSD_RX_EP           1
#define MSD_TX_EP           1
#define MSD_MAX_PACKET      64
#define MSD_MAX_BYTE_BUFFER 512
#endif // CONFIG_USB_DEVICE_MSD

#if CONFIG_USB_DEVICE_VCP
#define SERIAL_RX_EP       2
#define SERIAL_TX_EP       2
#define SERIAL_MAX_PACKET  64
#define SERIAL_MAX_RX_SIZE 1024
#define SERIAL_RX_TIMEOUT  3 // tick (1ms/tick)
#endif // CONFIG_USB_DEVICE_VCP

#if CONFIG_USB_DEVICE_HID
#define HID_RX_EP      3
#define HID_TX_EP      3
#define HID_MAX_PACKET 64
#endif // CONFIG_USB_DEVICE_HID


*/

/* USB Interfaces Default Config End                                          */

#endif
