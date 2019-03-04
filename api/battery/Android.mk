LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES:=$(LOCAL_PATH)
LOCAL_MODULE:=libbattery_get_status
LOCAL_SRC_FILES := \
	bat_get_stat_api.cpp
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_C_INCLUDES:=$(LOCAL_PATH)
LOCAL_SRC_FILES:=main.c
LOCAL_SHARED_LIBRARIES:=libbattery_get_status
LOCAL_MODULE:=battery_test
include $(BUILD_EXECUTABLE)

