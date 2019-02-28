LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := external/tinyalsa/include
LOCAL_SRC_FILES := suspend_api.c
LOCAL_SHARED_LIBRARIES:= liblog libcutils
LOCAL_MODULE := libsuspend_api
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_SRC_FILES := suspend_api_test.c
LOCAL_SHARED_LIBRARIES := libsuspend_api
LOCAL_MODULE := suspend_api_test
include $(BUILD_EXECUTABLE)

