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
        include/ParameterMgrPlatformConnector.h \
        include/SelectionCriterionTypeInterface.h \
        include/SelectionCriterionInterface.h \
        include/ParameterHandle.h

common_src_files := \
        Subsystem.cpp \
        PathNavigator.cpp \
        Element.cpp \
        SystemClass.cpp \
        Component.cpp \
        ParameterMgr.cpp \
        SelectionCriteria.cpp \
        SelectionCriterionLibrary.cpp \
        SelectionCriteriaDefinition.cpp \
        SelectionCriterionType.cpp \
        ElementLibrary.cpp \
        SubsystemElementBuilder.cpp \
        SubsystemLibrary.cpp \
        SelectionCriterion.cpp \
        ComponentLibrary.cpp \
        ParameterBlock.cpp \
        ConfigurableElement.cpp \
        MappingData.cpp \
        ParameterBlackboard.cpp \
        ComponentType.cpp \
        TypeElement.cpp \
        InstanceConfigurableElement.cpp \
        ComponentInstance.cpp \
        ParameterBlockType.cpp \
        ParameterType.cpp \
        Parameter.cpp \
        BooleanParameterType.cpp \
        IntegerParameterType.cpp \
        ArrayParameter.cpp \
        InstanceDefinition.cpp \
        ParameterMgrPlatformConnector.cpp \
        FixedPointParameterType.cpp \
        ParameterAccessContext.cpp \
        XmlFileIncluderElement.cpp \
        ParameterFrameworkConfiguration.cpp \
        FrameworkConfigurationGroup.cpp \
        SubsystemPlugins.cpp \
        FrameworkConfigurationLocation.cpp \
        PluginLocation.cpp \
        KindElement.cpp \
        ElementLibrarySet.cpp \
        ErrorContext.cpp \
        AreaConfiguration.cpp \
        DomainConfiguration.cpp \
        ConfigurableDomain.cpp \
        SyncerSet.cpp \
        ConfigurableDomains.cpp \
        BinaryStream.cpp \
        ConfigurationAccessContext.cpp \
        XmlElementSerializingContext.cpp \
        XmlParameterSerializingContext.cpp \
        XmlDomainSerializingContext.cpp \
        BinarySerializableElement.cpp \
        BitwiseAreaConfiguration.cpp \
        BitParameterBlockType.cpp \
        BitParameterBlock.cpp \
        BitParameterType.cpp \
        BitParameter.cpp \
        ElementLocator.cpp \
        ParameterMgrLogger.cpp \
        AutoLog.cpp \
        Rule.cpp \
        CompoundRule.cpp \
        SelectionCriterionRule.cpp \
        ConfigurableElementAggregator.cpp \
        BackSynchronizer.cpp \
        HardwareBackSynchronizer.cpp \
        SimulatedBackSynchronizer.cpp \
        MappingContext.cpp \
        SubsystemObject.cpp \
        FormattedSubsystemObject.cpp \
        SubsystemObjectCreator.cpp \
        BaseParameter.cpp \
        AutoLock.cpp \
        StringParameterType.cpp \
        StringParameter.cpp \
        EnumParameterType.cpp \
        VirtualSubsystem.cpp \
        VirtualSyncer.cpp \
        ParameterHandle.cpp \
        ParameterAdaptation.cpp \
        LinearParameterAdaptation.cpp \
        RuleParser.cpp \
        EnumValuePair.cpp

common_module := libparameter
common_module_tags := optional

common_cflags := \
        -Wall \
        -Werror \
        -Wextra \
        -Wno-unused-parameter

common_c_includes := \
    $(LOCAL_PATH)/include/ \
    $(LOCAL_PATH)/../utility/ \
    $(LOCAL_PATH)/../xmlserializer/ \
    $(LOCAL_PATH)/../remote-processor/

common_shared_libraries := libicuuc
common_static_libraries := libxml2

#############################
# Target build

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO := $(common_copy_headers_to)
LOCAL_COPY_HEADERS := $(common_copy_headers)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

LOCAL_CFLAGS := $(common_cflags)

LOCAL_SRC_FILES := $(common_src_files)

LOCAL_MODULE := $(common_module)
LOCAL_MODULE_TAGS := $(common_module_tags)

LOCAL_C_INCLUDES += \
    $(common_c_includes) \
    external/stlport/stlport \
    bionic/

LOCAL_SHARED_LIBRARIES := $(common_shared_libraries) libdl libstlport
LOCAL_STATIC_LIBRARIES := libxmlserializer libutility $(common_static_libraries)

include $(BUILD_SHARED_LIBRARY)

##############################
# Host build

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO := $(common_copy_headers_to)
LOCAL_COPY_HEADERS := $(common_copy_headers)

LOCAL_CFLAGS := $(common_cflags) -O0 -ggdb

LOCAL_SRC_FILES := $(common_src_files)

LOCAL_MODULE := $(common_module)_host
LOCAL_MODULE_TAGS := $(common_module_tags)

LOCAL_C_INCLUDES += \
    $(common_c_includes)

LOCAL_SHARED_LIBRARIES := $(common_shared_libraries)-host
LOCAL_STATIC_LIBRARIES := libxmlserializer_host libutility_host $(common_static_libraries)

LOCAL_LDLIBS += -ldl

include $(BUILD_HOST_SHARED_LIBRARY)

################################
# Export includes for plugins (Target build)

include $(CLEAR_VARS)

LOCAL_MODULE := $(common_module)_includes

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)

LOCAL_STATIC_LIBRARIES := \
    libxmlserializer \
    libutility \
    $(common_static_libraries)

include $(BUILD_STATIC_LIBRARY)

################################
# Export includes for plugins (Host build)

include $(CLEAR_VARS)

LOCAL_MODULE := $(common_module)_includes_host

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)

LOCAL_STATIC_LIBRARIES := \
    libxmlserializer_host \
    libutility_host \
    $(common_static_libraries)

include $(BUILD_HOST_STATIC_LIBRARY)

