LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=c99

LOCAL_SRC_FILES := ../common/calc_dir_digest_recursively.c gen_sys_part_sign.c host_fs_config.c
LOCAL_C_INCLUDES += external/openssl/include $(LOCAL_PATH)/../common

LOCAL_SHARED_LIBRARIES := libcrypto-host
LOCAL_MODULE := host_gen_sys_part_sign

include $(BUILD_HOST_EXECUTABLE)
