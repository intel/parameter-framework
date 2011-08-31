LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

LOCAL_SRC_FILES:= \
        XmlParser.cpp \
        XmlElement.cpp \
        XmlComposer.cpp \
        XmlSerializingContext.cpp \
        XmlSerializer.cpp

LOCAL_MODULE:= libxmlserializer

LOCAL_MODULE_TAGS := optional

TARGET_ERROR_FLAGS += -Wno-non-virtual-dtor

LOCAL_C_INCLUDES +=

LOCAL_C_INCLUDES += \
	external/stlport/stlport/ \
	external/libxml2/include/ \
	external/webkit/WebCore/icu/ \
	bionic/libstdc++ \
	bionic/

LOCAL_C_INCLUDES += 

LOCAL_SHARED_LIBRARIES := libstlport libicuuc
LOCAL_STATIC_LIBRARIES := libxml2

LOCAL_LDLIBS += -Lexternal/libxml2/lib


include $(BUILD_SHARED_LIBRARY)

