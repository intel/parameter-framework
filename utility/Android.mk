LOCAL_PATH := $(call my-dir)

####################
# Common definitions

common_src_files := \
    Tokenizer.cpp \
    Utility.cpp

common_module := libutility
common_module_tags := optional

common_cflags := \
        -Wall \
        -Werror \
        -Wextra \
        -Wno-unused-parameter

#############################
# Target build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(common_src_files)

LOCAL_MODULE := $(common_module)
LOCAL_MODULE_TAGS := $(common_module_tags)

LOCAL_CFLAGS := $(common_cflags)

LOCAL_C_INCLUDES += \
    $(call include-path-for, stlport) \
    bionic

include $(BUILD_STATIC_LIBRARY)

##############################
# Host build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(common_src_files)

LOCAL_MODULE := $(common_module)_host
LOCAL_MODULE_TAGS := $(common_module_tags)

LOCAL_CFLAGS := $(common_cflags)

include $(BUILD_HOST_STATIC_LIBRARY)
