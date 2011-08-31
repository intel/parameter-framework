LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

LOCAL_SRC_FILES:= \
        Socket.cpp \
        ListeningSocket.cpp \
        ConnectionSocket.cpp \
        Message.cpp \
        RequestMessage.cpp \
        AnswerMessage.cpp \
        RemoteProcessorServer.cpp \
        RemoteProcessorServerBuilder.cpp

LOCAL_MODULE:= libremote-processor

LOCAL_MODULE_TAGS := optional

TARGET_ERROR_FLAGS += -Wno-non-virtual-dtor

LOCAL_C_INCLUDES +=

LOCAL_C_INCLUDES += \
        external/stlport/stlport/ \
        bionic/libstdc++ \
        bionic/

LOCAL_C_INCLUDES +=

LOCAL_SHARED_LIBRARIES := libstlport libicuuc
LOCAL_STATIC_LIBRARIES :=

LOCAL_LDLIBS +=


include $(BUILD_SHARED_LIBRARY)

