LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_PATH :=


LOCAL_SRC_FILES:= \
        main.cpp \
        ExamplePlatform.cpp


LOCAL_MODULE:= parameter-connector-test

LOCAL_MODULE_TAGS := optional

TARGET_ERROR_FLAGS += -Wno-non-virtual-dtor

LOCAL_C_INCLUDES += \
        external/stlport/stlport/ \
        external/webkit/WebCore/icu/ \
        bionic/libstdc++ \
        bionic/

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../parameter

LOCAL_SHARED_LIBRARIES := libstlport libicuuc libparameter
LOCAL_STATIC_LIBRARIES :=

LOCAL_LDLIBS +=
LOCAL_LDFLAGS +=

include $(BUILD_EXECUTABLE)

