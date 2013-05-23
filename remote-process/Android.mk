LOCAL_PATH := $(call my-dir)

####################
# Common definitions

COMMON_SRC_FILES := \
        main.cpp

COMMON_MODULE := remote-process
COMMON_MODULE_TAGS := optional

COMMON_ERROR_FLAGS := -Wno-non-virtual-dtor

COMMON_C_INCLUDES := \
    $(LOCAL_PATH)/../remote-processor/

COMMON_SHARED_LIBRARIES := libremote-processor

#############################
# Target build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(COMMON_SRC_FILES)

LOCAL_MODULE := $(COMMON_MODULE)
LOCAL_MODULE_TAGS := $(COMMON_MODULE_TAGS)

TARGET_ERROR_FLAGS += $(COMMON_ERROR_FLAGS)

LOCAL_C_INCLUDES += \
    $(COMMON_C_INCLUDES) \
    external/stlport/stlport/ \
    bionic/

LOCAL_SHARED_LIBRARIES := $(COMMON_SHARED_LIBRARIES) libstlport


include $(BUILD_EXECUTABLE)

##############################
# Host build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(COMMON_SRC_FILES)

LOCAL_MODULE := $(COMMON_MODULE)_host
LOCAL_MODULE_TAGS := $(COMMON_MODULE_TAGS)

TARGET_ERROR_FLAGS += $(COMMON_ERROR_FLAGS)

LOCAL_C_INCLUDES += \
    $(COMMON_C_INCLUDES)

LOCAL_SHARED_LIBRARIES := $(COMMON_SHARED_LIBRARIES)

include $(BUILD_HOST_EXECUTABLE)
