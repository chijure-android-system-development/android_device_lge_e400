ifeq ($(TARGET_BOOTLOADER_BOARD_NAME),e400)

LOCAL_PATH := $(call my-dir)

# libcamera.so is the wrapper; the stock LG HAL is installed as
# libcamera.e400.so (see e400-vendor-blobs.mk). QualcommCameraHardware.cpp in
# this directory is the old source HAL and is not built.
include $(CLEAR_VARS)

LOCAL_MODULE := libcamera
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libcamera_client \
    libui \
    libbinder \
    libdl
LOCAL_SRC_FILES := E400CameraWrapper.cpp

include $(BUILD_SHARED_LIBRARY)

endif
