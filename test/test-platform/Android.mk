# Copyright (c) 2011-2014, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


LOCAL_PATH:= $(call my-dir)

####################
# Common definitions

common_src_files := \
    main.cpp \
    TestPlatform.cpp

common_module := test-platform
common_module_tags := optional

common_c_includes := \
    $(LOCAL_PATH)/../../parameter/include \
    $(LOCAL_PATH)/../../remote-processor/

common_shared_libraries := libparameter libremote-processor
#############################
# Target build

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= $(common_src_files)

LOCAL_MODULE:= $(common_module)
LOCAL_MODULE_OWNER := intel
LOCAL_MODULE_TAGS := $(common_module_tags)

LOCAL_C_INCLUDES := $(common_c_includes)

LOCAL_STATIC_LIBRARIES := libpfw_utility
LOCAL_SHARED_LIBRARIES := $(common_shared_libraries)

include $(BUILD_EXECUTABLE)

##############################
# Host build

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= $(common_src_files)

LOCAL_MODULE:= $(common_module)_host
LOCAL_MODULE_OWNER := intel
LOCAL_MODULE_TAGS := $(common_module_tags)

LOCAL_C_INCLUDES += $(common_c_includes)
LOCAL_CFLAGS := -pthread
LOCAL_LDLIBS := -lpthread

LOCAL_STATIC_LIBRARIES := libpfw_utility_host
LOCAL_SHARED_LIBRARIES := $(foreach shared_library, $(common_shared_libraries), \
    $(shared_library)_host)

include $(BUILD_HOST_EXECUTABLE)
