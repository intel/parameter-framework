LOCAL_PATH := $(call my-dir)

####################
# Common definitions

COMMON_SRC_FILES := \
        Socket.cpp \
        ListeningSocket.cpp \
        ConnectionSocket.cpp \
        Message.cpp \
        RequestMessage.cpp \
        AnswerMessage.cpp \
        RemoteProcessorServer.cpp \
        RemoteProcessorServerBuilder.cpp

COMMON_MODULE := libremote-processor
COMMON_MODULE_TAGS := optional

COMMON_ERROR_FLAGS := -Wno-non-virtual-dtor

COMMON_LDLIBS := -lpthread

#############################
# Target build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(COMMON_SRC_FILES)

LOCAL_MODULE := $(COMMON_MODULE)
LOCAL_MODULE_TAGS := $(COMMON_MODULE_TAGS)

TARGET_ERROR_FLAGS += $(COMMON_ERROR_FLAGS)

LOCAL_C_INCLUDES += \
    external/stlport/stlport/ \
    bionic/libstdc++ \
    bionic/

LOCAL_SHARED_LIBRARIES := libstlport

LOCAL_LDLIBS += $(COMMON_LDLIBS)

include $(BUILD_SHARED_LIBRARY)

##############################
# Host build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(COMMON_SRC_FILES)

LOCAL_MODULE := $(COMMON_MODULE)
LOCAL_MODULE_TAGS := $(COMMON_MODULE_TAGS)

TARGET_ERROR_FLAGS += $(COMMON_ERROR_FLAGS)

LOCAL_LDLIBS += $(COMMON_LDLIBS)

include $(BUILD_HOST_SHARED_LIBRARY)
