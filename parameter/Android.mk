LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)

LOCAL_COPY_HEADERS_TO := parameter

LOCAL_COPY_HEADERS := \
        ParameterMgrPlatformConnector.h \
        SelectionCriterionTypeInterface.h \
        SelectionCriterionInterface.h

LOCAL_SRC_FILES:= \
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
        SelectionCriterionDefinition.cpp \
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
        FrameworkConfigurationLocation.cpp \
        KindElement.cpp \
        ElementLibrarySet.cpp \
        ErrorContext.cpp \
        SystemClassConfiguration.cpp \
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
        SubsystemObjectCreator.cpp


LOCAL_MODULE:= libparameter

LOCAL_MODULE_TAGS := optional

TARGET_ERROR_FLAGS += -Wno-non-virtual-dtor

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../utility/ $(LOCAL_PATH)/../xmlserializer/ $(LOCAL_PATH)/../remote-processor/

LOCAL_C_INCLUDES += \
	external/stlport/stlport/ \
	bionic/libstdc++ \
	bionic/

LOCAL_C_INCLUDES += 

LOCAL_SHARED_LIBRARIES := libstlport libdl libxmlserializer
LOCAL_STATIC_LIBRARIES := libutility

LOCAL_LDLIBS += 
LOCAL_LDFLAGS +=

include $(BUILD_SHARED_LIBRARY)

# ==== export parameter script  ========================
include $(CLEAR_VARS)

LOCAL_MODULE := parameter

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := EXECUTABLES

# This will install the file in /system/bin
LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)

LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)
