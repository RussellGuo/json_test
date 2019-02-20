LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	adf_class.cpp simple_draw.cpp gdi.c

LOCAL_C_INCLUDES +=\
    system/core/adf/libadf/include
    
LOCAL_WHOLE_STATIC_LIBRARIES += libadf

LOCAL_MODULE := libgdi
LOCAL_CPPFLAGS += -std=c++11

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	main.c

LOCAL_SHARED_LIBRARIES := libgdi
LOCAL_MODULE := adf_test
LOCAL_CFLAGS += -std=c99

include $(BUILD_EXECUTABLE)

