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

####################
# Common definitions

common_copy_headers_to := parameter

common_copy_headers := \
        include/ParameterMgrLoggerForward.h \
        include/ParameterMgrPlatformConnector.h \
        include/ParameterMgrFullConnector.h \
        include/SelectionCriterionTypeInterface.h \
        include/SelectionCriterionInterface.h \
        include/ParameterHandle.h

common_src_files := \
        AreaConfiguration.cpp \
        ArrayParameter.cpp \
        AutoLog.cpp \
        BaseParameter.cpp \
        BinarySerializableElement.cpp \
        BinaryStream.cpp \
        BitParameterBlock.cpp \
        BitParameterBlockType.cpp \
        BitParameter.cpp \
        BitParameterType.cpp \
        BitwiseAreaConfiguration.cpp \
        BooleanParameterType.cpp \
        ComponentInstance.cpp \
        ComponentLibrary.cpp \
        ComponentType.cpp \
        CompoundRule.cpp \
        ConfigurableDomain.cpp \
        ConfigurableDomains.cpp \
        ConfigurableElementAggregator.cpp \
        ConfigurableElement.cpp \
        ConfigurationAccessContext.cpp \
        DomainConfiguration.cpp \
        Element.cpp \
        ElementLibrary.cpp \
        ElementLibrarySet.cpp \
        ElementLocator.cpp \
        EnumParameterType.cpp \
        EnumValuePair.cpp \
        ErrorContext.cpp \
        FixedPointParameterType.cpp \
        FormattedSubsystemObject.cpp \
        FrameworkConfigurationLocation.cpp \
        HardwareBackSynchronizer.cpp \
        InstanceConfigurableElement.cpp \
        InstanceDefinition.cpp \
        IntegerParameterType.cpp \
        LinearParameterAdaptation.cpp \
        LogarithmicParameterAdaptation.cpp \
        MappingContext.cpp \
        MappingData.cpp \
        ParameterAccessContext.cpp \
        ParameterAdaptation.cpp \
        ParameterBlackboard.cpp \
        ParameterBlockType.cpp \
        Parameter.cpp \
        ParameterFrameworkConfiguration.cpp \
        ParameterHandle.cpp \
        ParameterMgr.cpp \
        ParameterMgrFullConnector.cpp \
        ParameterMgrPlatformConnector.cpp \
        ParameterType.cpp \
        PathNavigator.cpp \
        PluginLocation.cpp \
        RuleParser.cpp \
        SelectionCriteria.cpp \
        SelectionCriteriaDefinition.cpp \
        SelectionCriterion.cpp \
        SelectionCriterionLibrary.cpp \
        SelectionCriterionRule.cpp \
        SelectionCriterionType.cpp \
        SimulatedBackSynchronizer.cpp \
        StringParameter.cpp \
        StringParameterType.cpp \
        Subsystem.cpp \
        SubsystemElementBuilder.cpp \
        SubsystemObject.cpp \
        SubsystemObjectCreator.cpp \
        SyncerSet.cpp \
        SystemClass.cpp \
        TypeElement.cpp \
        VirtualSubsystem.cpp \
        VirtualSyncer.cpp \
        XmlElementSerializingContext.cpp \
        XmlFileIncluderElement.cpp \
        XmlParameterSerializingContext.cpp

common_module := libparameter
common_module_tags := optional

common_cflags := \
        -Wall \
        -Werror \
        -Wextra \
        -Wno-unused-parameter \
        -Wno-maybe-uninitialized

common_c_includes := \
    $(LOCAL_PATH)/include/ \
    $(LOCAL_PATH)/../utility/ \
    $(LOCAL_PATH)/../remote-processor/

#############################
# Target build

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO := $(common_copy_headers_to)
LOCAL_COPY_HEADERS := $(common_copy_headers)

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)

LOCAL_CFLAGS := $(common_cflags)

LOCAL_SRC_FILES := $(common_src_files)

LOCAL_MODULE := $(common_module)
LOCAL_MODULE_OWNER := intel
LOCAL_MODULE_TAGS := $(common_module_tags)

LOCAL_C_INCLUDES := $(common_c_includes)

LOCAL_SHARED_LIBRARIES := libxmlserializer libdl
LOCAL_STATIC_LIBRARIES := libpfw_utility

LOCAL_REQUIRED_MODULES := libremote-processor

include $(BUILD_SHARED_LIBRARY)

##############################
# Host build

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO := $(common_copy_headers_to)
LOCAL_COPY_HEADERS := $(common_copy_headers)

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)

LOCAL_CFLAGS := $(common_cflags) -O0 -ggdb

LOCAL_SRC_FILES := $(common_src_files)

LOCAL_MODULE := $(common_module)_host
LOCAL_MODULE_OWNER := intel
LOCAL_MODULE_TAGS := $(common_module_tags)

LOCAL_C_INCLUDES += \
    $(common_c_includes)

LOCAL_SHARED_LIBRARIES := libxmlserializer_host
LOCAL_STATIC_LIBRARIES := libpfw_utility_host libxml2

LOCAL_LDLIBS += -ldl

include $(BUILD_HOST_SHARED_LIBRARY)
