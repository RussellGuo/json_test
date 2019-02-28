LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES := libcutils liblog
LOCAL_INCLUDES += $(LOCAL_PATH)
LOCAL_SRC_FILES += set_led_api.c
LOCAL_MODULE:= libsetled
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES := libsetled
LOCAL_INCLUDES += $(LOCAL_PATH)
LOCAL_SRC_FILES += set_led_test.c
LOCAL_MODULE:= set_led_test
include $(BUILD_EXECUTABLE)
