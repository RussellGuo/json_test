LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := tts_api.c

LOCAL_CFLAGS += -std=c99
LOCAL_SHARED_LIBRARIES += libipc_cmd libpoll_event_api
LOCAL_MODULE := libtts
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := main.c

LOCAL_SHARED_LIBRARIES := libtts libipc_cmd libpoll_event_api libkey_api
LOCAL_MODULE := tts_test
LOCAL_CFLAGS += -std=c99


include $(BUILD_EXECUTABLE)

