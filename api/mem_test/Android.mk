LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=c99
LOCAL_SRC_FILES := mem_test.c
LOCAL_MODULE := mem_test


include $(BUILD_EXECUTABLE)

