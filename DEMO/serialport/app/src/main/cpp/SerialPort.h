/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_huaqin_serialport_SerialPort */

#ifndef _Included_com_huaqin_posservices_SerialPort
#define _Included_com_huaqin_posservices_SerialPort
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_huaqin_serialport_SerialPort
 * Method:    initUart
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_huaqin_posservices_SerialPort_initUart
        (JNIEnv *, jobject);

/*
 * Class:     com_huaqin_serialport_SerialPort
 * Method:    serialDatagramSend
 * Signature: ([B)Z
 */
JNIEXPORT jboolean JNICALL Java_com_huaqin_posservices_SerialPort_serialDatagramSend
        (JNIEnv *, jclass, jbyteArray);

/*
 * Class:     com_huaqin_serialport_SerialPort
 * Method:    serialDatagramReceiveLoop
 * Signature: ()Z
 */
JNIEXPORT void JNICALL Java_com_huaqin_posservices_SerialPort_serialDatagramReceiveLoop
(JNIEnv *, jobject);

void receive_loop_result(const void *data_ptr, unsigned short len);

#ifdef __cplusplus
}
#endif
#endif
