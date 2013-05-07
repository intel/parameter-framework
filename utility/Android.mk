LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_SRC_FILES := \
    Tokenizer.cpp \
    Utility.cpp

LOCAL_MODULE := libutility

LOCAL_MODULE_TAGS :=


LOCAL_C_INCLUDES +=


LOCAL_C_INCLUDES += \
	external/stlport/stlport/ \
	bionic/libstdc++ \
	bionic/

LOCAL_C_INCLUDES +=

LOCAL_SHARED_LIBRARIES :=

LOCAL_LDLIBS +=


include $(BUILD_STATIC_LIBRARY)

