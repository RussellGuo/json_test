LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := libserialport
LOCAL_SRC_FILES := SerialPort.cpp
LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)
