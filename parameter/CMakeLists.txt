# Copyright (c) 2014-2015, Intel Corporation
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

if(WIN32)
    set(parameter_RESOURCE_FILE "${CMAKE_CURRENT_BINARY_DIR}/Resource.rc")
    set(parameter_OS_SPECIFIC_SRCS ${parameter_RESOURCE_FILE})
endif()

add_library(parameter SHARED
    ${parameter_OS_SPECIFIC_SRCS}
    AreaConfiguration.cpp
    ArrayParameter.cpp
    BaseParameter.cpp
    BitParameterBlock.cpp
    BitParameterBlockType.cpp
    BitParameter.cpp
    BitParameterType.cpp
    BitwiseAreaConfiguration.cpp
    BooleanParameterType.cpp
    CommandHandlerWrapper.cpp
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
    FixedPointParameterType.cpp
    FloatingPointParameterType.cpp
    FormattedSubsystemObject.cpp
    FrameworkConfigurationLocation.cpp
    HardwareBackSynchronizer.cpp
    InstanceConfigurableElement.cpp
    InstanceDefinition.cpp
    IntegerParameterType.cpp
    LinearParameterAdaptation.cpp
    LogarithmicParameterAdaptation.cpp
    LoggingElementBuilderTemplate.cpp
    MappingContext.cpp
    MappingData.cpp
    ParameterAccessContext.cpp
    ParameterAdaptation.cpp
    ParameterBlackboard.cpp
    ParameterBlockType.cpp
    Parameter.cpp
    ParameterFrameworkConfiguration.cpp
    ElementHandle.cpp
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

include(GenerateExportHeader)
generate_export_header(parameter)

if(WIN32)
    set(WINRC_MAJOR ${PF_VERSION_MAJOR})
    set(WINRC_MINOR ${PF_VERSION_MINOR})
    set(WINRC_PATCH ${PF_VERSION_PATCH})
    set(WINRC_TWEAK ${PF_VERSION_TWEAK})
    target_compile_definitions(parameter PRIVATE
        WINRC_DLL
        WINRC_FILENAME="$<TARGET_FILE_NAME:parameter>")
    set(WINRC_FILE_DESCRIPTION "Parameter Framework library")
    configure_file("${PROJECT_SOURCE_DIR}/support/windows/Resource.rc.in"
                   "${parameter_RESOURCE_FILE}")
endif()

configure_file(version.h.in "${CMAKE_CURRENT_BINARY_DIR}/version.h")

target_link_libraries(parameter
    # Unfortunatly xmlSink and xmlSource need to be exposed to the plugins
    PUBLIC xmlserializer
    PRIVATE pfw_utility remote-processor
    PRIVATE ${CMAKE_DL_LIBS})

target_include_directories(parameter
    PUBLIC include log/include
    # Export symbol macro header
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}"
    # FIXE: define . as an PUBLIC include directory only for plugins
    PUBLIC .)

install(TARGETS parameter LIBRARY DESTINATION lib RUNTIME DESTINATION bin ARCHIVE DESTINATION lib)
# Client headers
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/parameter_export.h"
    include/CommandHandlerInterface.h
    include/ElementHandle.h
    include/ParameterHandle.h
    include/ParameterMgrLoggerForward.h
    include/ParameterMgrFullConnector.h
    include/ParameterMgrPlatformConnector.h
    include/SelectionCriterionInterface.h
    include/SelectionCriterionTypeInterface.h
    DESTINATION "include/parameter/client")
# Core (plugin) headers
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/parameter_export.h"
    BitParameterBlockType.h
    ConfigurableElement.h
    DefaultElementLibrary.h
    Element.h
    ElementBuilder.h
    ElementLibrary.h
    FileIncluderElementBuilder.h
    FormattedSubsystemObject.h
    InstanceConfigurableElement.h
    LoggingElementBuilderTemplate.h
    Mapper.h
    MappingContext.h
    ParameterBlockType.h
    ParameterType.h
    PathNavigator.h
    Plugin.h
    Subsystem.h
    SubsystemLibrary.h
    SubsystemObject.h
    SubsystemObjectCreator.h
    SubsystemObjectFactory.h
    Syncer.h
    TypeElement.h
    VirtualSubsystem.h
    DESTINATION "include/parameter/plugin")
install(DIRECTORY log/include/log/
        DESTINATION "include/parameter/plugin/log")
