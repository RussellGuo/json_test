LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
        adf_class.cpp simple_draw.cpp gdi.c lcd_brightness_timeout.cpp

LOCAL_C_INCLUDES +=\
    system/core/adf/libadf/include
    
LOCAL_SHARED_LIBRARIES := libpoll_event_api libkey_api
LOCAL_WHOLE_STATIC_LIBRARIES += libadf

LOCAL_MODULE := libgdi
LOCAL_CPPFLAGS += -std=c++11 -DFONT_PATH=\"/system/vendor/huaqin/fonts/\"

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	main.c

LOCAL_SHARED_LIBRARIES := libgdi libpoll_event_api libkey_api
LOCAL_MODULE := adf_test
LOCAL_CFLAGS += -std=c99

include $(BUILD_EXECUTABLE)

