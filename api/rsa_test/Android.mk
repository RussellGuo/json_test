LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += external/openssl/include
LOCAL_CFLAGS += -std=c99
LOCAL_SRC_FILES := rsa_demo.c
LOCAL_MODULE := rsa_demo

LOCAL_STATIC_LIBRARIES := \
        libc \
    libcrypto_static


LOCAL_FORCE_STATIC_EXECUTABLE := true

include $(BUILD_EXECUTABLE)

