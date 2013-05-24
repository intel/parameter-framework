LOCAL_PATH := $(call my-dir)

####################
# Common definitions

COMMON_COPY_HEADERS_TO := parameter

COMMON_COPY_HEADERS := \
        include/ParameterMgrPlatformConnector.h \
        include/SelectionCriterionTypeInterface.h \
        include/SelectionCriterionInterface.h \
        include/ParameterHandle.h

COMMON_SRC_FILES := \
        Subsystem.cpp \
        PathNavigator.cpp \
        Element.cpp \
        SystemClass.cpp \
        ElementBuilder.cpp \
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

COMMON_MODULE := libparameter
COMMON_MODULE_TAGS := optional

COMMON_CFLAGS := -Wno-non-virtual-dtor

COMMON_C_INCLUDES := \
    $(LOCAL_PATH)/include/ \
    $(LOCAL_PATH)/../utility/ \
    $(LOCAL_PATH)/../xmlserializer/ \
    $(LOCAL_PATH)/../remote-processor/

COMMON_SHARED_LIBRARIES := libicuuc
COMMON_STATIC_LIBRARIES := libxmlserializer libutility libxml2

#############################
# Target build

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO := $(COMMON_COPY_HEADERS_TO)
LOCAL_COPY_HEADERS := $(COMMON_COPY_HEADERS)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

LOCAL_SRC_FILES := $(COMMON_SRC_FILES)

LOCAL_MODULE := $(COMMON_MODULE)
LOCAL_MODULE_TAGS := $(COMMON_MODULE_TAGS)

LOCAL_CFLAGS += $(COMMON_CFLAGS)

LOCAL_C_INCLUDES += \
    $(COMMON_C_INCLUDES) \
    external/stlport/stlport/ \
    bionic/

LOCAL_SHARED_LIBRARIES := $(COMMON_SHARED_LIBRARIES) libdl libstlport
LOCAL_STATIC_LIBRARIES := $(COMMON_STATIC_LIBRARIES)

include $(BUILD_SHARED_LIBRARY)

##############################
# Host build

include $(CLEAR_VARS)

LOCAL_COPY_HEADERS_TO := $(COMMON_COPY_HEADERS_TO)
LOCAL_COPY_HEADERS := $(COMMON_COPY_HEADERS)

LOCAL_SRC_FILES := $(COMMON_SRC_FILES)

LOCAL_MODULE := $(COMMON_MODULE)
LOCAL_MODULE_TAGS := $(COMMON_MODULE_TAGS)

LOCAL_CFLAGS += $(COMMON_CFLAGS)

LOCAL_C_INCLUDES += \
    $(COMMON_C_INCLUDES)

LOCAL_SHARED_LIBRARIES := $(COMMON_SHARED_LIBRARIES)
LOCAL_STATIC_LIBRARIES := $(COMMON_STATIC_LIBRARIES)

LOCAL_LDLIBS += -ldl

include $(BUILD_HOST_SHARED_LIBRARY)
