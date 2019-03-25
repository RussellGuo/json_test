LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

include $(LOCAL_PATH)/bar_test.mk

include $(call first-makefiles-under,$(LOCAL_PATH))
