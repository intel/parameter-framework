# Copyright (c) 2014, Intel Corporation
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

add_library(parameter SHARED
    Subsystem.cpp
    PathNavigator.cpp
    Element.cpp
    SystemClass.cpp
    ParameterMgr.cpp
    SelectionCriteria.cpp
    SelectionCriterionLibrary.cpp
    SelectionCriteriaDefinition.cpp
    SelectionCriterionType.cpp
    ElementLibrary.cpp
    SubsystemElementBuilder.cpp
    SelectionCriterion.cpp
    ComponentLibrary.cpp
    ConfigurableElement.cpp
    MappingData.cpp
    ParameterBlackboard.cpp
    ComponentType.cpp
    TypeElement.cpp
    InstanceConfigurableElement.cpp
    ComponentInstance.cpp
    ParameterBlockType.cpp
    ParameterType.cpp
    Parameter.cpp
    BooleanParameterType.cpp
    IntegerParameterType.cpp
    ArrayParameter.cpp
    InstanceDefinition.cpp
    ParameterMgrPlatformConnector.cpp
    FixedPointParameterType.cpp
    ParameterAccessContext.cpp
    XmlFileIncluderElement.cpp
    ParameterFrameworkConfiguration.cpp
    FrameworkConfigurationLocation.cpp
    PluginLocation.cpp
    ElementLibrarySet.cpp
    ErrorContext.cpp
    AreaConfiguration.cpp
    DomainConfiguration.cpp
    ConfigurableDomain.cpp
    SyncerSet.cpp
    ConfigurableDomains.cpp
    BinaryStream.cpp
    ConfigurationAccessContext.cpp
    XmlElementSerializingContext.cpp
    XmlParameterSerializingContext.cpp
    BinarySerializableElement.cpp
    BitwiseAreaConfiguration.cpp
    BitParameterBlockType.cpp
    BitParameterBlock.cpp
    BitParameterType.cpp
    BitParameter.cpp
    ElementLocator.cpp
    ParameterMgrLogger.cpp
    AutoLog.cpp
    CompoundRule.cpp
    SelectionCriterionRule.cpp
    ConfigurableElementAggregator.cpp
    HardwareBackSynchronizer.cpp
    SimulatedBackSynchronizer.cpp
    MappingContext.cpp
    SubsystemObject.cpp
    FormattedSubsystemObject.cpp
    SubsystemObjectCreator.cpp
    BaseParameter.cpp
    StringParameterType.cpp
    StringParameter.cpp
    EnumParameterType.cpp
    VirtualSubsystem.cpp
    VirtualSyncer.cpp
    ParameterHandle.cpp
    ParameterAdaptation.cpp
    LinearParameterAdaptation.cpp
    RuleParser.cpp
    EnumValuePair.cpp
    Subsystem.cpp
    PathNavigator.cpp
    Element.cpp
    SystemClass.cpp
    ParameterMgr.cpp
    SelectionCriteria.cpp
    SelectionCriterionLibrary.cpp
    SelectionCriteriaDefinition.cpp
    SelectionCriterionType.cpp
    ElementLibrary.cpp
    SubsystemElementBuilder.cpp
    SelectionCriterion.cpp
    ComponentLibrary.cpp
    ConfigurableElement.cpp
    MappingData.cpp
    ParameterBlackboard.cpp
    ComponentType.cpp
    TypeElement.cpp
    InstanceConfigurableElement.cpp
    ComponentInstance.cpp
    ParameterBlockType.cpp
    ParameterType.cpp
    Parameter.cpp
    BooleanParameterType.cpp
    IntegerParameterType.cpp
    ArrayParameter.cpp
    InstanceDefinition.cpp
    ParameterMgrPlatformConnector.cpp
    FixedPointParameterType.cpp
    ParameterAccessContext.cpp
    XmlFileIncluderElement.cpp
    ParameterFrameworkConfiguration.cpp
    FrameworkConfigurationLocation.cpp
    PluginLocation.cpp
    ElementLibrarySet.cpp
    ErrorContext.cpp
    AreaConfiguration.cpp
    DomainConfiguration.cpp
    ConfigurableDomain.cpp
    SyncerSet.cpp
    ConfigurableDomains.cpp
    BinaryStream.cpp
    ConfigurationAccessContext.cpp
    XmlElementSerializingContext.cpp
    XmlParameterSerializingContext.cpp
    BinarySerializableElement.cpp
    BitwiseAreaConfiguration.cpp
    BitParameterBlockType.cpp
    BitParameterBlock.cpp
    BitParameterType.cpp
    BitParameter.cpp
    ElementLocator.cpp
    ParameterMgrLogger.cpp
    AutoLog.cpp
    CompoundRule.cpp
    SelectionCriterionRule.cpp
    ConfigurableElementAggregator.cpp
    HardwareBackSynchronizer.cpp
    SimulatedBackSynchronizer.cpp
    MappingContext.cpp
    SubsystemObject.cpp
    FormattedSubsystemObject.cpp
    SubsystemObjectCreator.cpp
    BaseParameter.cpp
    StringParameterType.cpp
    StringParameter.cpp
    EnumParameterType.cpp
    VirtualSubsystem.cpp
    VirtualSyncer.cpp
    ParameterHandle.cpp
    ParameterAdaptation.cpp
    LinearParameterAdaptation.cpp
    RuleParser.cpp
    EnumValuePair.cpp)

include_directories(
    include
    "${PROJECT_SOURCE_DIR}/xmlserializer"
    "${PROJECT_SOURCE_DIR}/utility"
    "${PROJECT_SOURCE_DIR}/remote-processor")

# No need to link with libremote-processor: it is accessed via dlopen()
target_link_libraries(parameter xmlserializer pfw_utility)

install(TARGETS parameter LIBRARY DESTINATION lib)
# Client headers
install(FILES
    include/ParameterHandle.h
    include/ParameterMgrPlatformConnector.h
    include/SelectionCriterionInterface.h
    include/SelectionCriterionTypeInterface.h
    DESTINATION "include/parameter/client")
# Core (plugin) headers
install(FILES
    AutoLog.h
    BitParameterBlockType.h
    ConfigurableElement.h
    ConfigurableElementWithMapping.h
    DefaultElementLibrary.h
    Element.h
    ElementBuilder.h
    ElementLibrary.h
    FileIncluderElementBuilder.h
    FormattedSubsystemObject.h
    InstanceConfigurableElement.h
    Mapper.h
    MappingContext.h
    NamedElementBuilderTemplate.h
    ParameterBlockType.h
    ParameterType.h
    PathNavigator.h
    Subsystem.h
    SubsystemLibrary.h
    SubsystemObject.h
    SubsystemObjectCreator.h
    SubsystemObjectFactory.h
    Syncer.h
    TypeElement.h
    VirtualSubsystem.h
    DESTINATION "include/parameter/plugin")
