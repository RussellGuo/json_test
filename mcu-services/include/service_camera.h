/*
 * service_camera.h
 *
 * Implementation of 'setup camera mode'
 * This service does not need a separate thread, because the response operation is very fast
 * and will not block the receiving thread of the protocol.
 * Refer to the documentation "SaIP communication protocol between the host and the MCU"
 * for the configuration method.
 *
 *  Created on: Mar 08, 2021
 *      Author: Guo Qiang
 */

#ifndef __SERVICE_CAMERA_H__
#define __SERVICE_CAMERA_H__

#ifdef __cplusplus
extern "C" {
#endif


// The hardware initialization function required for the operation of the camera module.
void camera_hw_init(void);

#ifdef __cplusplus
}
#endif

#endif // __SERVICE_CAMERA_H__
