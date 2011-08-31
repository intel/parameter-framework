LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_PATH := 


LOCAL_SRC_FILES:= \
	main.cpp


LOCAL_MODULE:= parameter-test

LOCAL_MODULE_TAGS := optional

TARGET_ERROR_FLAGS += -Wno-non-virtual-dtor

LOCAL_C_INCLUDES += \
	external/stlport/stlport/ \
	external/libxml2/include/ \
	external/webkit/WebCore/icu/ \
	bionic/libstdc++ \
	bionic/

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../parameter $(LOCAL_PATH)/../xmlserializer/ $(LOCAL_PATH)/../remote-processor/

LOCAL_SHARED_LIBRARIES := libstlport libicuuc libparameter libxmlserializer
LOCAL_STATIC_LIBRARIES := libxml2

LOCAL_LDLIBS += -Lexternal/libxml2/lib
LOCAL_LDFLAGS +=

include $(BUILD_EXECUTABLE)

