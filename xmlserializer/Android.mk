LOCAL_PATH := $(call my-dir)

####################
# Common definitions

COMMON_SRC_FILES := \
        XmlElement.cpp \
        XmlSerializingContext.cpp \
        XmlDocSource.cpp \
        XmlDocSink.cpp \
        XmlMemoryDocSink.cpp \
        XmlMemoryDocSource.cpp \
        XmlStringDocSink.cpp \
        XmlFileDocSink.cpp \
        XmlFileDocSource.cpp

COMMON_MODULE := libxmlserializer

COMMON_MODULE_TAGS := optional

COMMON_CFLAGS := -Wno-non-virtual-dtor

COMMON_C_INCLUDES := \
    external/libxml2/include/ \
    external/webkit/Source/WebCore/icu/ \

COMMON_SHARED_LIBRARIES := libicuuc
COMMON_STATIC_LIBRARIES := libxml2

COMMON_LDLIBS := -Lexternal/libxml2/lib

#############################
# Target build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(COMMON_SRC_FILES)

LOCAL_MODULE := $(COMMON_MODULE)
LOCAL_MODULE_TAGS := $(COMMON_MODULE_TAGS)

LOCAL_CFLAGS += $(COMMON_CFLAGS)

LOCAL_C_INCLUDES += \
    $(COMMON_C_INCLUDES) \
    external/stlport/stlport/ \
    bionic/

LOCAL_SHARED_LIBRARIES := $(COMMON_SHARED_LIBRARIES) libstlport
LOCAL_STATIC_LIBRARIES := $(COMMON_STATIC_LIBRARIES)

LOCAL_LDLIBS += $(COMMON_LDLIBS)

include $(BUILD_STATIC_LIBRARY)

##############################
# Host build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(COMMON_SRC_FILES)

LOCAL_MODULE := $(COMMON_MODULE)
LOCAL_MODULE_TAGS := $(COMMON_MODULE_TAGS)

LOCAL_CFLAGS += $(COMMON_CFLAGS)

LOCAL_C_INCLUDES += \
    $(COMMON_C_INCLUDES)

LOCAL_SHARED_LIBRARIES := $(COMMON_SHARED_LIBRARIES)
LOCAL_STATIC_LIBRARIES := $(COMMON_STATIC_LIBRARIES)

LOCAL_LDLIBS += $(COMMON_LDLIBS)

include $(BUILD_HOST_STATIC_LIBRARY)
