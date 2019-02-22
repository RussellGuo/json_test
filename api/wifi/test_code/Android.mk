
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS:= optional

LOCAL_MODULE:= wifi_test

LOCAL_SRC_FILES:= \
    main.cpp       


#LOCAL_C_INCLUDES:= \


#LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_SHARED_LIBRARIES:= libwifiapi

include $(BUILD_EXECUTABLE)
