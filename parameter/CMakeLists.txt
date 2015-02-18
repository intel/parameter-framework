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
    AreaConfiguration.cpp
    ArrayParameter.cpp
    AutoLog.cpp
    BaseParameter.cpp
    BinarySerializableElement.cpp
    BinaryStream.cpp
    BitParameterBlock.cpp
    BitParameterBlockType.cpp
    BitParameter.cpp
    BitParameterType.cpp
    BitwiseAreaConfiguration.cpp
    BooleanParameterType.cpp
    ComponentInstance.cpp
    ComponentLibrary.cpp
    ComponentType.cpp
    CompoundRule.cpp
    ConfigurableDomain.cpp
    ConfigurableDomains.cpp
    ConfigurableElementAggregator.cpp
    ConfigurableElement.cpp
    ConfigurationAccessContext.cpp
    DomainConfiguration.cpp
    Element.cpp
    ElementLibrary.cpp
    ElementLibrarySet.cpp
    ElementLocator.cpp
    EnumParameterType.cpp
    EnumValuePair.cpp
    ErrorContext.cpp
    FixedPointParameterType.cpp
    FormattedSubsystemObject.cpp
    FrameworkConfigurationLocation.cpp
    HardwareBackSynchronizer.cpp
    InstanceConfigurableElement.cpp
    InstanceDefinition.cpp
    IntegerParameterType.cpp
    LinearParameterAdaptation.cpp
    LogarithmicParameterAdaptation.cpp
    MappingContext.cpp
    MappingData.cpp
    ParameterAccessContext.cpp
    ParameterAdaptation.cpp
    ParameterBlackboard.cpp
    ParameterBlockType.cpp
    Parameter.cpp
    ParameterFrameworkConfiguration.cpp
    ParameterHandle.cpp
    ParameterMgr.cpp
    ParameterMgrFullConnector.cpp
    ParameterMgrPlatformConnector.cpp
    ParameterType.cpp
    PathNavigator.cpp
    PluginLocation.cpp
    RuleParser.cpp
    SelectionCriteria.cpp
    SelectionCriteriaDefinition.cpp
    SelectionCriterion.cpp
    SelectionCriterionLibrary.cpp
    SelectionCriterionRule.cpp
    SelectionCriterionType.cpp
    SimulatedBackSynchronizer.cpp
    StringParameter.cpp
    StringParameterType.cpp
    Subsystem.cpp
    SubsystemElementBuilder.cpp
    SubsystemObject.cpp
    SubsystemObjectCreator.cpp
    SyncerSet.cpp
    SystemClass.cpp
    TypeElement.cpp
    VirtualSubsystem.cpp
    VirtualSyncer.cpp
    XmlElementSerializingContext.cpp
    XmlFileIncluderElement.cpp
    XmlParameterSerializingContext.cpp)

include_directories(
    include
    "${PROJECT_SOURCE_DIR}/xmlserializer"
    "${PROJECT_SOURCE_DIR}/utility"
    "${PROJECT_SOURCE_DIR}/remote-processor")

# No need to link with libremote-processor: it is accessed via dlopen()
find_library(dl dl)
target_link_libraries(parameter xmlserializer pfw_utility dl)

install(TARGETS parameter LIBRARY DESTINATION lib)
# Client headers
install(FILES
    include/ParameterHandle.h
    include/ParameterMgrLoggerForward.h
    include/ParameterMgrFullConnector.h
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
