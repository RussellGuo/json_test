LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
 
LOCAL_MODULE := libyt_tts
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := lib/libyt_tts_cn_man_arm_300.a
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
include $(BUILD_PREBUILT)


include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := libtinyalsa libipc_cmd libpoll_event_api
LOCAL_CFLAGS += -std=c99 -DTTS_VERSION=300
LOCAL_WHOLE_STATIC_LIBRARIES += libyt_tts
LOCAL_C_INCLUDES += external/tinyalsa/include
LOCAL_SRC_FILES := tts.c pcm_play.c

LOCAL_MODULE := tts_service
LOCAL_CFLAGS += -std=c99

include $(BUILD_EXECUTABLE)

