LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_PATH := 


LOCAL_SRC_FILES:= \
	main.cpp \
	TestPlatform.cpp


LOCAL_MODULE:= test-platform

LOCAL_MODULE_TAGS := optional

TARGET_ERROR_FLAGS += -Wno-non-virtual-dtor

LOCAL_C_INCLUDES += \
	external/stlport/stlport/ \
	bionic/libstdc++ \
	bionic/

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../../parameter \
	$(LOCAL_PATH)/../../remote-processor/ 

LOCAL_SHARED_LIBRARIES :=  \
	libparameter \
	libremote-processor

LOCAL_STATIC_LIBRARIES := 

LOCAL_LDLIBS += 
LOCAL_LDFLAGS +=

include $(BUILD_EXECUTABLE)

