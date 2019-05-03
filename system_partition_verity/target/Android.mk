LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=c99

LOCAL_SRC_FILES := ../common/calc_dir_digest_recursively.c check_sys_part_sign.c target_fs_config.c
LOCAL_C_INCLUDES += external/openssl/include $(LOCAL_PATH)/../common

LOCAL_MODULE := target_chk_sys_part_sign
LOCAL_STATIC_LIBRARIES := \
        libc \
    libcrypto_static


LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)

include $(BUILD_EXECUTABLE)
