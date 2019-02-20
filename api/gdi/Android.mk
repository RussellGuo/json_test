LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	adf_class.cpp

LOCAL_C_INCLUDES +=\
    system/core/adf/libadf/include
    
LOCAL_WHOLE_STATIC_LIBRARIES += libadf

LOCAL_MODULE := adf_test
LOCAL_CPPFLAGS += -std=c++11

include $(BUILD_EXECUTABLE)
