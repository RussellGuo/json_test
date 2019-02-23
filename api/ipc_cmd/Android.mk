LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
 
LOCAL_CFLAGS += -std=c99
LOCAL_SRC_FILES := ipc_cmd.c

LOCAL_MODULE := libipc_cmd
LOCAL_SHARED_LIBRARIES := libpoll_event_api
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include

include $(BUILD_STATIC_LIBRARY)
