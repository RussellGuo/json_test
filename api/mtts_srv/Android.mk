LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libtinysndfile

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES := lib_mtts_src/lib/libtinysndfile.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/lib_mtts_src/include
LOCAL_EXPORT_CPP_INCLUDE_DIRS := $(LOCAL_PATH)/lib_mtts_src/include


include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE := libsndfile

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES := lib_mtts_src/lib/libsndfile.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/lib_mtts_src/include
LOCAL_EXPORT_CPP_INCLUDE_DIRS := $(LOCAL_PATH)/lib_mtts_src/include


include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE := libmtts_c

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES := lib_mtts_src/libmtts_c.so
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/lib_mtts_src/include
LOCAL_EXPORT_CPP_INCLUDE_DIRS := $(LOCAL_PATH)/lib_mtts_src/include


include $(BUILD_PREBUILT)


include $(CLEAR_VARS)

LOCAL_MODULE := libpcm_48k

LOCAL_SHARED_LIBRARIES += libtinyalsa
LOCAL_CFLAGS += -std=c99
LOCAL_SRC_FILES := pcm_play.c pcm_play_warp.c
LOCAL_C_INCLUDES += external/tinyalsa/include

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := libtinyalsa libsndfile libmtts_c libipc_cmd libpoll_event_api
LOCAL_CFLAGS += -std=c99
LOCAL_SRC_FILES := mtts_main.c pcm_play.c
LOCAL_MODULE := mtts_service
LOCAL_C_INCLUDES += external/tinyalsa/include

include $(BUILD_EXECUTABLE)


