# INTEL CONFIDENTIAL
#
# Copyright 2013 Intel Corporation All Rights Reserved.
# The source code contained or described herein and all documents
# related to the source code ("Material") are owned by Intel
# Corporation or its suppliers or licensors. Title to the Material
# remains with Intel Corporation or its suppliers and licensors. The
# Material contains trade secrets and proprietary and confidential
# information of Intel or its suppliers and licensors. The Material is
# protected by worldwide copyright and trade secret laws and treaty
# provisions. No part of the Material may be used, copied, reproduced,
# modified, published, uploaded, posted, transmitted, distributed, or
# disclosed in any way without Intel's prior express written permission.
#
# No license under any patent, copyright, trade secret or other
# intellectual property right is granted to or conferred upon you by
# disclosure or delivery of the Materials, either expressly, by
# implication, inducement, estoppel or otherwise. Any license under
# such intellectual property rights must be express and approved by
# Intel in writing.

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
    $(call include-path-for, stlport) \
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

