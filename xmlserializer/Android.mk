LOCAL_PATH := $(call my-dir)

####################
# Common definitions

common_src_files := \
        XmlElement.cpp \
        XmlSerializingContext.cpp \
        XmlDocSource.cpp \
        XmlDocSink.cpp \
        XmlMemoryDocSink.cpp \
        XmlMemoryDocSource.cpp \
        XmlStringDocSink.cpp \
        XmlFileDocSink.cpp \
        XmlFileDocSource.cpp \
        XmlStringDocSource.cpp

common_module := libxmlserializer

common_module_tags := optional

common_c_includes := \
    $(call include-path-for, libxml2) \
    $(call include-path-for, webcore-icu) \

common_shared_libraries := libicuuc
common_static_libraries := libxml2

common_ldlibs := -Lexternal/libxml2/lib

#############################
# Target build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(common_src_files)

LOCAL_MODULE := $(common_module)
LOCAL_MODULE_TAGS := $(common_module_tags)

LOCAL_C_INCLUDES += \
    $(common_c_includes) \
    $(call include-path-for, stlport) \
    bionic/

LOCAL_SHARED_LIBRARIES := $(common_shared_libraries) libstlport
LOCAL_STATIC_LIBRARIES := $(common_static_libraries)

LOCAL_LDLIBS += $(common_ldlibs)

include $(BUILD_STATIC_LIBRARY)

##############################
# Host build

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(common_src_files)

LOCAL_MODULE := $(common_module)_host
LOCAL_MODULE_TAGS := $(common_module_tags)

LOCAL_C_INCLUDES += \
    $(common_c_includes)

LOCAL_SHARED_LIBRARIES := $(common_shared_libraries)
LOCAL_STATIC_LIBRARIES := $(common_static_libraries)

LOCAL_LDLIBS += $(common_ldlibs)

include $(BUILD_HOST_STATIC_LIBRARY)
