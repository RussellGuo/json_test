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

#if CONFIG_USB_DEVICE_CCID
#define CCID_MAX_COMMAND_BUFFER  (271)
#define CCID_MAX_RESPONSE_BUFFER (271)
#endif // CONFIG_USB_DEVICE_CCID


// The following configs of endpoints that has been commented out are DEPRECATED!
// These configs can be overrode after InterfaceConstractor(interface) and before interface->Init()
// It is more recommanded to set these configs dynamically.
/* DEPRECATED CONFIGS of ENDPOINTS




#if CONFIG_USB_DEVICE_CCID
#define CCID_RX_EP           2
#define CCID_TX_EP           2
#define CCID_MAX_PACKET      64
#define CCID_MAX_BYTE_BUFFER 271
#endif // CONFIG_USB_DEVICE_CCID

*/

/* USB Interfaces Default Config End                                          */

#endif
