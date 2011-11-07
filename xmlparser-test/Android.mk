LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_PATH := 


LOCAL_SRC_FILES:= \
	main.cpp


LOCAL_MODULE:= xmlparser-test

LOCAL_MODULE_TAGS := optional

TARGET_ERROR_FLAGS += -Wno-non-virtual-dtor

LOCAL_C_INCLUDES += \
	external/stlport/stlport/ \
	external/libxml2/include/ \
	external/webkit/Source/WebCore/icu/ \
	bionic/libstdc++ \
	bionic/

LOCAL_C_INCLUDES += 

LOCAL_SHARED_LIBRARIES := libstlport libicuuc
LOCAL_STATIC_LIBRARIES := libxml2

LOCAL_LDLIBS +=

include $(BUILD_EXECUTABLE)

