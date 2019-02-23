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
LOCAL_C_INCLUDES := external/tinyalsa/include
LOCAL_CFLAGS += -std=c99 -DTTS_VERSION=300
LOCAL_SRC_FILES := \
	tts.c tts_api.c pcm_play.c

LOCAL_WHOLE_STATIC_LIBRARIES += libyt_tts
LOCAL_SHARED_LIBRARIES:= libtinyalsa libpoll_event_api 
LOCAL_STATIC_LIBRARIES:= libipc_cmd libtimer_api
LOCAL_MODULE := libtts
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	main.c

LOCAL_SHARED_LIBRARIES := libtinyalsa libpoll_event_api
LOCAL_STATIC_LIBRARIES := libtts libipc_cmd libtimer_api
LOCAL_MODULE := tts_test
LOCAL_CFLAGS += -std=c99


include $(BUILD_EXECUTABLE)

