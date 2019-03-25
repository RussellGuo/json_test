LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

TARGET_BOARD_CAMERA_ISP_3AMOD:=1  # TBD only test
ISP_HW_VER = 2v1
ISPALG_DIR := ispalg/isp2.x
ISPDRV_DIR := camdrv/isp2.3
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../../$(ISPDRV_DIR)/isp_tune \
	$(LOCAL_PATH)/../../$(ISPALG_DIR)/common/inc \
	$(LOCAL_PATH)/../../$(ISPDRV_DIR)/middleware/inc \
	$(LOCAL_PATH)/../../$(ISPDRV_DIR)/driver/inc


HAL_DIR := hal3_2v1
LOCAL_C_INCLUDES += \
		   $(LOCAL_PATH)/../.. \
		   $(LOCAL_PATH)/../../common/inc \
		   $(LOCAL_PATH)/../../oem2v1/inc \
		   $(LOCAL_PATH)/../../$(HAL_DIR) \
		   $(TARGET_OUT_INTERMEDIATES)/KERNEL/usr/include/video \
		   $(TOP)/vendor/sprd/external/kernel-headers \
		   $(TOP)/vendor/sprd/modules/libmemion \

LOCAL_SHARED_LIBRARIES := libcutils liblog libcamoem libcamcommon libmemion 

LOCAL_LDFLAGS += -ldl

include $(LOCAL_PATH)/../../../../sprd/modules/libcamera/SprdCtrl.mk

LOCAL_SRC_FILES := bartest.cpp 
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := lib_bartest
LOCAL_MODULE_TAGS := debug
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
TARGET_BOARD_CAMERA_ISP_3AMOD:=1  # TBD only test
ISP_HW_VER = 2v1
ISPALG_DIR := ispalg/isp2.x
ISPDRV_DIR := camdrv/isp2.3
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../../$(ISPDRV_DIR)/isp_tune \
	$(LOCAL_PATH)/../../$(ISPALG_DIR)/common/inc \
	$(LOCAL_PATH)/../../$(ISPDRV_DIR)/middleware/inc \
	$(LOCAL_PATH)/../../$(ISPDRV_DIR)/driver/inc


HAL_DIR := hal3_2v1
LOCAL_C_INCLUDES += \
		   $(LOCAL_PATH)/../.. \
		   $(LOCAL_PATH)/../../common/inc \
		   $(LOCAL_PATH)/../../oem2v1/inc \
		   $(LOCAL_PATH)/../../$(HAL_DIR) \
		   $(TARGET_OUT_INTERMEDIATES)/KERNEL/usr/include/video \
		   $(TOP)/vendor/sprd/external/kernel-headers \
		   $(TOP)/vendor/sprd/modules/libmemion \

LOCAL_SHARED_LIBRARIES := libcutils liblog libcamoem libcamcommon libmemion 

LOCAL_LDFLAGS += -ldl
include $(LOCAL_PATH)/../../../../sprd/modules/libcamera/SprdCtrl.mk
LOCAL_SRC_FILES := minicam_api.cpp
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := minicam_api
LOCAL_MODULE_TAGS := debug
include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_EXECUTABLE)

#include $(CLEAR_VARS)

#LOCAL_SRC_FILES := \
#	main.cpp \
#       bartest.cpp

#LOCAL_SHARED_LIBRARIES := libcutils liblog libcamoem libcamcommon libmemion libHDSD
#LOCAL_MODULE := bartest
#include $(BUILD_EXECUTABLE)

