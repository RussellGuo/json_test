LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=c99
ifeq ($(TARGET_BUILD_VARIANT),userdebug)
LOCAL_CFLAGS += -DCONTINUE_THE_SYSTEM_BOOTING=1
endif

LOCAL_CPPFLAGS += -std=c++11

LOCAL_SRC_FILES := ../common/calc_dir_digest_recursively.c check_sys_part_sign.c target_fs_config.c ../../api/gdi/adf_class.cpp
LOCAL_C_INCLUDES += external/openssl/include $(LOCAL_PATH)/../common system/core/adf/libadf/include $(LOCAL_PATH)/../../api/gdi

LOCAL_MODULE := target_chk_sys_part_sign
LOCAL_STATIC_LIBRARIES := \
        libc \
        libadf \
        libcutils \
    libcrypto_static


LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)

include $(BUILD_EXECUTABLE)
