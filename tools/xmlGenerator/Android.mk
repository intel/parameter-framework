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


LOCAL_PATH := $(call my-dir)

##################################################
# Scripts are not compiled so the prebuild mechanism is used to export them.

include $(CLEAR_VARS)
LOCAL_MODULE := EddParser.py
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := PfwBaseTranslator.py
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := PFWScriptGenerator.py
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true
LOCAL_REQUIRED_MODULES := \
    PfwBaseTranslator.py \
    EddParser.py
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := hostConfig.py
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := domainGenerator.py
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true
LOCAL_REQUIRED_MODULES := \
    _PyPfw_32 \
    EddParser.py \
    PfwBaseTranslator.py \
    hostConfig.py
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := portAllocator.py
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := hostDomainGenerator.sh
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_REQUIRED_MODULES :=  \
    PFWScriptGenerator.py \
    hostConfig.py \
    portAllocator.py \
    test-platform_host \
    remote-process_host \
    ParameterFrameworkConfiguration.xsd \
    ConfigurableDomains.xsd \
    SystemClass.xsd \
    ParameterSettings.xsd \
    FileIncluder.xsd \
    Subsystem.xsd \
    ComponentLibrary.xsd \
    ComponentTypeSet.xsd \
    W3cXmlAttributes.xsd \
    Parameter.xsd
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := lightRoutingUpdate.sh
LOCAL_MODULE_OWNER := intel
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_REQUIRED_MODULES :=  \
    PFWScriptGenerator.py
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true
include $(BUILD_PREBUILT)
##################################################
