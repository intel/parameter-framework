LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_PATH :=


LOCAL_SRC_FILES:= \
        main.cpp


LOCAL_MODULE:= remote-process

LOCAL_MODULE_TAGS := optional

TARGET_ERROR_FLAGS += -Wno-non-virtual-dtor

LOCAL_C_INCLUDES += \
        external/stlport/stlport/ \
        bionic/libstdc++ \
        bionic/

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../remote-processor/

LOCAL_SHARED_LIBRARIES := libstlport libicuuc libremote-processor
LOCAL_STATIC_LIBRARIES :=

LOCAL_LDLIBS +=
LOCAL_LDFLAGS +=

include $(BUILD_EXECUTABLE)

