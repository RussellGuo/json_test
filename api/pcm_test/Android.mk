LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := libtinyalsa
LOCAL_CFLAGS += -std=c99
LOCAL_WHOLE_STATIC_LIBRARIES += 
LOCAL_C_INCLUDES += external/tinyalsa/include
LOCAL_SRC_FILES := pcm_test.c

LOCAL_MODULE := pcm_test
LOCAL_CFLAGS += -std=c99

include $(BUILD_EXECUTABLE)

