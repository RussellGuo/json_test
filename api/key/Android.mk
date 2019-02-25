LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CPPFLAGS += -std=c++11
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SRC_FILES := \
	key_api.cpp

LOCAL_SHARED_LIBRARIES:= libpoll_event_api
LOCAL_MODULE := libkey_api
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := main.c

LOCAL_SHARED_LIBRARIES := libkey_api libpoll_event_api libtimer_api
LOCAL_MODULE := key_test
LOCAL_CFLAGS += -std=c99

include $(BUILD_EXECUTABLE)

