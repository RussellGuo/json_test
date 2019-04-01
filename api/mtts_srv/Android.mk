LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
 
LOCAL_MODULE := libtts_ms
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_SUFFIX := .a
LOCAL_SRC_FILES := libmtts.a
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)
include $(BUILD_PREBUILT)


include $(CLEAR_VARS)
 
LOCAL_MODULE := libsnd_file
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES := libsndfile.so
include $(BUILD_PREBUILT)


include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := libtinyalsa libsnd_file 
LOCAL_CFLAGS += -std=c99
LOCAL_CPPFLAGS += -std=c++11 -fexceptions
LOCAL_WHOLE_STATIC_LIBRARIES += libtts_ms
LOCAL_C_INCLUDES += external/tinyalsa/include
LOCAL_CPP_INCLUDES += external/tinyalsa/include
LOCAL_SRC_FILES := demo.cpp pcm_play.c

LOCAL_NDK_STL_VARIANT := stlport_static

LOCAL_SDK_VERSION := 9

LOCAL_MODULE := tts_ms_service

include $(BUILD_EXECUTABLE)

