/*
 * Copyright (c) 2011-2016, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "version.h"
#include "ParameterMgr.h"
#include "ConfigurationAccessContext.h"
#include "XmlParameterSerializingContext.h"
#include "XmlElementSerializingContext.h"
#include "SystemClass.h"
#include "ElementLibrarySet.h"
#include "SubsystemLibrary.h"
#include "NamedElementBuilderTemplate.h"
#include "KindElementBuilderTemplate.h"
#include "ElementBuilderTemplate.h"
#include "SelectionCriterionType.h"
#include "SubsystemElementBuilder.h"
#include "FileIncluderElementBuilder.h"
#include "SelectionCriteria.h"
#include "ComponentType.h"
#include "ComponentInstance.h"
#include "ParameterBlockType.h"
#include "BooleanParameterType.h"
#include "IntegerParameterType.h"
#include "FixedPointParameterType.h"
#include "FloatingPointParameterType.h"
#include "ParameterBlackboard.h"
#include "Parameter.h"
#include "ParameterAccessContext.h"
#include "ParameterFrameworkConfiguration.h"
#include "FrameworkConfigurationGroup.h"
#include "PluginLocation.h"
#include "SubsystemPlugins.h"
#include "FrameworkConfigurationLocation.h"
#include "ConfigurableDomains.h"
#include "ConfigurableDomain.h"
#include "DomainConfiguration.h"
#include "XmlDomainSerializingContext.h"
#include "XmlDomainExportContext.h"
#include "XmlDomainImportContext.h"
#include "BitParameterBlockType.h"
#include "BitParameterType.h"
#include "StringParameterType.h"
#include "EnumParameterType.h"
#include "BackgroundRemoteProcessorServer.h"
#include "ElementLocator.h"
#include "CompoundRule.h"
#include "SelectionCriterionRule.h"
#include "SimulatedBackSynchronizer.h"
#include "HardwareBackSynchronizer.h"
#include <cassert>
#include "ParameterHandle.h"
#include "LinearParameterAdaptation.h"
#include "LogarithmicParameterAdaptation.h"
#include "EnumValuePair.h"
#include "Subsystem.h"
#include "XmlStreamDocSink.h"
#include "XmlMemoryDocSink.h"
#include "XmlDocSource.h"
#include "XmlMemoryDocSource.h"
#include "SelectionCriteriaDefinition.h"
#include "Utility.h"
#include "Memory.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <mutex>
#include <iomanip>
#include "convert.hpp"

#define base CElement

/** Private macro helper to declare a new context
 *
 * Context declaration always need logger and logging prefix to be
 * passed as parameters.
 * This macro aims to avoid this boring notation.
 * This macro should be called only once in a scope. Nested scopes can
 * call this macro too, as variable shadowing is supported.
 */
#define LOG_CONTEXT(contextTitle) core::log::Context context(_logger, contextTitle)

#ifdef SIMULATION
// In simulation, back synchronization of the blackboard won't probably work
// We need to ensure though the blackboard is initialized with valid data
typedef CSimulatedBackSynchronizer BackSynchronizer;
#else
// Real back synchronizer from subsystems
typedef CHardwareBackSynchronizer BackSynchronizer;
#endif

using std::string;
using std::list;
using std::vector;
using std::ostringstream;
using std::ofstream;
using std::ifstream;
using std::mutex;
using std::lock_guard;

// FIXME: integrate ParameterMgr to core namespace
using namespace core;

// Used for remote processor server creation
typedef IRemoteProcessorServerInterface *(*CreateRemoteProcessorServer)(
    uint16_t uiPort, IRemoteCommandHandler *pCommandHandler);

// Config File System looks normally like this:
// ---------------------------------------------
//|-- <ParameterFrameworkConfiguration>.xml
//|-- schemas
//|   `-- *.xsd
//|-- Settings
//|   `-- <SystemClassName folder>*
//|       |-- <ConfigurableDomains>.xml
//|       `-- <Settings>.bin?
//`-- Structure
//    `-- <SystemClassName folder>*
//        |-- <SystemClassName>Class.xml
//        `-- <Subsystem>.xml*
// --------------------------------------------

// Remote command parser array
const CParameterMgr::SRemoteCommandParserItem CParameterMgr::gastRemoteCommandParserItems[] = {

    /// Version
    {"version", &CParameterMgr::versionCommandProcess, 0, "", "Show version"},

    /// Status
    {"status", &CParameterMgr::statusCommandProcess, 0, "", "Show current status"},

    /// Tuning Mode
    {"setTuningMode", &CParameterMgr::setTuningModeCommandProcess, 1, "on|off*",
     "Turn on or off Tuning Mode"},
    {"getTuningMode", &CParameterMgr::getTuningModeCommandProcess, 0, "", "Show Tuning Mode"},

    /// Value Space
    {"setValueSpace", &CParameterMgr::setValueSpaceCommandProcess, 1, "raw|real*",
     "Assigns Value Space used for parameter value interpretation"},
    {"getValueSpace", &CParameterMgr::getValueSpaceCommandProcess, 0, "", "Show Value Space"},

    /// Output Raw Format
    {"setOutputRawFormat", &CParameterMgr::setOutputRawFormatCommandProcess, 1, "dec*|hex",
     "Assigns format used to output parameter values when in raw Value Space"},
    {"getOutputRawFormat", &CParameterMgr::getOutputRawFormatCommandProcess, 0, "",
     "Show Output Raw Format"},

    /// Sync
    {"setAutoSync", &CParameterMgr::setAutoSyncCommandProcess, 1, "on*|off",
     "Turn on or off automatic synchronization to hardware while in Tuning Mode"},
    {"getAutoSync", &CParameterMgr::getAutoSyncCommandProcess, 0, "", "Show Auto Sync state"},
    {"sync", &CParameterMgr::syncCommandProcess, 0, "",
     "Synchronize current settings to hardware while in Tuning Mode and Auto Sync off"},

    /// Criteria
    {"listCriteria", &CParameterMgr::listCriteriaCommandProcess, 0, "[CSV|XML]",
     "List selection criteria"},

    /// Domains
    {"listDomains", &CParameterMgr::listDomainsCommandProcess, 0, "", "List configurable domains"},
    {"dumpDomains", &CParameterMgr::dumpDomainsCommandProcess, 0, "",
     "Show all domains and configurations, including applicability conditions"},
    {"createDomain", &CParameterMgr::createDomainCommandProcess, 1, "<domain>",
     "Create new configurable domain"},
    {"deleteDomain", &CParameterMgr::deleteDomainCommandProcess, 1, "<domain>",
     "Delete configurable domain"},
    {"deleteAllDomains", &CParameterMgr::deleteAllDomainsCommandProcess, 0, "",
     "Delete all configurable domains"},
    {"renameDomain", &CParameterMgr::renameDomainCommandProcess, 2, "<domain> <new name>",
     "Rename configurable domain"},
    {"setSequenceAwareness", &CParameterMgr::setSequenceAwarenessCommandProcess, 1,
     "<domain> true|false*", "Set configurable domain sequence awareness"},
    {"getSequenceAwareness", &CParameterMgr::getSequenceAwarenessCommandProcess, 1, "<domain>",
     "Get configurable domain sequence awareness"},
    {"listDomainElements", &CParameterMgr::listDomainElementsCommandProcess, 1, "<domain>",
     "List elements associated to configurable domain"},
    {"addElement", &CParameterMgr::addElementCommandProcess, 2, "<domain> <elem path>",
     "Associate element at given path to configurable domain"},
    {"removeElement", &CParameterMgr::removeElementCommandProcess, 2, "<domain> <elem path>",
     "Dissociate element at given path from configurable domain"},
    {"splitDomain", &CParameterMgr::splitDomainCommandProcess, 2, "<domain> <elem path>",
     "Split configurable domain at given associated element path"},

    /// Configurations
    {"listConfigurations", &CParameterMgr::listConfigurationsCommandProcess, 1, "<domain>",
     "List domain configurations"},
    {"createConfiguration", &CParameterMgr::createConfigurationCommandProcess, 2,
     "<domain> <configuration>", "Create new domain configuration"},
    {"deleteConfiguration", &CParameterMgr::deleteConfigurationCommandProcess, 2,
     "<domain> <configuration>", "Delete domain configuration"},
    {"renameConfiguration", &CParameterMgr::renameConfigurationCommandProcess, 3,
     "<domain> <configuration> <new name>", "Rename domain configuration"},
    {"saveConfiguration", &CParameterMgr::saveConfigurationCommandProcess, 2,
     "<domain> <configuration>", "Save current settings into configuration"},
    {"restoreConfiguration", &CParameterMgr::restoreConfigurationCommandProcess, 2,
     "<domain> <configuration>", "Restore current settings from configuration"},
    {"setElementSequence", &CParameterMgr::setElementSequenceCommandProcess, 3,
     "<domain> <configuration> <elem path list>",
     "Set element application order for configuration"},
    {"getElementSequence", &CParameterMgr::getElementSequenceCommandProcess, 2,
     "<domain> <configuration>", "Get element application order for configuration"},
    {"setRule", &CParameterMgr::setRuleCommandProcess, 3, "<domain> <configuration> <rule>",
     "Set configuration application rule"},
    {"clearRule", &CParameterMgr::clearRuleCommandProcess, 2, "<domain> <configuration>",
     "Clear configuration application rule"},
    {"getRule", &CParameterMgr::getRuleCommandProcess, 2, "<domain> <configuration>",
     "Get configuration application rule"},

    /// Elements/Parameters
    {"listElements", &CParameterMgr::listElementsCommandProcess, 1, "<elem path>|/",
     "List elements under element at given path or root"},
    {"listParameters", &CParameterMgr::listParametersCommandProcess, 1, "<elem path>|/",
     "List parameters under element at given path or root"},
    {"getElementStructureXML", &CParameterMgr::getElementStructureXMLCommandProcess, 1,
     "<elem path>", "Get structure of element at given path in XML format"},
    {"getElementBytes", &CParameterMgr::getElementBytesCommandProcess, 1, "<elem path>",
     "Get settings of element at given path in Byte Array format"},
    {"setElementBytes", &CParameterMgr::setElementBytesCommandProcess, 2, "<elem path> <values>",
     "Set settings of element at given path in Byte Array format"},
    {"getElementXML", &CParameterMgr::getElementXMLCommandProcess, 1, "<elem path>",
     "Get settings of element at given path in XML format"},
    {"setElementXML", &CParameterMgr::setElementXMLCommandProcess, 2, "<elem path> <values>",
     "Set settings of element at given path in XML format"},
    {"dumpElement", &CParameterMgr::dumpElementCommandProcess, 1, "<elem path>",
     "Dump structure and content of element at given path"},
    {"getElementSize", &CParameterMgr::getElementSizeCommandProcess, 1, "<elem path>",
     "Show size of element at given path"},
    {"showProperties", &CParameterMgr::showPropertiesCommandProcess, 1, "<elem path>",
     "Show properties of element at given path"},
    {"getParameter", &CParameterMgr::getParameterCommandProcess, 1, "<param path>",
     "Get value for parameter at given path"},
    {"setParameter", &CParameterMgr::setParameterCommandProcess, 2, "<param path> <value>",
     "Set value for parameter at given path"},
    {"listBelongingDomains", &CParameterMgr::listBelongingDomainsCommandProcess, 1, "<elem path>",
     "List domain(s) element at given path belongs to"},
    {"listAssociatedDomains", &CParameterMgr::listAssociatedDomainsCommandProcess, 1, "<elem path>",
     "List domain(s) element at given path is associated to"},
    {"getConfigurationParameter", &CParameterMgr::getConfigurationParameterCommandProcess, 3,
     "<domain> <configuration> <param path>",
     "Get value for parameter at given path from configuration"},
    {"setConfigurationParameter", &CParameterMgr::setConfigurationParameterCommandProcess, 4,
     "<domain> <configuration> <param path> <value>",
     "Set value for parameter at given path to configuration"},
    {"showMapping", &CParameterMgr::showMappingCommandProcess, 1, "<elem path>",
     "Show mapping for an element at given path"},

    /// Browse
    {"listAssociatedElements", &CParameterMgr::listAssociatedElementsCommandProcess, 0, "",
     "List element sub-trees associated to at least one configurable domain"},
    {"listConflictingElements", &CParameterMgr::listConflictingElementsCommandProcess, 0, "",
     "List element sub-trees contained in more than one configurable domain"},
    {"listRogueElements", &CParameterMgr::listRogueElementsCommandProcess, 0, "",
     "List element sub-trees owned by no configurable domain"},

    /// Settings Import/Export
    {"exportDomainsXML", &CParameterMgr::exportDomainsXMLCommandProcess, 1, "<file path> ",
     "Export domains to an XML file (provide an absolute path or relative"
     "to the client's working directory)"},
    {"importDomainsXML", &CParameterMgr::importDomainsXMLCommandProcess, 1, "<file path>",
     "Import domains from an XML file (provide an absolute path or relative"
     "to the client's working directory)"},
    {"exportDomainsWithSettingsXML", &CParameterMgr::exportDomainsWithSettingsXMLCommandProcess, 1,
     "<file path> ",
     "Export domains including settings to XML file (provide an absolute path or relative"
     "to the client's working directory)"},
    {"exportDomainWithSettingsXML", &CParameterMgr::exportDomainWithSettingsXMLCommandProcess, 2,
     "<domain> <file path> ", "Export a single given domain including settings to XML file"
                              " (provide an absolute path or relative to the client's"
                              " working directory)"},
    {"importDomainsWithSettingsXML", &CParameterMgr::importDomainsWithSettingsXMLCommandProcess, 1,
     "<file path>",
     "Import domains including settings from XML file (provide an absolute path or relative"
     "to the client's working directory)"},
    {"importDomainWithSettingsXML", &CParameterMgr::importDomainWithSettingsXMLCommandProcess, 1,
     "<file path> [overwrite]",
     "Import a single domain including settings from XML file."
     " Does not overwrite an existing domain unless 'overwrite' is passed as second"
     " argument. Provide an absolute path or relative to the client's working directory)"},
    {"getDomainsWithSettingsXML", &CParameterMgr::getDomainsWithSettingsXMLCommandProcess, 0, "",
     "Print domains including settings as XML"},
    {"getDomainWithSettingsXML", &CParameterMgr::getDomainWithSettingsXMLCommandProcess, 1,
     "<domain>", "Print the given domain including settings as XML"},
    {"setDomainsWithSettingsXML", &CParameterMgr::setDomainsWithSettingsXMLCommandProcess, 1,
     "<xml configurable domains>", "Import domains including settings from XML string"},
    {"setDomainWithSettingsXML", &CParameterMgr::setDomainWithSettingsXMLCommandProcess, 1,
     "<xml configurable domain> [overwrite]",
     "Import domains including settings from XML"
     " string. Does not overwrite an existing domain unless 'overwrite' is passed as second"
     " argument"},
    /// Structure Export
    {"getSystemClassXML", &CParameterMgr::getSystemClassXMLCommandProcess, 0, "",
     "Print parameter structure as XML"},
    /// Deprecated Commands
    {"getDomainsXML", &CParameterMgr::getDomainsWithSettingsXMLCommandProcess, 0, "",
     "DEPRECATED COMMAND, please use getDomainsWithSettingsXML"},

};

// Remote command parsers array Size
CParameterMgr::CParameterMgr(const string &strConfigurationFilePath, log::ILogger &logger)
    : _pMainParameterBlackboard(new CParameterBlackboard),
      _pElementLibrarySet(new CElementLibrarySet),
      _xmlConfigurationUri(CXmlDocSource::mkUri(strConfigurationFilePath, "")), _logger(logger)
{
    // Deal with children
    addChild(new CParameterFrameworkConfiguration);
    addChild(new CSelectionCriteria);
    addChild(new CSystemClass(_logger));
    addChild(new CConfigurableDomains);
}

CParameterMgr::~CParameterMgr()
{
    // Children
    delete _pRemoteProcessorServer;
    delete _pMainParameterBlackboard;
    delete _pElementLibrarySet;
}

string CParameterMgr::getKind() const
{
    return "ParameterMgr";
}

// Version
string CParameterMgr::getVersion() const
{
    return PARAMETER_FRAMEWORK_VERSION;
}

bool CParameterMgr::load(string &strError)
{
    LOG_CONTEXT("Loading");

    feedElementLibraries();

    // Load Framework configuration
    if (!loadFrameworkConfiguration(strError)) {

        return false;
    }

    if (!loadSubsystems(strError)) {

        return false;
    }

    // Load structure
    if (!loadStructure(strError)) {

        return false;
    }

    // Load settings
    if (!loadSettings(strError)) {

        return false;
    }

    // Init flow of element tree
    if (!init(strError)) {

        return false;
    }

    {
        LOG_CONTEXT("Main blackboard back synchronization");

        // Back synchronization for areas in parameter blackboard not covered by any domain
        BackSynchronizer(getConstSystemClass(), _pMainParameterBlackboard).sync();
    }

    // We're done loading the settings and back synchronizing
    CConfigurableDomains *pConfigurableDomains = getConfigurableDomains();

    // We need to ensure all domains are valid
    pConfigurableDomains->validate(_pMainParameterBlackboard);

    // Log selection criterion states
    {
        LOG_CONTEXT("Criterion states");

        const CSelectionCriteria *selectionCriteria = getConstSelectionCriteria();

        list<string> criteria;
        selectionCriteria->listSelectionCriteria(criteria, true, false);

        info() << criteria;
    }

    // Subsystem can not ask for resync as they have not been synced yet
    getSystemClass()->cleanSubsystemsNeedToResync();

    // At initialization, check subsystems that need resync
    doApplyConfigurations(true);

    // Start remote processor server if appropriate
    return handleRemoteProcessingInterface(strError);
}

bool CParameterMgr::loadFrameworkConfiguration(string &strError)
{
    LOG_CONTEXT("Loading framework configuration");

    // Parse Structure XML file
    CXmlElementSerializingContext elementSerializingContext(strError);

    _xmlDoc *doc =
        CXmlDocSource::mkXmlDoc(_xmlConfigurationUri, true, true, elementSerializingContext);
    if (doc == NULL) {
        return false;
    }

    if (!xmlParse(elementSerializingContext, getFrameworkConfiguration(), doc, _xmlConfigurationUri,
                  EFrameworkConfigurationLibrary)) {

        return false;
    }
    // Set class name to system class and configurable domains
    getSystemClass()->setName(getConstFrameworkConfiguration()->getSystemClassName());
    getConfigurableDomains()->setName(getConstFrameworkConfiguration()->getSystemClassName());

    // Get subsystem plugins elements
    _pSubsystemPlugins = static_cast<const CSubsystemPlugins *>(
        getConstFrameworkConfiguration()->findChild("SubsystemPlugins"));

    if (!_pSubsystemPlugins) {

        strError = "Parameter Framework Configuration: couldn't find SubsystemPlugins element";

        return false;
    }

    // Log tuning availability
    info() << "Tuning "
           << (getConstFrameworkConfiguration()->isTuningAllowed() ? "allowed" : "prohibited");

    return true;
}

bool CParameterMgr::loadSubsystems(std::string &error)
{
    LOG_CONTEXT("Loading subsystem plugins");

    // Load subsystems
    bool isSuccess =
        getSystemClass()->loadSubsystems(error, _pSubsystemPlugins, !_bFailOnMissingSubsystem);

    if (isSuccess) {
        info() << "All subsystem plugins successfully loaded";

        if (!error.empty()) {
            // Log missing subsystems as info
            info() << error;
        }
    } else {
        warning() << error;
    }
    return isSuccess;
}

bool CParameterMgr::loadStructure(string &strError)
{
    // Retrieve system to load structure to
    CSystemClass *pSystemClass = getSystemClass();

    LOG_CONTEXT("Loading " + pSystemClass->getName() + " system class structure");

    // Get structure description element
    const CFrameworkConfigurationLocation *pStructureDescriptionFileLocation =
        static_cast<const CFrameworkConfigurationLocation *>(
            getConstFrameworkConfiguration()->findChildOfKind("StructureDescriptionFileLocation"));

    if (!pStructureDescriptionFileLocation) {

        strError = "No StructureDescriptionFileLocation element found for SystemClass " +
                   pSystemClass->getName();

        return false;
    }

    // Parse Structure XML file
    CParameterAccessContext accessContext(strError);
    CXmlParameterSerializingContext parameterBuildContext(accessContext, strError);

    {
        // Get structure URI
        string structureUri =
            CXmlDocSource::mkUri(_xmlConfigurationUri, pStructureDescriptionFileLocation->getUri());

        LOG_CONTEXT("Importing system structure from file " + structureUri);

        _xmlDoc *doc = CXmlDocSource::mkXmlDoc(structureUri, true, true, parameterBuildContext);
        if (doc == NULL) {
            return false;
        }

        if (!xmlParse(parameterBuildContext, pSystemClass, doc, structureUri,
                      EParameterCreationLibrary)) {

            return false;
        }
    }

    // Initialize offsets
    pSystemClass->setOffset(0);

    // Initialize main blackboard's size
    _pMainParameterBlackboard->setSize(pSystemClass->getFootPrint());

    return true;
}

bool CParameterMgr::loadSettings(string &strError)
{
    string strLoadError;
    bool success = loadSettingsFromConfigFile(strLoadError);

    if (!success && !_bFailOnFailedSettingsLoad) {
        // Load can not fail, ie continue but log the load errors
        warning() << strLoadError;
        warning() << "Failed to load settings, continue without domains.";
        success = true;
    }

    if (!success) {
        // Propagate the litteral error only if the function fails
        strError = strLoadError;
        return false;
    }

    return true;
}

bool CParameterMgr::loadSettingsFromConfigFile(string &strError)
{
    LOG_CONTEXT("Loading settings");

    // Get settings configuration element
    const CFrameworkConfigurationGroup *pParameterConfigurationGroup =
        static_cast<const CFrameworkConfigurationGroup *>(
            getConstFrameworkConfiguration()->findChildOfKind("SettingsConfiguration"));

    if (!pParameterConfigurationGroup) {

        // No settings to load

        return true;
    }

    // Get configurable domains element
    const CFrameworkConfigurationLocation *pConfigurableDomainsFileLocation =
        static_cast<const CFrameworkConfigurationLocation *>(
            pParameterConfigurationGroup->findChildOfKind("ConfigurableDomainsFileLocation"));

    if (!pConfigurableDomainsFileLocation) {

        strError = "No ConfigurableDomainsFileLocation element found for SystemClass " +
                   getSystemClass()->getName();

        return false;
    }
    // Get destination root element
    CConfigurableDomains *pConfigurableDomains = getConfigurableDomains();

    // Get Xml configuration domains URI
    string configurationDomainsUri =
        CXmlDocSource::mkUri(_xmlConfigurationUri, pConfigurableDomainsFileLocation->getUri());

    // Parse configuration domains XML file
    CXmlDomainImportContext xmlDomainImportContext(strError, true, *getSystemClass());

    // Selection criteria definition for rule creation
    xmlDomainImportContext.setSelectionCriteriaDefinition(
        getConstSelectionCriteria()->getSelectionCriteriaDefinition());

    // Auto validation of configurations
    xmlDomainImportContext.setAutoValidationRequired(true);

    info() << "Importing configurable domains from file " << configurationDomainsUri
           << " with settings";

    _xmlDoc *doc =
        CXmlDocSource::mkXmlDoc(configurationDomainsUri, true, true, xmlDomainImportContext);
    if (doc == NULL) {
        return false;
    }

    return xmlParse(xmlDomainImportContext, pConfigurableDomains, doc, _xmlConfigurationUri,
                    EParameterConfigurationLibrary, true, "SystemClassName");
}

// XML parsing
bool CParameterMgr::xmlParse(CXmlElementSerializingContext &elementSerializingContext,
                             CElement *pRootElement, _xmlDoc *doc, const string &baseUri,
                             CParameterMgr::ElementLibrary eElementLibrary, bool replace,
                             const string &strNameAttributeName)
{
    // Init serializing context
    elementSerializingContext.set(_pElementLibrarySet->getElementLibrary(eElementLibrary), baseUri);

    CXmlDocSource docSource(doc, _bValidateSchemasOnStart, pRootElement->getXmlElementName(),
                            pRootElement->getName(), strNameAttributeName);

    docSource.setSchemaBaseUri(getSchemaUri());

    // Start clean
    auto clean = [replace, &pRootElement] {
        if (replace) {
            pRootElement->clean();
        }
    };
    clean();

    CXmlMemoryDocSink memorySink(pRootElement);

    if (!memorySink.process(docSource, elementSerializingContext)) {
        clean();
        return false;
    }

    return true;
}

// Init
bool CParameterMgr::init(string &strError)
{
    return base::init(strError);
}

// Selection criteria interface
CSelectionCriterionType *CParameterMgr::createSelectionCriterionType(bool bIsInclusive)
{
    // Propagate
    return getSelectionCriteria()->createSelectionCriterionType(bIsInclusive);
}

CSelectionCriterion *CParameterMgr::createSelectionCriterion(
    const string &strName, const CSelectionCriterionType *pSelectionCriterionType)
{
    // Propagate
    return getSelectionCriteria()->createSelectionCriterion(strName, pSelectionCriterionType,
                                                            _logger);
}

// Selection criterion retrieval
CSelectionCriterion *CParameterMgr::getSelectionCriterion(const string &strName)
{
    // Propagate
    return getSelectionCriteria()->getSelectionCriterion(strName);
}

// Configuration application
void CParameterMgr::applyConfigurations()
{
    LOG_CONTEXT("Configuration application request");

    // Lock state
    lock_guard<mutex> autoLock(getBlackboardMutex());

    if (!_bTuningModeIsOn) {

        // Apply configuration(s)
        doApplyConfigurations(false);
    } else {

        warning() << "Configurations were not applied because the TuningMode is on";
    }
}

const CConfigurableElement *CParameterMgr::getConfigurableElement(const string &strPath,
                                                                  string &strError) const
{
    CPathNavigator pathNavigator(strPath);

    // Nagivate through system class
    if (!pathNavigator.navigateThrough(getConstSystemClass()->getName(), strError)) {

        return NULL;
    }

    // Find element
    const CElement *pElement = getConstSystemClass()->findDescendant(pathNavigator);

    if (!pElement) {

        strError = "Path not found: " + strPath;

        return NULL;
    }

    // Check found element is a parameter
    const CConfigurableElement *pConfigurableElement =
        static_cast<const CConfigurableElement *>(pElement);

    return pConfigurableElement;
}

CConfigurableElement *CParameterMgr::getConfigurableElement(const string &strPath, string &strError)
{
    // Implement the mutable version by calling the const one and removing
    // the const from the result.
    const auto *constThis = this;
    return const_cast<CConfigurableElement *>(constThis->getConfigurableElement(strPath, strError));
}

// Dynamic parameter handling
CParameterHandle *CParameterMgr::createParameterHandle(const string &strPath, string &strError)
{
    CConfigurableElement *pConfigurableElement = getConfigurableElement(strPath, strError);

    if (!pConfigurableElement) {

        // Element not found
        strError = "Element not found: " + strPath;
        return NULL;
    }

    if (!pConfigurableElement->isParameter()) {

        // Element is not parameter
        strError = "Not a parameter: " + strPath;

        return NULL;
    }

    // Convert as parameter and return new handle
    return new CParameterHandle(static_cast<CBaseParameter &>(*pConfigurableElement), *this);
}

// Dynamic element handling
ElementHandle *CParameterMgr::createElementHandle(const std::string &path, std::string &error)
{
    CConfigurableElement *pConfigurableElement;

    if (path == "/") {
        // Attempt to access root configurable element
        pConfigurableElement = getSystemClass();
    } else {
        pConfigurableElement = getConfigurableElement(path, error);
    }

    if (!pConfigurableElement) {

        // Element not found
        error = "Element not found: " + path;
        return nullptr;
    }

    // The only reason why a heap object is returned instead of retuning by copy
    // is to inform the client of a failure through a nullptr.
    // It could be avoided (return by copy) with an
    //  - optional equivalent (see boost::optional or std::experimental::optional)
    //  - exception (but the api is noexcept)
    return new ElementHandle(*pConfigurableElement, *this);
}

void CParameterMgr::getSettingsAsBytes(const CConfigurableElement &element,
                                       std::vector<uint8_t> &settings) const
{
    // Not useful as the get can not fail,
    // but the current design forces all serialization and deserialization to
    // have an error out string
    std::string error;

    // Prepare parameter access context for main blackboard.
    // No need to handle output raw format and value space as Byte arrays are hexa formatted
    CParameterAccessContext parameterAccessContext(error);
    parameterAccessContext.setParameterBlackboard(_pMainParameterBlackboard);

    // Get the settings
    element.getSettingsAsBytes(settings, parameterAccessContext);
}

bool CParameterMgr::setSettingsAsBytes(const CConfigurableElement &element,
                                       const std::vector<uint8_t> &settings, std::string &error)
{
    // Prepare parameter access context for main blackboard.
    // Notes:
    //     - No need to handle output raw format and value space as Byte arrays are interpreted as
    //     raw formatted
    //     - No check is done as to the intgrity of the input data.
    //       This may lead to undetected out of range value assignment.
    //       Use this functionality with caution
    CParameterAccessContext parameterAccessContext(error);
    parameterAccessContext.setParameterBlackboard(_pMainParameterBlackboard);
    parameterAccessContext.setAutoSync(autoSyncOn());

    // Set the settings
    return element.setSettingsAsBytes(settings, parameterAccessContext);
}

void CParameterMgr::setFailureOnMissingSubsystem(bool bFail)
{
    _bFailOnMissingSubsystem = bFail;
}

bool CParameterMgr::getFailureOnMissingSubsystem() const
{
    return _bFailOnMissingSubsystem;
}

void CParameterMgr::setFailureOnFailedSettingsLoad(bool bFail)
{
    _bFailOnFailedSettingsLoad = bFail;
}

bool CParameterMgr::getFailureOnFailedSettingsLoad() const
{
    return _bFailOnFailedSettingsLoad;
}

const string &CParameterMgr::getSchemaUri() const
{
    return _schemaUri;
}

void CParameterMgr::setSchemaUri(const string &schemaUri)
{
    _schemaUri = schemaUri;
}

void CParameterMgr::setValidateSchemasOnStart(bool bValidate)
{
    _bValidateSchemasOnStart = bValidate;
}

bool CParameterMgr::getValidateSchemasOnStart() const
{
    return _bValidateSchemasOnStart;
}

/////////////////// Remote command parsers
/// Version
CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::versionCommandProcess(
    const IRemoteCommand & /*command*/, string &strResult)
{
    // Show version
    strResult = getVersion();

    return CCommandHandler::ESucceeded;
}

/// Status
CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::statusCommandProcess(
    const IRemoteCommand & /*command*/, string &strResult)
{
    // System class
    const CSystemClass *pSystemClass = getSystemClass();

    // Show status
    /// General section
    utility::appendTitle(strResult, "General:");
    // System class
    strResult += "System Class: ";
    strResult += pSystemClass->getName();
    strResult += "\n";

    // Tuning mode
    strResult += "Tuning Mode: ";
    strResult += tuningModeOn() ? "on" : "off";
    strResult += "\n";

    // Value space
    strResult += "Value Space: ";
    strResult += valueSpaceIsRaw() ? "raw" : "real";
    strResult += "\n";

    // Output raw format
    strResult += "Output Raw Format: ";
    strResult += outputRawFormatIsHex() ? "hex" : "dec";
    strResult += "\n";

    // Auto Sync
    strResult += "Auto Sync: ";
    strResult += autoSyncOn() ? "on" : "off";
    strResult += "\n";

    /// Subsystem list
    utility::appendTitle(strResult, "Subsystems:");
    string strSubsystemList;
    pSystemClass->listChildrenPaths(strSubsystemList);
    strResult += strSubsystemList;

    /// Last applied configurations
    utility::appendTitle(strResult, "Last Applied [Pending] Configurations:");
    string strLastAppliedConfigurations;
    getConfigurableDomains()->listLastAppliedConfigurations(strLastAppliedConfigurations);
    strResult += strLastAppliedConfigurations;

    /// Criteria states
    utility::appendTitle(strResult, "Selection Criteria:");
    list<string> lstrSelectionCriteria;
    getSelectionCriteria()->listSelectionCriteria(lstrSelectionCriteria, false, true);
    // Concatenate the criterion list as the command result
    strResult += utility::asString(lstrSelectionCriteria);

    return CCommandHandler::ESucceeded;
}

/// Tuning Mode
CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::setTuningModeCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    if (remoteCommand.getArgument(0) == "on") {

        if (setTuningMode(true, strResult)) {

            return CCommandHandler::EDone;
        }
    } else if (remoteCommand.getArgument(0) == "off") {

        if (setTuningMode(false, strResult)) {

            return CCommandHandler::EDone;
        }
    } else {
        // Show usage
        return CCommandHandler::EShowUsage;
    }
    return CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getTuningModeCommandProcess(
    const IRemoteCommand & /*command*/, string &strResult)
{
    strResult = tuningModeOn() ? "on" : "off";

    return CCommandHandler::ESucceeded;
}

/// Value Space
CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::setValueSpaceCommandProcess(
    const IRemoteCommand &remoteCommand, string & /*strResult*/)
{
    if (remoteCommand.getArgument(0) == "raw") {

        setValueSpace(true);

        return CCommandHandler::EDone;

    } else if (remoteCommand.getArgument(0) == "real") {

        setValueSpace(false);

        return CCommandHandler::EDone;

    } else {
        // Show usage
        return CCommandHandler::EShowUsage;
    }
    return CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getValueSpaceCommandProcess(
    const IRemoteCommand & /*command*/, string &strResult)
{
    strResult = valueSpaceIsRaw() ? "raw" : "real";

    return CCommandHandler::ESucceeded;
}

/// Output Raw Format
CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::setOutputRawFormatCommandProcess(
    const IRemoteCommand &remoteCommand, string & /*strResult*/)
{
    if (remoteCommand.getArgument(0) == "hex") {

        setOutputRawFormat(true);

        return CCommandHandler::EDone;

    } else if (remoteCommand.getArgument(0) == "dec") {

        setOutputRawFormat(false);

        return CCommandHandler::EDone;

    } else {
        // Show usage
        return CCommandHandler::EShowUsage;
    }
    return CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getOutputRawFormatCommandProcess(
    const IRemoteCommand & /*command*/, string &strResult)
{
    strResult = outputRawFormatIsHex() ? "hex" : "dec";

    return CCommandHandler::ESucceeded;
}

/// Sync
CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::setAutoSyncCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    if (remoteCommand.getArgument(0) == "on") {

        if (setAutoSync(true, strResult)) {

            return CCommandHandler::EDone;
        }
    } else if (remoteCommand.getArgument(0) == "off") {

        if (setAutoSync(false, strResult)) {

            return CCommandHandler::EDone;
        }
    } else {
        // Show usage
        return CCommandHandler::EShowUsage;
    }
    return CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getAutoSyncCommandProcess(
    const IRemoteCommand & /*command*/, string &strResult)
{
    strResult = autoSyncOn() ? "on" : "off";

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::syncCommandProcess(
    const IRemoteCommand &, string &strResult)
{
    return sync(strResult) ? CCommandHandler::EDone : CCommandHandler::EFailed;
}

/// Criteria
CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::listCriteriaCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    if (remoteCommand.getArgumentCount() > 1) {

        return CCommandHandler::EShowUsage;
    }

    string strOutputFormat;

    // Look for optional arguments
    if (remoteCommand.getArgumentCount() == 1) {

        // Get requested format
        strOutputFormat = remoteCommand.getArgument(0);

        // Capitalize
        std::transform(strOutputFormat.begin(), strOutputFormat.end(), strOutputFormat.begin(),
                       ::toupper);

        if (strOutputFormat != "XML" && strOutputFormat != "CSV") {

            return CCommandHandler::EShowUsage;
        }
    }

    if (strOutputFormat == "XML") {
        // Get Root element where to export from
        const CSelectionCriteriaDefinition *pSelectionCriteriaDefinition =
            getConstSelectionCriteria()->getSelectionCriteriaDefinition();

        if (!exportElementToXMLString(pSelectionCriteriaDefinition, "SelectionCriteria",
                                      CXmlSerializingContext{strResult}, strResult)) {

            return CCommandHandler::EFailed;
        }

        // Succeeded
        return CCommandHandler::ESucceeded;
    } else {

        // Requested format will be either CSV or human readable based on strOutputFormat content
        bool bHumanReadable = strOutputFormat.empty();

        list<string> lstrResult;
        getSelectionCriteria()->listSelectionCriteria(lstrResult, true, bHumanReadable);

        // Concatenate the criterion list as the command result
        strResult += utility::asString(lstrResult);

        return CCommandHandler::ESucceeded;
    }
}

/// Domains
CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::listDomainsCommandProcess(
    const IRemoteCommand & /*command*/, string &strResult)
{
    getConfigurableDomains()->listDomains(strResult);

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::createDomainCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return createDomain(remoteCommand.getArgument(0), strResult) ? CCommandHandler::EDone
                                                                 : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::deleteDomainCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return deleteDomain(remoteCommand.getArgument(0), strResult) ? CCommandHandler::EDone
                                                                 : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::deleteAllDomainsCommandProcess(
    const IRemoteCommand & /*command*/, string &strResult)
{
    return deleteAllDomains(strResult) ? CCommandHandler::EDone : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::renameDomainCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return renameDomain(remoteCommand.getArgument(0), remoteCommand.getArgument(1), strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::setSequenceAwarenessCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    // Set property
    bool bSequenceAware;

    if (remoteCommand.getArgument(1) == "true") {

        bSequenceAware = true;

    } else if (remoteCommand.getArgument(1) == "false") {

        bSequenceAware = false;

    } else {
        // Show usage
        return CCommandHandler::EShowUsage;
    }

    return setSequenceAwareness(remoteCommand.getArgument(0), bSequenceAware, strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getSequenceAwarenessCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    // Get property
    bool bSequenceAware;

    if (!getSequenceAwareness(remoteCommand.getArgument(0), bSequenceAware, strResult)) {

        return CCommandHandler::EFailed;
    }

    strResult = bSequenceAware ? "true" : "false";

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::listDomainElementsCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return getConfigurableDomains()->listDomainElements(remoteCommand.getArgument(0), strResult)
               ? CCommandHandler::ESucceeded
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::addElementCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return addConfigurableElementToDomain(remoteCommand.getArgument(0),
                                          remoteCommand.getArgument(1), strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::removeElementCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return removeConfigurableElementFromDomain(remoteCommand.getArgument(0),
                                               remoteCommand.getArgument(1), strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::splitDomainCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return split(remoteCommand.getArgument(0), remoteCommand.getArgument(1), strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

/// Configurations
CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::listConfigurationsCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return getConstConfigurableDomains()->listConfigurations(remoteCommand.getArgument(0),
                                                             strResult)
               ? CCommandHandler::ESucceeded
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::dumpDomainsCommandProcess(
    const IRemoteCommand & /*command*/, string &strResult)
{
    // Dummy error context
    string strError;
    utility::ErrorContext errorContext(strError);

    // Dump
    strResult = getConstConfigurableDomains()->dumpContent(errorContext);

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::createConfigurationCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return createConfiguration(remoteCommand.getArgument(0), remoteCommand.getArgument(1),
                               strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::deleteConfigurationCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return deleteConfiguration(remoteCommand.getArgument(0), remoteCommand.getArgument(1),
                               strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::renameConfigurationCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return renameConfiguration(remoteCommand.getArgument(0), remoteCommand.getArgument(1),
                               remoteCommand.getArgument(2), strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::saveConfigurationCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return saveConfiguration(remoteCommand.getArgument(0), remoteCommand.getArgument(1), strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::restoreConfigurationCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    core::Results result;
    if (!restoreConfiguration(remoteCommand.getArgument(0), remoteCommand.getArgument(1), result)) {
        // Concatenate the error list as the command result
        strResult = utility::asString(result);

        return CCommandHandler::EFailed;
    }
    return CCommandHandler::EDone;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::setElementSequenceCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    // Build configurable element path list
    std::vector<string> astrNewElementSequence;

    for (size_t argument = 2; argument < remoteCommand.getArgumentCount(); argument++) {

        astrNewElementSequence.push_back(remoteCommand.getArgument(argument));
    }

    // Delegate to configurable domains
    return setElementSequence(remoteCommand.getArgument(0), remoteCommand.getArgument(1),
                              astrNewElementSequence, strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getElementSequenceCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    // Delegate to configurable domains
    return getConfigurableDomains()->getElementSequence(remoteCommand.getArgument(0),
                                                        remoteCommand.getArgument(1), strResult)
               ? CCommandHandler::ESucceeded
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::setRuleCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    // Delegate to configurable domains
    return setApplicationRule(remoteCommand.getArgument(0), remoteCommand.getArgument(1),
                              remoteCommand.packArguments(2, remoteCommand.getArgumentCount() - 2),
                              strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::clearRuleCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    // Delegate to configurable domains
    return clearApplicationRule(remoteCommand.getArgument(0), remoteCommand.getArgument(1),
                                strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getRuleCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    // Delegate to configurable domains
    return getApplicationRule(remoteCommand.getArgument(0), remoteCommand.getArgument(1), strResult)
               ? CCommandHandler::ESucceeded
               : CCommandHandler::EFailed;
}

/// Elements/Parameters
CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::listElementsCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    CElementLocator elementLocator(getSystemClass(), false);

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, strResult)) {

        return CCommandHandler::EFailed;
    }

    if (!pLocatedElement) {

        // List from root folder

        // Return system class qualified name
        pLocatedElement = getSystemClass();
    }

    // Return sub-elements
    strResult += pLocatedElement->listQualifiedPaths(false);

    return CCommandHandler::ESucceeded;
}

/// Elements/Parameters
CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::listParametersCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    CElementLocator elementLocator(getSystemClass(), false);

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, strResult)) {

        return CCommandHandler::EFailed;
    }

    if (!pLocatedElement) {

        // List from root folder

        // Return system class qualified name
        pLocatedElement = getSystemClass();
    }

    // Return sub-elements
    strResult += pLocatedElement->listQualifiedPaths(true);

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getElementStructureXMLCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    CElementLocator elementLocator(getSystemClass());

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, strResult)) {

        return CCommandHandler::EFailed;
    }

    // Use default access context for structure export
    CParameterAccessContext accessContext(strResult);
    if (!exportElementToXMLString(pLocatedElement, pLocatedElement->getXmlElementName(),
                                  CXmlParameterSerializingContext{accessContext, strResult},
                                  strResult)) {

        return CCommandHandler::EFailed;
    }

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getElementBytesCommandProcess(
    const IRemoteCommand &remoteCommand, std::string &strResult)
{
    CElementLocator elementLocator(getSystemClass());

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, strResult)) {

        return CCommandHandler::EFailed;
    }

    const CConfigurableElement *pConfigurableElement =
        static_cast<CConfigurableElement *>(pLocatedElement);

    // Get the settings
    vector<uint8_t> bytes;
    getSettingsAsBytes(*pConfigurableElement, bytes);

    // Hexa formatting
    std::ostringstream ostream;
    ostream << std::hex << std::setfill('0');

    // Format bytes
    for (auto byte : bytes) {

        // Convert to an int in order to avoid the "char" overload that would
        // print characters instead of numbers.
        ostream << "0x" << std::setw(2) << int{byte} << " ";
    }

    strResult = ostream.str();
    if (not strResult.empty()) {
        // Remove the trailing space
        strResult.pop_back();
    }

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::setElementBytesCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    // Check tuning mode
    if (!checkTuningModeOn(strResult)) {

        return CCommandHandler::EFailed;
    }

    // Retrieve configurable element
    CElementLocator elementLocator(getSystemClass());

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, strResult)) {

        return CCommandHandler::EFailed;
    }

    const CConfigurableElement *pConfigurableElement =
        static_cast<CConfigurableElement *>(pLocatedElement);

    // Convert input data to binary
    vector<uint8_t> bytes;

    auto first = remoteCommand.getArguments().cbegin() + 1;
    auto last = remoteCommand.getArguments().cend();

    try {
        std::transform(first, last, begin(bytes), [](decltype(*first) input) {
            uint8_t byte;

            if (!convertTo(input, byte)) {
                throw std::domain_error("Some values out of byte range");
            }

            return byte;
        });
    } catch (const std::domain_error &e) {
        strResult = e.what();

        return CCommandHandler::EFailed;
    }

    // Set the settings
    if (!setSettingsAsBytes(*pConfigurableElement, bytes, strResult)) {

        return CCommandHandler::EFailed;
    }

    return CCommandHandler::EDone;
}

bool CParameterMgr::getSettingsAsXML(const CConfigurableElement *configurableElement,
                                     string &result) const
{
    string error;
    CConfigurationAccessContext configContext(error, _pMainParameterBlackboard, _bValueSpaceIsRaw,
                                              _bOutputRawFormatIsHex, true);

    CXmlParameterSerializingContext xmlParameterContext(configContext, error);

    // Use a doc source by loading data from instantiated Configurable Domains
    CXmlMemoryDocSource memorySource(configurableElement, false,
                                     configurableElement->getXmlElementName());

    // Use a doc sink that write the doc data in a string
    ostringstream output;
    CXmlStreamDocSink streamSink(output);

    if (not streamSink.process(memorySource, xmlParameterContext)) {
        result = error;
        return false;
    }
    result = output.str();
    return true;
}

bool CParameterMgr::setSettingsAsXML(CConfigurableElement *configurableElement,
                                     const string &settings, string &error)
{
    CConfigurationAccessContext configContext(error, _pMainParameterBlackboard, _bValueSpaceIsRaw,
                                              _bOutputRawFormatIsHex, false);

    CXmlParameterSerializingContext xmlParameterContext(configContext, error);

    // It doesn't make sense to resolve XIncludes on an imported file because
    // we can't reliably decide of a "base url"
    _xmlDoc *doc = CXmlDocSource::mkXmlDoc(settings, false, false, xmlParameterContext);
    if (doc == nullptr) {
        return false;
    }
    if (not xmlParse(xmlParameterContext, configurableElement, doc, "",
                     EParameterConfigurationLibrary, false)) {
        return false;
    }
    if (_bAutoSyncOn) {
        CSyncerSet syncerSet;
        static_cast<CConfigurableElement *>(configurableElement)->fillSyncerSet(syncerSet);
        core::Results errors;
        if (not syncerSet.sync(*_pMainParameterBlackboard, false, &errors)) {
            error = utility::asString(errors);

            return false;
        }
    }
    return true;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getElementXMLCommandProcess(
    const IRemoteCommand &remoteCommand, string &result)
{
    CElementLocator elementLocator(getSystemClass());

    CElement *locatedElement = nullptr;

    if (not elementLocator.locate(remoteCommand.getArgument(0), &locatedElement, result)) {

        return CCommandHandler::EFailed;
    }

    if (not getSettingsAsXML(static_cast<CConfigurableElement *>(locatedElement), result)) {
        return CCommandHandler::EFailed;
    }
    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::setElementXMLCommandProcess(
    const IRemoteCommand &remoteCommand, string &result)
{
    if (!checkTuningModeOn(result)) {

        return CCommandHandler::EFailed;
    }

    CElementLocator elementLocator(getSystemClass());

    CElement *locatedElement = nullptr;

    if (not elementLocator.locate(remoteCommand.getArgument(0), &locatedElement, result)) {

        return CCommandHandler::EFailed;
    }
    if (not setSettingsAsXML(static_cast<CConfigurableElement *>(locatedElement),
                             remoteCommand.getArgument(1), result)) {
        return CCommandHandler::EFailed;
    }
    return CCommandHandler::EDone;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::dumpElementCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    CElementLocator elementLocator(getSystemClass());

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, strResult)) {

        return CCommandHandler::EFailed;
    }

    string strError;

    CParameterAccessContext parameterAccessContext(strError, _pMainParameterBlackboard,
                                                   _bValueSpaceIsRaw, _bOutputRawFormatIsHex);

    // Dump elements
    strResult = pLocatedElement->dumpContent(parameterAccessContext);

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getElementSizeCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    CElementLocator elementLocator(getSystemClass());

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, strResult)) {

        return CCommandHandler::EFailed;
    }

    // Converted to actual sizable element
    const CConfigurableElement *pConfigurableElement =
        static_cast<const CConfigurableElement *>(pLocatedElement);

    // Get size as string
    strResult = pConfigurableElement->getFootprintAsString();

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::showPropertiesCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    CElementLocator elementLocator(getSystemClass());

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, strResult)) {

        return CCommandHandler::EFailed;
    }

    // Convert element
    const CConfigurableElement *pConfigurableElement =
        static_cast<const CConfigurableElement *>(pLocatedElement);

    // Return element properties
    pConfigurableElement->showProperties(strResult);

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getParameterCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    string strValue;

    if (!accessParameterValue(remoteCommand.getArgument(0), strValue, false, strResult)) {

        return CCommandHandler::EFailed;
    }
    // Succeeded
    strResult = strValue;

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::setParameterCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    // Get value to set
    string strValue = remoteCommand.packArguments(1, remoteCommand.getArgumentCount() - 1);

    return accessParameterValue(remoteCommand.getArgument(0), strValue, true, strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::listBelongingDomainsCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    CElementLocator elementLocator(getSystemClass());

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, strResult)) {

        return CCommandHandler::EFailed;
    }

    // Convert element
    const CConfigurableElement *pConfigurableElement =
        static_cast<const CConfigurableElement *>(pLocatedElement);

    // Return element belonging domains
    pConfigurableElement->listBelongingDomains(strResult);

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::listAssociatedDomainsCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    CElementLocator elementLocator(getSystemClass());

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(remoteCommand.getArgument(0), &pLocatedElement, strResult)) {

        return CCommandHandler::EFailed;
    }

    // Convert element
    const CConfigurableElement *pConfigurableElement =
        static_cast<const CConfigurableElement *>(pLocatedElement);

    // Return element belonging domains
    pConfigurableElement->listAssociatedDomains(strResult);

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::listAssociatedElementsCommandProcess(
    const IRemoteCommand & /*command*/, string &strResult)
{
    getConfigurableDomains()->listAssociatedElements(strResult);

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::listConflictingElementsCommandProcess(
    const IRemoteCommand & /*command*/, string &strResult)
{
    getConfigurableDomains()->listConflictingElements(strResult);

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::listRogueElementsCommandProcess(
    const IRemoteCommand & /*command*/, string &strResult)
{
    getSystemClass()->listRogueElements(strResult);

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::
    getConfigurationParameterCommandProcess(const IRemoteCommand &remoteCommand, string &strResult)
{
    string strOutputValue;
    string strError;

    if (!accessConfigurationValue(remoteCommand.getArgument(0), remoteCommand.getArgument(1),
                                  remoteCommand.getArgument(2), strOutputValue, false, strError)) {

        strResult = strError;
        return CCommandHandler::EFailed;
    }
    // Succeeded
    strResult = strOutputValue;

    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::
    setConfigurationParameterCommandProcess(const IRemoteCommand &remoteCommand, string &strResult)
{
    // Get value to set
    string strValue = remoteCommand.packArguments(3, remoteCommand.getArgumentCount() - 3);

    bool bSuccess =
        accessConfigurationValue(remoteCommand.getArgument(0), remoteCommand.getArgument(1),
                                 remoteCommand.getArgument(2), strValue, true, strResult);

    return bSuccess ? CCommandHandler::EDone : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::showMappingCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    if (!getParameterMapping(remoteCommand.getArgument(0), strResult)) {

        return CCommandHandler::EFailed;
    }

    return CCommandHandler::ESucceeded;
}

/// Settings Import/Export
CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::exportDomainsXMLCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    string strFileName = remoteCommand.getArgument(0);
    return exportDomainsXml(strFileName, false, true, strResult) ? CCommandHandler::EDone
                                                                 : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::importDomainsXMLCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    return importDomainsXml(remoteCommand.getArgument(0), false, true, strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::
    exportDomainsWithSettingsXMLCommandProcess(const IRemoteCommand &remoteCommand,
                                               string &strResult)
{
    string strFileName = remoteCommand.getArgument(0);
    return exportDomainsXml(strFileName, true, true, strResult) ? CCommandHandler::EDone
                                                                : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::
    exportDomainWithSettingsXMLCommandProcess(const IRemoteCommand &remoteCommand, string &result)
{
    string domainName = remoteCommand.getArgument(0);
    string fileName = remoteCommand.getArgument(1);
    return exportSingleDomainXml(fileName, domainName, true, true, result)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::
    importDomainsWithSettingsXMLCommandProcess(const IRemoteCommand &remoteCommand,
                                               string &strResult)
{
    return importDomainsXml(remoteCommand.getArgument(0), true, true, strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::
    importDomainWithSettingsXMLCommandProcess(const IRemoteCommand &remoteCommand,
                                              string &strResult)
{
    bool bOverwrite = false;

    // Look for optional arguments
    if (remoteCommand.getArgumentCount() > 1) {

        if (remoteCommand.getArgument(1) == "overwrite") {

            bOverwrite = true;
        } else {
            // Show usage
            return CCommandHandler::EShowUsage;
        }
    }

    return importSingleDomainXml(remoteCommand.getArgument(0), bOverwrite, true, true, strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::
    getDomainsWithSettingsXMLCommandProcess(const IRemoteCommand & /*command*/, string &strResult)
{
    if (!exportDomainsXml(strResult, true, false, strResult)) {

        return CCommandHandler::EFailed;
    }
    // Succeeded
    return CCommandHandler::ESucceeded;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getDomainWithSettingsXMLCommandProcess(
    const IRemoteCommand &remoteCommand, string &strResult)
{
    string strDomainName = remoteCommand.getArgument(0);

    return exportSingleDomainXml(strResult, strDomainName, true, false, strResult)
               ? CCommandHandler::ESucceeded
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::
    setDomainsWithSettingsXMLCommandProcess(const IRemoteCommand &remoteCommand, string &strResult)
{
    return importDomainsXml(remoteCommand.getArgument(0), true, false, strResult)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::setDomainWithSettingsXMLCommandProcess(
    const IRemoteCommand &remoteCommand, string &result)
{
    bool overwrite = false;

    if (remoteCommand.getArgumentCount() > 1) {

        if (remoteCommand.getArgument(1) == "overwrite") {

            overwrite = true;
        } else {
            // Show usage
            return CCommandHandler::EShowUsage;
        }
    }

    return importSingleDomainXml(remoteCommand.getArgument(0), overwrite, true, false, result)
               ? CCommandHandler::EDone
               : CCommandHandler::EFailed;
}

CParameterMgr::CCommandHandler::CommandStatus CParameterMgr::getSystemClassXMLCommandProcess(
    const IRemoteCommand & /*command*/, string &strResult)
{
    // Get Root element where to export from
    const CSystemClass *pSystemClass = getSystemClass();

    // Use default access context for structure export
    CParameterAccessContext accessContext(strResult);
    if (!exportElementToXMLString(pSystemClass, pSystemClass->getXmlElementName(),
                                  CXmlParameterSerializingContext{accessContext, strResult},
                                  strResult)) {
        return CCommandHandler::EFailed;
    }
    // Succeeded
    return CCommandHandler::ESucceeded;
}

// User set/get parameters in main BlackBoard
bool CParameterMgr::accessParameterValue(const string &strPath, string &strValue, bool bSet,
                                         string &strError)
{
    // Forbid write access when not in TuningMode
    if (bSet && !checkTuningModeOn(strError)) {

        return false;
    }

    // Define context
    CParameterAccessContext parameterAccessContext(strError, _pMainParameterBlackboard,
                                                   _bValueSpaceIsRaw, _bOutputRawFormatIsHex);

    // Activate the auto synchronization with the hardware
    if (bSet) {

        parameterAccessContext.setAutoSync(_bAutoSyncOn);
    }

    return accessValue(parameterAccessContext, strPath, strValue, bSet, strError);
}

// User get parameter mapping
bool CParameterMgr::getParameterMapping(const string &strPath, string &strResult) const
{
    // Get the ConfigurableElement corresponding to strPath
    const CConfigurableElement *pConfigurableElement = getConfigurableElement(strPath, strResult);
    if (!pConfigurableElement) {

        return false;
    }

    // Find the list of the ancestors of the current ConfigurableElement that have a mapping
    auto configurableElementPath = pConfigurableElement->getConfigurableElementContext();

    // Get the Subsystem containing the ConfigurableElement
    const CSubsystem *pSubsystem = pConfigurableElement->getBelongingSubsystem();
    if (!pSubsystem) {

        strResult = "Unable to find the Subsystem containing the parameter";
        return false;
    }

    // Fetch the mapping corresponding to the ConfigurableElement
    strResult = pSubsystem->getMapping(configurableElementPath);

    return true;
}

// User set/get parameters in specific Configuration BlackBoard
bool CParameterMgr::accessConfigurationValue(const string &strDomain,
                                             const string &strConfiguration, const string &strPath,
                                             string &strValue, bool bSet, string &strError)
{
    CElementLocator elementLocator(getSystemClass());

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(strPath, &pLocatedElement, strError)) {

        return false;
    }

    // Convert element
    const CConfigurableElement *pConfigurableElement =
        static_cast<const CConfigurableElement *>(pLocatedElement);

    // Get the Configuration blackboard and the Base Offset of the configurable element in this
    // blackboard
    size_t baseOffset;
    bool bIsLastApplied;

    CParameterBlackboard *pConfigurationBlackboard = NULL;

    {
        pConfigurationBlackboard = getConstConfigurableDomains()->findConfigurationBlackboard(
            strDomain, strConfiguration, pConfigurableElement, baseOffset, bIsLastApplied,
            strError);
        if (!pConfigurationBlackboard) {

            warning() << "Fail: " << strError;
            return false;
        }
    }

    info() << "Element " << strPath << " in Domain " << strDomain
           << ", offset: " << pConfigurableElement->getOffset() << ", base offset: " << baseOffset;

    /// Update the Configuration Blackboard

    // Define Configuration context using Base Offset and keep Auto Sync off to prevent access to HW
    CParameterAccessContext parameterAccessContext(
        strError, pConfigurationBlackboard, _bValueSpaceIsRaw, _bOutputRawFormatIsHex, baseOffset);

    // Deactivate the auto synchronization with the hardware during the Configuration Blackboard
    // access (only Main Blackboard shall be synchronized, Configurations Blackboards are copied
    // into the Main Blackboard each time a configuration is restored but they are not synchronized
    // directly).
    if (bSet) {

        parameterAccessContext.setAutoSync(false);
    }

    // Access Value in the Configuration Blackboard
    if (!accessValue(parameterAccessContext, strPath, strValue, bSet, strError)) {

        return false;
    }

    /// If the Configuration is the last one applied, update the Main Blackboard as well

    if (bIsLastApplied) {

        // Define Main context
        parameterAccessContext.setParameterBlackboard(_pMainParameterBlackboard);

        // Activate the auto synchronization with the hardware
        if (bSet) {

            parameterAccessContext.setAutoSync(_bAutoSyncOn);
        }

        // Access Value in the Main Blackboard
        return accessValue(parameterAccessContext, strPath, strValue, bSet, strError);
    }

    return true;
}

// User set/get parameters
bool CParameterMgr::accessValue(CParameterAccessContext &parameterAccessContext,
                                const string &strPath, string &strValue, bool bSet,
                                string &strError)
{
    // Lock state
    lock_guard<mutex> autoLock(getBlackboardMutex());

    CPathNavigator pathNavigator(strPath);

    // Nagivate through system class
    if (!pathNavigator.navigateThrough(getConstSystemClass()->getName(), strError)) {

        parameterAccessContext.setError(strError);

        return false;
    }

    // Do the get
    return getConstSystemClass()->accessValue(pathNavigator, strValue, bSet,
                                              parameterAccessContext);
}

// Tuning mode
bool CParameterMgr::setTuningMode(bool bOn, string &strError)
{
    if (bOn == tuningModeOn()) {
        strError = "Tuning mode is already in the state requested";
        return false;
    }
    // Tuning allowed?
    if (bOn && !getConstFrameworkConfiguration()->isTuningAllowed()) {

        strError = "Tuning prohibited";

        return false;
    }
    // Lock state
    lock_guard<mutex> autoLock(getBlackboardMutex());

    // Warn domains about exiting tuning mode
    if (!bOn) {

        // Ensure application of currently selected configurations
        // Force-apply configurations
        doApplyConfigurations(true);
    }

    // Store
    _bTuningModeIsOn = bOn;

    return true;
}

bool CParameterMgr::tuningModeOn() const
{
    return _bTuningModeIsOn;
}

// Current value space for user set/get value interpretation
void CParameterMgr::setValueSpace(bool bIsRaw)
{
    _bValueSpaceIsRaw = bIsRaw;
}

bool CParameterMgr::valueSpaceIsRaw()
{
    return _bValueSpaceIsRaw;
}

// Current Output Raw Format for user get value interpretation
void CParameterMgr::setOutputRawFormat(bool bIsHex)
{
    _bOutputRawFormatIsHex = bIsHex;
}

bool CParameterMgr::outputRawFormatIsHex()
{
    return _bOutputRawFormatIsHex;
}

/// Sync
// Automatic hardware synchronization control (during tuning session)
bool CParameterMgr::setAutoSync(bool bAutoSyncOn, string &strError)
{
    // Warn domains about turning auto sync back on
    if (bAutoSyncOn && !_bAutoSyncOn) {

        // Do the synchronization at system class level (could be optimized by keeping track of all
        // modified parameters)
        if (!sync(strError)) {

            return false;
        }
    }

    // Set Auto sync
    _bAutoSyncOn = bAutoSyncOn;

    return true;
}

bool CParameterMgr::autoSyncOn() const
{
    return _bAutoSyncOn;
}

// Manual hardware synchronization control (during tuning session)
bool CParameterMgr::sync(string &strError)
{
    // Warn domains about turning auto sync back on
    if (_bAutoSyncOn) {

        strError = "Feature unavailable when Auto Sync is on";

        return false;
    }

    // Get syncer set
    CSyncerSet syncerSet;
    // ... from system class
    getConstSystemClass()->fillSyncerSet(syncerSet);

    // Sync
    core::Results error;
    if (!syncerSet.sync(*_pMainParameterBlackboard, false, &error)) {

        strError = utility::asString(error);
        return false;
    };

    return true;
}

// Configuration/Domains handling
bool CParameterMgr::createDomain(const string &strName, string &strError)
{
    LOG_CONTEXT("Creating configurable domain " + strName);
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        return false;
    }

    // Delegate to configurable domains
    return logResult(getConfigurableDomains()->createDomain(strName, strError), strError);
}

bool CParameterMgr::deleteDomain(const string &strName, string &strError)
{
    LOG_CONTEXT("Deleting configurable domain '" + strName + "'");

    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail: " << strError;
        return false;
    }

    // Delegate to configurable domains
    return logResult(getConfigurableDomains()->deleteDomain(strName, strError), strError);
}

bool CParameterMgr::renameDomain(const string &strName, const string &strNewName, string &strError)
{
    LOG_CONTEXT("Renaming configurable domain '" + strName + "' to '" + strNewName + "'");

    // Delegate to configurable domains
    return logResult(getConfigurableDomains()->renameDomain(strName, strNewName, strError),
                     strError);
}

bool CParameterMgr::deleteAllDomains(string &strError)
{
    LOG_CONTEXT("Deleting all configurable domains");

    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail: " << strError;
        return false;
    }

    // Delegate to configurable domains
    getConfigurableDomains()->deleteAllDomains();

    info() << "Success";
    return true;
}

bool CParameterMgr::setSequenceAwareness(const string &strName, bool bSequenceAware,
                                         string &strResult)
{
    LOG_CONTEXT("Making domain '" + strName + "' sequence " +
                (bSequenceAware ? "aware" : "unaware"));
    // Check tuning mode
    if (!checkTuningModeOn(strResult)) {

        warning() << "Fail: " << strResult;
        return false;
    }

    return logResult(
        getConfigurableDomains()->setSequenceAwareness(strName, bSequenceAware, strResult),
        strResult);
}

bool CParameterMgr::getSequenceAwareness(const string &strName, bool &bSequenceAware,
                                         string &strResult)
{
    return getConfigurableDomains()->getSequenceAwareness(strName, bSequenceAware, strResult);
}

bool CParameterMgr::createConfiguration(const string &strDomain, const string &strConfiguration,
                                        string &strError)
{
    LOG_CONTEXT("Creating domain configuration '" + strConfiguration + "' into domain '" +
                strDomain + "'");
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail: " << strError;
        return false;
    }

    // Delegate to configurable domains
    return logResult(getConfigurableDomains()->createConfiguration(
                         strDomain, strConfiguration, _pMainParameterBlackboard, strError),
                     strError);
}
bool CParameterMgr::renameConfiguration(const string &strDomain, const string &strConfiguration,
                                        const string &strNewConfiguration, string &strError)
{
    LOG_CONTEXT("Renaming domain '" + strDomain + "''s configuration '" + strConfiguration +
                "' to '" + strNewConfiguration + "'");

    return logResult(getConfigurableDomains()->renameConfiguration(strDomain, strConfiguration,
                                                                   strNewConfiguration, strError),
                     strError);
}

bool CParameterMgr::deleteConfiguration(const string &strDomain, const string &strConfiguration,
                                        string &strError)
{
    LOG_CONTEXT("Deleting configuration '" + strConfiguration + "' from domain '" + strDomain +
                "'");

    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail:" << strError;
        return false;
    }

    // Delegate to configurable domains
    return logResult(
        getConfigurableDomains()->deleteConfiguration(strDomain, strConfiguration, strError),
        strError);
}

bool CParameterMgr::restoreConfiguration(const string &strDomain, const string &strConfiguration,
                                         core::Results &errors)
{
    string strError;
    LOG_CONTEXT("Restoring domain '" + strDomain + "''s configuration '" + strConfiguration +
                "' to parameter blackboard");
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        errors.push_back(strError);
        warning() << "Fail:" << strError;
        return false;
    }

    // Delegate to configurable domains
    return logResult(
        getConstConfigurableDomains()->restoreConfiguration(
            strDomain, strConfiguration, _pMainParameterBlackboard, _bAutoSyncOn, errors),
        strError);
}

bool CParameterMgr::saveConfiguration(const string &strDomain, const string &strConfiguration,
                                      string &strError)
{
    LOG_CONTEXT("Saving domain '" + strDomain + "' configuration '" + strConfiguration +
                "' from parameter blackboard");
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail:" << strError;
        return false;
    }

    // Delegate to configurable domains
    return logResult(getConfigurableDomains()->saveConfiguration(
                         strDomain, strConfiguration, _pMainParameterBlackboard, strError),
                     strError);
}

// Configurable element - domain association
bool CParameterMgr::addConfigurableElementToDomain(const string &strDomain,
                                                   const string &strConfigurableElementPath,
                                                   string &strError)
{
    LOG_CONTEXT("Adding configurable element '" + strConfigurableElementPath + "' to domain '" +
                strDomain + "'");
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail: " << strError;
        return false;
    }

    CElementLocator elementLocator(getSystemClass());

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(strConfigurableElementPath, &pLocatedElement, strError)) {

        warning() << "Fail: " << strError;
        return false;
    }

    // Convert element
    CConfigurableElement *pConfigurableElement =
        static_cast<CConfigurableElement *>(pLocatedElement);

    // Delegate
    core::Results infos;
    bool isSuccess = getConfigurableDomains()->addConfigurableElementToDomain(
        strDomain, pConfigurableElement, _pMainParameterBlackboard, infos);

    if (isSuccess) {
        info() << infos;
    } else {
        warning() << infos;
    }

    strError = utility::asString(infos);
    return isSuccess;
}

bool CParameterMgr::removeConfigurableElementFromDomain(const string &strDomain,
                                                        const string &strConfigurableElementPath,
                                                        string &strError)
{
    LOG_CONTEXT("Removing configurable element '" + strConfigurableElementPath + "' from domain '" +
                strDomain + "'");

    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail:" << strError;
        return false;
    }

    CElementLocator elementLocator(getSystemClass());

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(strConfigurableElementPath, &pLocatedElement, strError)) {

        warning() << "Fail:" << strError;
        return false;
    }

    // Convert element
    CConfigurableElement *pConfigurableElement =
        static_cast<CConfigurableElement *>(pLocatedElement);

    // Delegate
    return logResult(getConfigurableDomains()->removeConfigurableElementFromDomain(
                         strDomain, pConfigurableElement, strError),
                     strError);
}

bool CParameterMgr::split(const string &strDomain, const string &strConfigurableElementPath,
                          string &strError)
{
    LOG_CONTEXT("Splitting configurable element '" + strConfigurableElementPath + "' domain '" +
                strDomain + "'");
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail:" << strError;
        return false;
    }

    CElementLocator elementLocator(getSystemClass());

    CElement *pLocatedElement = NULL;

    if (!elementLocator.locate(strConfigurableElementPath, &pLocatedElement, strError)) {

        warning() << "Fail: " << strError;
        return false;
    }

    // Convert element
    CConfigurableElement *pConfigurableElement =
        static_cast<CConfigurableElement *>(pLocatedElement);

    // Delegate
    core::Results infos;
    bool isSuccess = getConfigurableDomains()->split(strDomain, pConfigurableElement, infos);

    if (isSuccess) {
        info() << infos;
    } else {
        warning() << infos;
    }

    strError = utility::asString(infos);
    return isSuccess;
}

bool CParameterMgr::setElementSequence(const string &strDomain, const string &strConfiguration,
                                       const std::vector<string> &astrNewElementSequence,
                                       string &strError)
{
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        return false;
    }

    return getConfigurableDomains()->setElementSequence(strDomain, strConfiguration,
                                                        astrNewElementSequence, strError);
}

bool CParameterMgr::getApplicationRule(const string &strDomain, const string &strConfiguration,
                                       string &strResult)
{
    return getConfigurableDomains()->getApplicationRule(strDomain, strConfiguration, strResult);
}

bool CParameterMgr::setApplicationRule(const string &strDomain, const string &strConfiguration,
                                       const string &strApplicationRule, string &strError)
{
    return getConfigurableDomains()->setApplicationRule(
        strDomain, strConfiguration, strApplicationRule,
        getConstSelectionCriteria()->getSelectionCriteriaDefinition(), strError);
}

bool CParameterMgr::clearApplicationRule(const string &strDomain, const string &strConfiguration,
                                         string &strError)
{
    return getConfigurableDomains()->clearApplicationRule(strDomain, strConfiguration, strError);
}

bool CParameterMgr::importDomainsXml(const string &xmlSource, bool withSettings, bool fromFile,
                                     string &errorMsg)
{
    // Check tuning mode
    if (!checkTuningModeOn(errorMsg)) {

        return false;
    }

    LOG_CONTEXT("Importing domains from " +
                (fromFile ? ("\"" + xmlSource + "\"") : "a user-provided buffer"));

    // Root element
    CConfigurableDomains *pConfigurableDomains = getConfigurableDomains();

    bool importSuccess = wrapLegacyXmlImport(xmlSource, fromFile, withSettings,
                                             *pConfigurableDomains, "SystemClassName", errorMsg);

    if (importSuccess) {

        // Validate domains after XML import
        pConfigurableDomains->validate(_pMainParameterBlackboard);
    }

    return importSuccess;
}

bool CParameterMgr::importSingleDomainXml(const string &xmlSource, bool overwrite,
                                          bool withSettings, bool fromFile, string &errorMsg)
{
    if (!checkTuningModeOn(errorMsg)) {

        return false;
    }

    LOG_CONTEXT("Importing a single domain from " +
                (fromFile ? ('"' + xmlSource + '"') : "a user-provided buffer"));

    // We initialize the domain with an empty name but since we have set the isDomainStandalone
    // context, the name will be retrieved during de-serialization
    auto standaloneDomain = utility::make_unique<CConfigurableDomain>();

    if (!wrapLegacyXmlImport(xmlSource, fromFile, withSettings, *standaloneDomain, "", errorMsg)) {
        return false;
    }

    if (!getConfigurableDomains()->addDomain(*standaloneDomain, overwrite, errorMsg)) {
        return false;
    }

    // ownership has been transfered to the ConfigurableDomains object
    standaloneDomain.release();
    return true;
}

bool CParameterMgr::wrapLegacyXmlImport(const string &xmlSource, bool fromFile, bool withSettings,
                                        CElement &element, const string &nameAttributeName,
                                        string &errorMsg)
{
    CXmlDomainImportContext xmlDomainImportContext(errorMsg, withSettings, *getSystemClass());

    // Selection criteria definition for rule creation
    xmlDomainImportContext.setSelectionCriteriaDefinition(
        getConstSelectionCriteria()->getSelectionCriteriaDefinition());

    // It doesn't make sense to resolve XIncludes on an imported file because
    // we can't reliably decide of a "base url"
    _xmlDoc *doc = CXmlDocSource::mkXmlDoc(xmlSource, fromFile, false, xmlDomainImportContext);
    if (doc == NULL) {
        return false;
    }

    return xmlParse(xmlDomainImportContext, &element, doc, "", EParameterConfigurationLibrary, true,
                    nameAttributeName);
}

bool CParameterMgr::serializeElement(std::ostream &output,
                                     CXmlSerializingContext &xmlSerializingContext,
                                     const CElement &element) const
{
    if (!output.good()) {
        xmlSerializingContext.setError("Can't write XML: the output is in a bad state.");
        return false;
    }

    // Use a doc source by loading data from instantiated Configurable Domains
    CXmlMemoryDocSource memorySource(&element, _bValidateSchemasOnStart,
                                     element.getXmlElementName(), "parameter-framework",
                                     getVersion());

    // Use a doc sink to write the doc data in a stream
    CXmlStreamDocSink sink(output);

    bool processSuccess = sink.process(memorySource, xmlSerializingContext);

    return processSuccess;
}

bool CParameterMgr::exportDomainsXml(string &xmlDest, bool withSettings, bool toFile,
                                     string &errorMsg) const
{
    LOG_CONTEXT("Exporting domains to " +
                (toFile ? ('"' + xmlDest + '"') : "a user-provided buffer"));

    const CConfigurableDomains *configurableDomains = getConstConfigurableDomains();

    return wrapLegacyXmlExport(xmlDest, toFile, withSettings, *configurableDomains, errorMsg);
}

bool CParameterMgr::exportSingleDomainXml(string &xmlDest, const string &domainName,
                                          bool withSettings, bool toFile, string &errorMsg) const
{
    LOG_CONTEXT("Exporting single domain '" + domainName + "' to " +
                (toFile ? ('"' + xmlDest + '"') : "a user-provided buffer"));

    // Element to be serialized
    const CConfigurableDomain *requestedDomain =
        getConstConfigurableDomains()->findConfigurableDomain(domainName, errorMsg);

    if (requestedDomain == NULL) {
        return false;
    }

    return wrapLegacyXmlExport(xmlDest, toFile, withSettings, *requestedDomain, errorMsg);
}

bool CParameterMgr::wrapLegacyXmlExport(string &xmlDest, bool toFile, bool withSettings,
                                        const CElement &element, string &errorMsg) const
{
    CXmlDomainExportContext context(errorMsg, withSettings, _bValueSpaceIsRaw,
                                    _bOutputRawFormatIsHex);

    if (toFile) {
        return wrapLegacyXmlExportToFile(xmlDest, element, context);
    } else {
        return wrapLegacyXmlExportToString(xmlDest, element, context);
    }
}

bool CParameterMgr::wrapLegacyXmlExportToFile(string &xmlDest, const CElement &element,
                                              CXmlDomainExportContext &context) const
{
    try {
        std::ofstream output;
        // Force stream to throw instead of using fail/bad bit
        // in order to retreive an error message.
        output.exceptions(~std::ifstream::goodbit);

        output.open(xmlDest.c_str());
        bool status = serializeElement(output, context, element);
        output.close(); // Explicit close to detect errors

        return status;

    } catch (std::ofstream::failure &e) {
        context.setError("Failed to open \"" + xmlDest + "\" for writing: " + e.what());
        return false;
    }
}

bool CParameterMgr::wrapLegacyXmlExportToString(string &xmlDest, const CElement &element,
                                                CXmlDomainExportContext &context) const
{
    std::ostringstream output;

    if (!serializeElement(output, context, element)) {
        return false;
    }

    xmlDest = output.str();

    return true;
}

// For tuning, check we're in tuning mode
bool CParameterMgr::checkTuningModeOn(string &strError) const
{
    // Tuning Mode on?
    if (!_bTuningModeIsOn) {

        strError = "Tuning Mode must be on";

        return false;
    }
    return true;
}

// Tuning mutex dynamic parameter handling
std::mutex &CParameterMgr::getBlackboardMutex()
{
    return _blackboardMutex;
}

// Blackboard reference (dynamic parameter handling)
CParameterBlackboard *CParameterMgr::getParameterBlackboard()
{
    return _pMainParameterBlackboard;
}

// Dynamic creation library feeding
void CParameterMgr::feedElementLibraries()
{
    // Global Configuration handling
    CElementLibrary *pFrameworkConfigurationLibrary = new CElementLibrary;

    pFrameworkConfigurationLibrary->addElementBuilder(
        "ParameterFrameworkConfiguration",
        new TElementBuilderTemplate<CParameterFrameworkConfiguration>());
    pFrameworkConfigurationLibrary->addElementBuilder(
        "SubsystemPlugins", new TKindElementBuilderTemplate<CSubsystemPlugins>());
    pFrameworkConfigurationLibrary->addElementBuilder(
        "Location", new TKindElementBuilderTemplate<CPluginLocation>());
    pFrameworkConfigurationLibrary->addElementBuilder(
        "StructureDescriptionFileLocation",
        new TKindElementBuilderTemplate<CFrameworkConfigurationLocation>());
    pFrameworkConfigurationLibrary->addElementBuilder(
        "SettingsConfiguration", new TKindElementBuilderTemplate<CFrameworkConfigurationGroup>());
    pFrameworkConfigurationLibrary->addElementBuilder(
        "ConfigurableDomainsFileLocation",
        new TKindElementBuilderTemplate<CFrameworkConfigurationLocation>());

    _pElementLibrarySet->addElementLibrary(pFrameworkConfigurationLibrary);

    // Parameter creation
    CElementLibrary *pParameterCreationLibrary = new CElementLibrary;

    pParameterCreationLibrary->addElementBuilder(
        "Subsystem", new CSubsystemElementBuilder(getSystemClass()->getSubsystemLibrary()));
    pParameterCreationLibrary->addElementBuilder(
        "ComponentType", new TNamedElementBuilderTemplate<CComponentType>());
    pParameterCreationLibrary->addElementBuilder(
        "Component", new TNamedElementBuilderTemplate<CComponentInstance>());
    pParameterCreationLibrary->addElementBuilder(
        "BitParameter", new TNamedElementBuilderTemplate<CBitParameterType>());
    pParameterCreationLibrary->addElementBuilder(
        "BitParameterBlock", new TNamedElementBuilderTemplate<CBitParameterBlockType>());
    pParameterCreationLibrary->addElementBuilder(
        "StringParameter", new TNamedElementBuilderTemplate<CStringParameterType>());
    pParameterCreationLibrary->addElementBuilder(
        "ParameterBlock", new TNamedElementBuilderTemplate<CParameterBlockType>());
    pParameterCreationLibrary->addElementBuilder(
        "BooleanParameter", new TNamedElementBuilderTemplate<CBooleanParameterType>());
    pParameterCreationLibrary->addElementBuilder(
        "IntegerParameter", new TNamedElementBuilderTemplate<CIntegerParameterType>());
    pParameterCreationLibrary->addElementBuilder(
        "LinearAdaptation", new TElementBuilderTemplate<CLinearParameterAdaptation>());
    pParameterCreationLibrary->addElementBuilder(
        "LogarithmicAdaptation", new TElementBuilderTemplate<CLogarithmicParameterAdaptation>());
    pParameterCreationLibrary->addElementBuilder(
        "EnumParameter", new TNamedElementBuilderTemplate<CEnumParameterType>());
    pParameterCreationLibrary->addElementBuilder("ValuePair",
                                                 new TElementBuilderTemplate<CEnumValuePair>());
    pParameterCreationLibrary->addElementBuilder(
        "FixedPointParameter", new TNamedElementBuilderTemplate<CFixedPointParameterType>());
    pParameterCreationLibrary->addElementBuilder(
        "FloatingPointParameter", new TNamedElementBuilderTemplate<CFloatingPointParameterType>);
    pParameterCreationLibrary->addElementBuilder(
        "SubsystemInclude",
        new CFileIncluderElementBuilder(_bValidateSchemasOnStart, getSchemaUri()));

    _pElementLibrarySet->addElementLibrary(pParameterCreationLibrary);

    // Parameter Configuration Domains creation
    CElementLibrary *pParameterConfigurationLibrary = new CElementLibrary;

    pParameterConfigurationLibrary->addElementBuilder(
        "ConfigurableDomain", new TElementBuilderTemplate<CConfigurableDomain>());
    pParameterConfigurationLibrary->addElementBuilder(
        "Configuration", new TNamedElementBuilderTemplate<CDomainConfiguration>());
    pParameterConfigurationLibrary->addElementBuilder("CompoundRule",
                                                      new TElementBuilderTemplate<CCompoundRule>());
    pParameterConfigurationLibrary->addElementBuilder(
        "SelectionCriterionRule", new TElementBuilderTemplate<CSelectionCriterionRule>());

    _pElementLibrarySet->addElementLibrary(pParameterConfigurationLibrary);
}

bool CParameterMgr::getForceNoRemoteInterface() const
{
    return _bForceNoRemoteInterface;
}

void CParameterMgr::setForceNoRemoteInterface(bool bForceNoRemoteInterface)
{
    _bForceNoRemoteInterface = bForceNoRemoteInterface;
}

CParameterMgr::CommandHandler CParameterMgr::createCommandHandler()
{
    auto commandHandler = utility::make_unique<CCommandHandler>(this);

    // Add command parsers
    for (const auto &remoteCommandParserItem : gastRemoteCommandParserItems) {
        commandHandler->addCommandParser(
            remoteCommandParserItem._pcCommandName, remoteCommandParserItem._pfnParser,
            remoteCommandParserItem._minArgumentCount, remoteCommandParserItem._pcHelp,
            remoteCommandParserItem._pcDescription);
    }

    return commandHandler;
}

bool CParameterMgr::isRemoteInterfaceRequired()
{
    // The remote interface should only be started if the client didn't
    // explicitly forbid it and if the configuration file allows it.
    return (not _bForceNoRemoteInterface) and getConstFrameworkConfiguration()->isTuningAllowed();
}

// Remote Processor Server connection handling
bool CParameterMgr::handleRemoteProcessingInterface(string &strError)
{
    LOG_CONTEXT("Handling remote processing interface");

    if (not isRemoteInterfaceRequired()) {
        return true;
    }

    auto port = getConstFrameworkConfiguration()->getServerPort();

    try {
        // The ownership of remoteComandHandler is given to Bg remote processor server.
        _pRemoteProcessorServer = new BackgroundRemoteProcessorServer(port, createCommandHandler());
    } catch (std::runtime_error &e) {
        strError = string("ParameterMgr: Unable to create Remote Processor Server: ") + e.what();
        return false;
    }

    if (_pRemoteProcessorServer == NULL) {
        strError = "ParameterMgr: Unable to create Remote Processor Server";
        return false;
    }

    if (!_pRemoteProcessorServer->start(strError)) {
        ostringstream oss;
        oss << "ParameterMgr: Unable to start remote processor server on port " << port;
        strError = oss.str() + ": " + strError;
        return false;
    }
    info() << "Remote Processor Server started on port " << port;
    return true;
}

// Children typwise access
CParameterFrameworkConfiguration *CParameterMgr::getFrameworkConfiguration()
{
    return static_cast<CParameterFrameworkConfiguration *>(getChild(EFrameworkConfiguration));
}

const CParameterFrameworkConfiguration *CParameterMgr::getConstFrameworkConfiguration()
{
    return getFrameworkConfiguration();
}

CSelectionCriteria *CParameterMgr::getSelectionCriteria()
{
    return static_cast<CSelectionCriteria *>(getChild(ESelectionCriteria));
}

const CSelectionCriteria *CParameterMgr::getConstSelectionCriteria()
{
    return static_cast<const CSelectionCriteria *>(getChild(ESelectionCriteria));
}

CSystemClass *CParameterMgr::getSystemClass()
{
    return static_cast<CSystemClass *>(getChild(ESystemClass));
}

const CSystemClass *CParameterMgr::getConstSystemClass() const
{
    return static_cast<const CSystemClass *>(getChild(ESystemClass));
}

// Configurable Domains
CConfigurableDomains *CParameterMgr::getConfigurableDomains()
{
    return static_cast<CConfigurableDomains *>(getChild(EConfigurableDomains));
}

const CConfigurableDomains *CParameterMgr::getConstConfigurableDomains()
{
    return static_cast<const CConfigurableDomains *>(getChild(EConfigurableDomains));
}

const CConfigurableDomains *CParameterMgr::getConstConfigurableDomains() const
{
    return static_cast<const CConfigurableDomains *>(getChild(EConfigurableDomains));
}

// Apply configurations
void CParameterMgr::doApplyConfigurations(bool bForce)
{
    LOG_CONTEXT("Applying configurations");

    CSyncerSet syncerSet;

    core::Results infos;
    // Check subsystems that need resync
    getSystemClass()->checkForSubsystemsToResync(syncerSet, infos);

    // Ensure application of currently selected configurations
    getConfigurableDomains()->apply(_pMainParameterBlackboard, syncerSet, bForce, infos);
    info() << infos;

    // Reset the modified status of the current criteria to indicate that a new configuration has
    // been applied
    getSelectionCriteria()->resetModifiedStatus();
}

// Export to XML string
bool CParameterMgr::exportElementToXMLString(const IXmlSource *pXmlSource,
                                             const string &strRootElementType,
                                             CXmlSerializingContext &&xmlSerializingContext,
                                             string &strResult) const
{
    // Use a doc source by loading data from instantiated Configurable Domains
    CXmlMemoryDocSource memorySource(pXmlSource, false, strRootElementType);

    // Use a doc sink that write the doc data in a string
    ostringstream output;
    CXmlStreamDocSink streamSink(output);

    // Do the export
    bool bProcessSuccess = streamSink.process(memorySource, xmlSerializingContext);

    strResult = output.str();

    return bProcessSuccess;
}

bool CParameterMgr::logResult(bool isSuccess, const std::string &result)
{
    std::string log = result.empty() ? "" : ": " + result;

    if (isSuccess) {
        info() << "Success" << log;
    } else {
        warning() << "Fail" << log;
    }

    return isSuccess;
}

log::details::Info CParameterMgr::info()
{
    return _logger.info();
}

log::details::Warning CParameterMgr::warning()
{
    return _logger.warning();
}
