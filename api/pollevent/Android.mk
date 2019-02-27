LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CPPFLAGS += -std=c++11
LOCAL_SRC_FILES := poll_event_api.cpp timer_api.cpp poll_event_api_thread_notify.cpp
LOCAL_MODULE := libpoll_event_api
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_EXPORT_C_INCLUDE_DIRS += $(LOCAL_PATH)/include
LOCAL_EXPORT_CPP_INCLUDE_DIRS += $(LOCAL_PATH)/include

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := poll_event_demo_main.cpp thread_notify_demo.cpp

LOCAL_SHARED_LIBRARIES := libpoll_event_api
LOCAL_MODULE := poll_event_api_test
LOCAL_CPPFLAGS += -std=c++11

include $(BUILD_EXECUTABLE)

