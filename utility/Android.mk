LOCAL_PATH := $(call my-dir)

####################
# Common definitions

COMMON_SRC_FILES := \
    Tokenizer.cpp \
    Utility.cpp

COMMON_MODULE := libutility
COMMON_MODULE_TAGS := optional

#############################
# Target build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(COMMON_SRC_FILES)

LOCAL_MODULE := $(COMMON_MODULE)
LOCAL_MODULE_TAGS := $(COMMON_MODULE_TAGS)

LOCAL_C_INCLUDES += \
    external/stlport/stlport/ \
    bionic/libstdc++ \
    bionic/

include $(BUILD_STATIC_LIBRARY)

##############################
# Host build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(COMMON_SRC_FILES)

LOCAL_MODULE := $(COMMON_MODULE)
LOCAL_MODULE_TAGS := $(COMMON_MODULE_TAGS)

include $(BUILD_HOST_STATIC_LIBRARY)
