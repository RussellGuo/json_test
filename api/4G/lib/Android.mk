# Copyright (C) 2016 Spreadtrum Communications Inc.
#

LOCAL_PATH:= $(call my-dir)

###################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := libmodemapi
LOCAL_SRC_FILES := \
    modemApi.cpp \
	utils/record_stream.c  \
    app_pppDial.cpp \
    app_at.cpp

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libbinder \
    libutils

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_CFLAGS := -DAPP_PPPDIAL_MODULE
ifeq ($(PLATFORM_VERSION),$(filter $(PLATFORM_VERSION),4.4 4.4.1 4.4.2 4.4.3i 4.4.4))
include external/stlport/libstlport.mk
endif

include $(BUILD_SHARED_LIBRARY)

