
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS:= optional

LOCAL_MODULE:= libwifiapi

ifeq ($(SHARKLJ1_BBAT_BIT64), true)
LOCAL_32_BIT_ONLY := false
LOCAL_CFLAGS += -DSHARKLJ1_BBAT_BIT64
else
LOCAL_32_BIT_ONLY := true
LOCAL_CFLAGS += -DSHARKLJ1_BBAT_BIT32
endif

LOCAL_SRC_FILES:= \
    wifi.cpp             \
    softap.cpp       \
    debug.cpp           


LOCAL_C_INCLUDES:= \
    external/bluetooth/bluez/lib \
    external/bluetooth/bluez/src \
    frameworks/base/include \
    frameworks/av/include \
    system/bluetooth/bluedroid/include \
    system/core/include \
    hardware/libhardware/include \
    hardware/libhardware_legacy/include \
    external/bluetooth/bluedroid/btif/include \
    external/bluetooth/bluedroid/gki/ulinux \
    external/bluetooth/bluedroid/stack/include \
    external/bluetooth/bluedroid/stack/btm \
    $(TARGET_OUT_INTERMEDIATES)/KERNEL/usr/include/video \


LOCAL_SHARED_LIBRARIES:= \
    libbinder \
    libatci \
    liblog \
    libcamera_client \
    libcutils    \
    libdl        \
    libgui       \
    libhardware  \
    libhardware_legacy \
    libmedia \
    libui    \
    libnetutils \
    libutils \
    libpixelflinger  \


LOCAL_STATIC_LIBRARIES:= \
    libatminui \
    libpng \
    libcutils \
    libz \
    liblog \
    libm \
    libmtdutils
include $(BUILD_SHARED_LIBRARY)
include $(call all-makefiles-under,$(LOCAL_PATH))
