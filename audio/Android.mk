LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += \
	external/tinyalsa/include \
	hardware/libhardware/include \
	$(call include-path-for, audio-effects) \
	$(call include-path-for, audio-utils) \
	frameworks/base/include

LOCAL_LDLIBS := -lpthread 
LOCAL_SHARED_LIBRARIES := libcutils libutils libmedia libmediaplayerservice libbinder libstagefright_foundation libdl libaudioutils
LOCAL_STATIC_LIBRARIES := libmedia_helper

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := playertest

LOCAL_SRC_FILES := aud_player.cpp player.c

include $(BUILD_EXECUTABLE)


