# Copyright (C) 2016 Spreadtrum Communications Inc.
#

LOCAL_PATH:= $(call my-dir)

###################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := modemApiTest
LOCAL_SRC_FILES := \
    modemApiTest.c

LOCAL_C_INCLUDES:= \
    vendor/huaqin/packages/api/4G/lib
    
LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libmodemapi \
    libutils

LOCAL_CFLAGS := -DAPP_PPPDIAL_MODULE

include $(BUILD_EXECUTABLE)
###################################################################





