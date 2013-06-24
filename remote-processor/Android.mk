LOCAL_PATH := $(call my-dir)

####################
# Common definitions

common_src_files := \
        Socket.cpp \
        ListeningSocket.cpp \
        ConnectionSocket.cpp \
        Message.cpp \
        RequestMessage.cpp \
        AnswerMessage.cpp \
        RemoteProcessorServer.cpp \
        RemoteProcessorServerBuilder.cpp

common_module := libremote-processor
common_module_tags := optional

common_ldlibs := -lpthread
#############################
# Target build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(common_src_files)

LOCAL_MODULE := $(common_module)
LOCAL_MODULE_TAGS := $(common_module_tags)

LOCAL_CFLAGS += $(common_cflags)

LOCAL_C_INCLUDES += \
    external/stlport/stlport/ \
    bionic/

LOCAL_SHARED_LIBRARIES := libstlport

LOCAL_LDLIBS += $(common_ldlibs)

include $(BUILD_SHARED_LIBRARY)

##############################
# Host build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(common_src_files)

LOCAL_MODULE := $(common_module)_host
LOCAL_MODULE_TAGS := $(common_module_tags)

LOCAL_CFLAGS += $(common_cflags)

LOCAL_LDLIBS += $(common_ldlibs)

include $(BUILD_HOST_SHARED_LIBRARY)
