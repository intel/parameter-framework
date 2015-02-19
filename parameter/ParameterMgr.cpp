/*
 * Copyright (c) 2011-2015, Intel Corporation
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
#include "ParameterMgr.h"
#include "XmlParameterSerializingContext.h"
#include "XmlElementSerializingContext.h"
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
#include "RemoteProcessorServerInterface.h"
#include "ElementLocator.h"
#include "CompoundRule.h"
#include "SelectionCriterionRule.h"
#include "SimulatedBackSynchronizer.h"
#include "HardwareBackSynchronizer.h"
#include "AutoLock.h"
#include <strings.h>
#include <dlfcn.h>
#include <assert.h>
#include "ParameterHandle.h"
#include "LinearParameterAdaptation.h"
#include "LogarithmicParameterAdaptation.h"
#include "EnumValuePair.h"
#include "Subsystem.h"
#include "XmlFileDocSink.h"
#include "XmlFileDocSource.h"
#include "XmlStringDocSink.h"
#include "XmlStringDocSource.h"
#include "XmlMemoryDocSink.h"
#include "XmlMemoryDocSource.h"
#include "Utility.h"
#include <sstream>
#include <memory>

#define base CElement

/** Private macro helper to declare a new context
 *
 * Context declaration always need logger and logging prefix to be
 * passed as parameters.
 * This macro aims to avoid this boring notation.
 * This macro should be called only once in a scope. Nested scopes can
 * call this macro too, as variable shadowing is supported.
 */
#define LOG_CONTEXT(contextTitle) \
    core::log::Context context(_logger, contextTitle)

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

// FIXME: integrate ParameterMgr to core namespace
using namespace core;

// Used for remote processor server creation
typedef IRemoteProcessorServerInterface* (*CreateRemoteProcessorServer)(uint16_t uiPort, IRemoteCommandHandler* pCommandHandler);

// Global configuration file name (fixed)
const char* gacParameterFrameworkConfigurationFileName = "ParameterFrameworkConfiguration.xml";
const char* gacSystemSchemasSubFolder = "Schemas";

// Config File System looks normally like this:
// ---------------------------------------------
//|-- <ParameterFrameworkConfiguration>.xml
//|-- Schemas
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

CParameterMgr::CParameterMgr(const string& strConfigurationFilePath, log::ILogger& logger) :
    _bTuningModeIsOn(false),
    _bValueSpaceIsRaw(false),
    _bOutputRawFormatIsHex(false),
    _bAutoSyncOn(true),
    _pMainParameterBlackboard(new CParameterBlackboard),
    _pElementLibrarySet(new CElementLibrarySet),
    _strXmlConfigurationFilePath(strConfigurationFilePath),
    _pSubsystemPlugins(NULL),
    _pvLibRemoteProcessorHandle(NULL),
    _uiStructureChecksum(0),
    _pRemoteProcessorServer(NULL),
    _commandParser(*this),
    _uiMaxCommandUsageLength(0),
    _logger(logger),
    _bForceNoRemoteInterface(false),
    _bFailOnMissingSubsystem(true),
    _bFailOnFailedSettingsLoad(true),
    _bValidateSchemasOnStart(false)

{
    // Tuning Mode Mutex
    bzero(&_blackboardMutex, sizeof(_blackboardMutex));
    pthread_mutex_init(&_blackboardMutex, NULL);

    // Deal with children
    addChild(new CParameterFrameworkConfiguration);
    addChild(new CSelectionCriteria);
    addChild(new CSystemClass(_logger));
    addChild(new CConfigurableDomains);

    // Configuration file folder
    std::string::size_type slashPos = _strXmlConfigurationFilePath.rfind('/', -1);
    if(slashPos == std::string::npos) {
        // Configuration folder is the current folder
        _strXmlConfigurationFolderPath = '.';
    } else {
        _strXmlConfigurationFolderPath = _strXmlConfigurationFilePath.substr(0, slashPos);
    }

    // Schema absolute folder location
    _strSchemaFolderLocation = _strXmlConfigurationFolderPath + "/" + gacSystemSchemasSubFolder;
}

CParameterMgr::~CParameterMgr()
{
    // Children
    delete _pRemoteProcessorServer;
    delete _pMainParameterBlackboard;
    delete _pElementLibrarySet;

    // Close remote processor library
    if (_pvLibRemoteProcessorHandle) {

        dlclose(_pvLibRemoteProcessorHandle);
    }

    // Tuning Mode Mutex
    pthread_mutex_destroy(&_blackboardMutex);
}

string CParameterMgr::getKind() const
{
    return "ParameterMgr";
}

// Version
string CParameterMgr::getVersion() const
{
    string strVersion;

    // Major
    strVersion = toString(guiEditionMajor) + ".";
    // Minor
    strVersion += toString(guiEditionMinor) + ".";
    // Revision
    strVersion += toString(guiRevision);

    return strVersion;
}

bool CParameterMgr::load(string& strError)
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
    CConfigurableDomains* pConfigurableDomains = getConfigurableDomains();

    // We need to ensure all domains are valid
    pConfigurableDomains->validate(_pMainParameterBlackboard);

    // Log selection criterion states
    {
        LOG_CONTEXT("Criterion states");

        const CSelectionCriteria* selectionCriteria = getConstSelectionCriteria();

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

bool CParameterMgr::loadFrameworkConfiguration(string& strError)
{
    LOG_CONTEXT("Loading framework configuration");

    // Parse Structure XML file
    CXmlElementSerializingContext elementSerializingContext(strError);

    if (!xmlParse(elementSerializingContext, getFrameworkConfiguration(), _strXmlConfigurationFilePath, _strXmlConfigurationFolderPath, EFrameworkConfigurationLibrary)) {

        return false;
    }
    // Set class name to system class and configurable domains
    getSystemClass()->setName(getConstFrameworkConfiguration()->getSystemClassName());
    getConfigurableDomains()->setName(getConstFrameworkConfiguration()->getSystemClassName());

    // Get subsystem plugins elements
    _pSubsystemPlugins = static_cast<const CSubsystemPlugins*>(getConstFrameworkConfiguration()->findChild("SubsystemPlugins"));

    if (!_pSubsystemPlugins) {

        strError = "Parameter Framework Configuration: couldn't find SubsystemPlugins element";

        return false;
    }

    // Log tuning availability
    info() << "Tuning "
           << (getConstFrameworkConfiguration()->isTuningAllowed() ? "allowed" : "prohibited");

    return true;
}

bool CParameterMgr::loadSubsystems(std::string& error)
{
    LOG_CONTEXT("Loading subsystem plugins");

    // Load subsystems
    bool isSuccess = getSystemClass()->loadSubsystems(error,
                                                      _pSubsystemPlugins,
                                                      !_bFailOnMissingSubsystem);

    if (isSuccess) {
        info() << "All subsystem plugins successfully loaded";

        if(!error.empty()) {
            // Log missing subsystems as info
            info() << error;
        }
    } else {
        warning() << error;
    }
    return isSuccess;
}

bool CParameterMgr::loadStructure(string& strError)
{
    // Retrieve system to load structure to
    CSystemClass* pSystemClass = getSystemClass();

    LOG_CONTEXT("Loading " + pSystemClass->getName() + " system class structure");

    // Get structure description element
    const CFrameworkConfigurationLocation* pStructureDescriptionFileLocation = static_cast<const CFrameworkConfigurationLocation*>(getConstFrameworkConfiguration()->findChildOfKind("StructureDescriptionFileLocation"));

    if (!pStructureDescriptionFileLocation) {

        strError = "No StructureDescriptionFileLocation element found for SystemClass " + pSystemClass->getName();

        return false;
    }

    // Get Xml structure folder
    string strXmlStructureFolder = pStructureDescriptionFileLocation->getFolderPath(_strXmlConfigurationFolderPath);

    // Get Xml structure file name
    string strXmlStructureFilePath = pStructureDescriptionFileLocation->getFilePath(_strXmlConfigurationFolderPath);

    // Parse Structure XML file
    CXmlParameterSerializingContext parameterBuildContext(strError);

    {
        LOG_CONTEXT("Importing system structure from file " + strXmlStructureFilePath);

        if (!xmlParse(parameterBuildContext, pSystemClass, strXmlStructureFilePath, strXmlStructureFolder, EParameterCreationLibrary)) {

            return false;
        }
    }

    // Initialize offsets
    pSystemClass->setOffset(0);

    // Initialize main blackboard's size
    _pMainParameterBlackboard->setSize(pSystemClass->getFootPrint());

    return true;
}

bool CParameterMgr::loadSettings(string& strError)
{
    string strLoadError;
    bool success = loadSettingsFromConfigFile(strLoadError);

    if (!success && !_bFailOnFailedSettingsLoad) {
        // Load can not fail, ie continue but log the load errors
        info() << strLoadError;
        info() << "Failed to load settings, continue without domains.";
        success = true;
    }

    if (!success) {
        // Propagate the litteral error only if the function fails
        strError = strLoadError;
        return false;
    }

    return true;
}

bool CParameterMgr::loadSettingsFromConfigFile(string& strError)
{
    LOG_CONTEXT("Loading settings");

    // Get settings configuration element
    const CFrameworkConfigurationGroup* pParameterConfigurationGroup = static_cast<const CFrameworkConfigurationGroup*>(getConstFrameworkConfiguration()->findChildOfKind("SettingsConfiguration"));

    if (!pParameterConfigurationGroup) {

        // No settings to load

        return true;
    }
    // Get binary settings file location
    const CFrameworkConfigurationLocation* pBinarySettingsFileLocation = static_cast<const CFrameworkConfigurationLocation*>(pParameterConfigurationGroup->findChildOfKind("BinarySettingsFileLocation"));

    string strXmlBinarySettingsFilePath;

    if (pBinarySettingsFileLocation) {

        // Get Xml binary settings file name
        strXmlBinarySettingsFilePath = pBinarySettingsFileLocation->getFilePath(_strXmlConfigurationFolderPath);
    }

    // Get configurable domains element
    const CFrameworkConfigurationLocation* pConfigurableDomainsFileLocation = static_cast<const CFrameworkConfigurationLocation*>(pParameterConfigurationGroup->findChildOfKind("ConfigurableDomainsFileLocation"));

    if (!pConfigurableDomainsFileLocation) {

        strError = "No ConfigurableDomainsFileLocation element found for SystemClass " + getSystemClass()->getName();

        return false;
    }
    // Get destination root element
    CConfigurableDomains* pConfigurableDomains = getConfigurableDomains();

    // Get Xml configuration domains file name
    string strXmlConfigurationDomainsFilePath = pConfigurableDomainsFileLocation->getFilePath(_strXmlConfigurationFolderPath);

    // Get Xml configuration domains folder
    string strXmlConfigurationDomainsFolder = pConfigurableDomainsFileLocation->getFolderPath(_strXmlConfigurationFolderPath);

    // Parse configuration domains XML file (ask to read settings from XML file if they are not provided as binary)
    CXmlDomainImportContext xmlDomainImportContext(strError, !pBinarySettingsFileLocation,
            *getSystemClass());

    // Selection criteria definition for rule creation
    xmlDomainImportContext.setSelectionCriteriaDefinition(getConstSelectionCriteria()->getSelectionCriteriaDefinition());

    // Auto validation of configurations if no binary settings provided
    xmlDomainImportContext.setAutoValidationRequired(!pBinarySettingsFileLocation);

    info() << "Importing configurable domains from file " << strXmlConfigurationDomainsFilePath
           << " "  << ( pBinarySettingsFileLocation ? "without" : "with") << " settings";

    // Do parse
    if (!xmlParse(xmlDomainImportContext, pConfigurableDomains, strXmlConfigurationDomainsFilePath, strXmlConfigurationDomainsFolder, EParameterConfigurationLibrary, "SystemClassName")) {

        return false;
    }
    // We have loaded the whole system structure, compute checksum
    const CSystemClass* pSystemClass = getConstSystemClass();
    _uiStructureChecksum = pSystemClass->computeStructureChecksum() + getConfigurableDomains()->computeStructureChecksum() + getSelectionCriteria()->computeStructureChecksum();

    // Load binary settings if any provided
    if (pBinarySettingsFileLocation && !pConfigurableDomains->serializeSettings(strXmlBinarySettingsFilePath, false, _uiStructureChecksum, strError)) {

        return false;
    }

    return true;
}

bool CParameterMgr::importDomainFromFile(const string& strXmlFilePath, bool bOverwrite,
                                         string& strError)
{
    CXmlDomainImportContext xmlDomainImportContext(strError, true, *getSystemClass());

    // Selection criteria definition for rule creation
    xmlDomainImportContext.setSelectionCriteriaDefinition(
            getConstSelectionCriteria()->getSelectionCriteriaDefinition());

    // Auto validation of configurations
    xmlDomainImportContext.setAutoValidationRequired(true);

    // We initialize the domain with an empty name but since we have set the isDomainStandalone
    // context, the name will be retrieved during de-serialization
    std::auto_ptr<CConfigurableDomain> standaloneDomain(new CConfigurableDomain());
    bool bSuccess = xmlParse(xmlDomainImportContext, standaloneDomain.get(),
                             strXmlFilePath, "", EParameterConfigurationLibrary, "");

    if (!bSuccess) {
        return false;
    }

    LOG_CONTEXT("Adding configurable domain '" + standaloneDomain->getName() + "'");

    if (!logResult(getConfigurableDomains()->addDomain(
                    *standaloneDomain, bOverwrite, strError), strError)) {
        return false;
    }

    // ownership has been transfered to the ConfigurableDomains object
    standaloneDomain.release();
    return true;
}

// XML parsing
bool CParameterMgr::xmlParse(CXmlElementSerializingContext& elementSerializingContext, CElement* pRootElement, const string& strXmlFilePath, const string& strXmlFolder, CParameterMgr::ElementLibrary eElementLibrary, const string& strNameAttrituteName)
{
    // Init serializing context
    elementSerializingContext.set(_pElementLibrarySet->getElementLibrary(
                                      eElementLibrary), strXmlFolder, _strSchemaFolderLocation);

    // Get Schema file associated to root element
    string strXmlSchemaFilePath = _strSchemaFolderLocation + "/" + pRootElement->getKind() + ".xsd";

    std::auto_ptr<CXmlFileDocSource> fileDocSource(NULL);

    if (strNameAttrituteName.empty()) {
        fileDocSource.reset(new CXmlFileDocSource(strXmlFilePath, strXmlSchemaFilePath,
                                                pRootElement->getKind(),
                                                _bValidateSchemasOnStart));
    } else {
        fileDocSource.reset(new CXmlFileDocSource(strXmlFilePath, strXmlSchemaFilePath,
                                               pRootElement->getKind(),
                                               pRootElement->getName(), strNameAttrituteName,
                                               _bValidateSchemasOnStart));
    }

    // Start clean
    pRootElement->clean();

    CXmlMemoryDocSink memorySink(pRootElement);

    if (!memorySink.process(*fileDocSource, elementSerializingContext)) {
        //Cleanup
        pRootElement->clean();

        return false;
    }

    return true;
}

// Init
bool CParameterMgr::init(string& strError)
{
    return base::init(strError);
}

// Selection criteria interface
CSelectionCriterionType* CParameterMgr::createSelectionCriterionType(bool bIsInclusive)
{
    // Propagate
    return getSelectionCriteria()->createSelectionCriterionType(bIsInclusive);
}

CSelectionCriterion* CParameterMgr::createSelectionCriterion(const string& strName, const CSelectionCriterionType* pSelectionCriterionType)
{
    // Propagate
    return getSelectionCriteria()->createSelectionCriterion(strName,
                                                            pSelectionCriterionType,
                                                            _logger);
}

// Selection criterion retrieval
CSelectionCriterion* CParameterMgr::getSelectionCriterion(const string& strName)
{
    // Propagate
    return getSelectionCriteria()->getSelectionCriterion(strName);
}

// Configuration application
void CParameterMgr::applyConfigurations()
{
    LOG_CONTEXT("Configuration application request");

    // Lock state
    CAutoLock autoLock(&_blackboardMutex);

    if (!_bTuningModeIsOn) {

        // Apply configuration(s)
        doApplyConfigurations(false);
    } else {

        warning() << "Configurations were not applied because the TuningMode is on";
    }
}

// Get the configurableElement corresponding to the given path
const CConfigurableElement* CParameterMgr::getConfigurableElement(const string& strPath,
                                                                  string& strError) const
{
    CPathNavigator pathNavigator(strPath);

    // Nagivate through system class
    if (!pathNavigator.navigateThrough(getConstSystemClass()->getName(), strError)) {

        return NULL;
    }

    // Find element
    const CElement* pElement = getConstSystemClass()->findDescendant(pathNavigator);

    if (!pElement) {

        strError = "Path not found: " + strPath;

        return NULL;
    }

    // Check found element is a parameter
    const CConfigurableElement* pConfigurableElement = static_cast<const CConfigurableElement*>(pElement);

    return pConfigurableElement;
}

// Dynamic parameter handling
CParameterHandle* CParameterMgr::createParameterHandle(const string& strPath, string& strError)
{
    const CConfigurableElement* pConfigurableElement = getConfigurableElement(strPath, strError);

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
    return new CParameterHandle(static_cast<const CBaseParameter*>(pConfigurableElement), this);
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

bool CParameterMgr::getFailureOnFailedSettingsLoad()
{
    return _bFailOnFailedSettingsLoad;
}

const string& CParameterMgr::getSchemaFolderLocation() const
{
    return _strSchemaFolderLocation;
}

void CParameterMgr::setSchemaFolderLocation(const string& strSchemaFolderLocation)
{
    _strSchemaFolderLocation = strSchemaFolderLocation;
}

void CParameterMgr::setValidateSchemasOnStart(bool bValidate)
{
    _bValidateSchemasOnStart = bValidate;
}

bool CParameterMgr::getValidateSchemasOnStart() const
{
    return _bValidateSchemasOnStart;
}


// User set/get parameters in main BlackBoard
bool CParameterMgr::accessParameterValue(const string& strPath, string& strValue, bool bSet, string& strError)
{
    // Forbid write access when not in TuningMode
    if (bSet && !checkTuningModeOn(strError)) {

        return false;
    }

    // Define context
    CParameterAccessContext parameterAccessContext(strError, _pMainParameterBlackboard, _bValueSpaceIsRaw, _bOutputRawFormatIsHex);

    // Activate the auto synchronization with the hardware
    if (bSet) {

        parameterAccessContext.setAutoSync(_bAutoSyncOn);
    }

    return accessValue(parameterAccessContext, strPath, strValue, bSet, strError);
}

// User get parameter mapping
bool CParameterMgr::getParameterMapping(const string& strPath, string& strResult) const
{
    CPathNavigator pathNavigator(strPath);

    // Nagivate through system class
    if (!pathNavigator.navigateThrough(getConstSystemClass()->getName(), strResult)) {

        return false;
    }

    // Get the ConfigurableElement corresponding to strPath
    const CConfigurableElement* pConfigurableElement = getConfigurableElement(strPath, strResult);
    if (!pConfigurableElement) {

        return false;
    }

    // Find the list of the ancestors of the current ConfigurableElement that have a mapping
    list<const CConfigurableElement*> configurableElementPath;
    pConfigurableElement->getListOfElementsWithMapping(configurableElementPath);

    // Get the Subsystem containing the ConfigurableElement
    const CSubsystem* pSubsystem = pConfigurableElement->getBelongingSubsystem();
    if (!pSubsystem) {

        strResult = "Unable to find the Subsystem containing the parameter";
        return false;
    }

    // Fetch the mapping corresponding to the ConfigurableElement
    strResult = pSubsystem->getMapping(configurableElementPath);

    return true;
}

// User set/get parameters in specific Configuration BlackBoard
bool CParameterMgr::accessConfigurationValue(const string& strDomain, const string& strConfiguration, const string& strPath, string& strValue, bool bSet, string& strError)
{
    CElementLocator elementLocator(getSystemClass());

    CElement* pLocatedElement = NULL;

    if (!elementLocator.locate(strPath, &pLocatedElement, strError)) {

        return false;
    }

    // Convert element
    const CConfigurableElement* pConfigurableElement = static_cast<const CConfigurableElement*>(pLocatedElement);

    // Get the Configuration blackboard and the Base Offset of the configurable element in this blackboard
    uint32_t uiBaseOffset;
    bool bIsLastApplied;

    CParameterBlackboard* pConfigurationBlackboard = NULL;

    {
        LOG_CONTEXT("Find configuration blackboard for Domain: " + strDomain +
                    ", Configuration: " + strConfiguration +
                    ", Element: " + pConfigurableElement->getPath());

        pConfigurationBlackboard =
            getConstConfigurableDomains()->findConfigurationBlackboard(strDomain,
                                                                       strConfiguration,
                                                                       pConfigurableElement,
                                                                       uiBaseOffset,
                                                                       bIsLastApplied,
                                                                       strError);
        if (!pConfigurationBlackboard) {

            warning() << "Fail: " << strError;
            return false;
        }
    }

    info() << "Element " << strPath << " in Domain " << strDomain << ", offset: "
           << pConfigurableElement->getOffset() << ", base offset: " << uiBaseOffset;

    /// Update the Configuration Blackboard

    // Define Configuration context using Base Offset and keep Auto Sync off to prevent access to HW
    CParameterAccessContext parameterAccessContext(strError, pConfigurationBlackboard, _bValueSpaceIsRaw, _bOutputRawFormatIsHex, uiBaseOffset);

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
bool CParameterMgr::accessValue(CParameterAccessContext& parameterAccessContext, const string& strPath, string& strValue, bool bSet, string& strError)
{
    // Lock state
    CAutoLock autoLock(&_blackboardMutex);

    CPathNavigator pathNavigator(strPath);

    // Nagivate through system class
    if (!pathNavigator.navigateThrough(getConstSystemClass()->getName(), strError)) {

        parameterAccessContext.setError(strError);

        return false;
    }

    // Do the get
    return getConstSystemClass()->accessValue(pathNavigator, strValue, bSet, parameterAccessContext);
}

// Tuning mode
bool CParameterMgr::setTuningMode(bool bOn, string& strError)
{
    // Tuning allowed?
    if (bOn && !getConstFrameworkConfiguration()->isTuningAllowed()) {

        strError = "Tuning prohibited";

        return false;
    }
    // Lock state
    CAutoLock autoLock(&_blackboardMutex);

    // Warn domains about exiting tuning mode
    if (!bOn && _bTuningModeIsOn) {

        // Ensure application of currently selected configurations
        // Force-apply configurations
        doApplyConfigurations(true);

        // Turn auto sync back on
        _bAutoSyncOn = true;
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
bool CParameterMgr::setAutoSync(bool bAutoSyncOn, string& strError)
{
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        return false;
    }
    // Warn domains about turning auto sync back on
    if (bAutoSyncOn && !_bAutoSyncOn) {

        // Do the synchronization at system class level (could be optimized by keeping track of all modified parameters)
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
bool CParameterMgr::sync(string& strError)
{
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        return false;
    }
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
    if (! syncerSet.sync(*_pMainParameterBlackboard, false, &error)){

        CUtility::asString(error, strError);
        return false;
    };

    return true;
}

// Configuration/Domains handling
bool CParameterMgr::createDomain(const string& strName, string& strError)
{
    LOG_CONTEXT("Creating configurable domain " + strName);
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        return false;
    }

    // Delegate to configurable domains
    return logResult(getConfigurableDomains()->createDomain(strName, strError), strError);
}

bool CParameterMgr::deleteDomain(const string& strName, string& strError)
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

bool CParameterMgr::renameDomain(const string& strName, const string& strNewName, string& strError)
{
    LOG_CONTEXT("Renaming configurable domain '" + strName + "' to '" + strNewName + "'");

    // Delegate to configurable domains
    return logResult(getConfigurableDomains()->renameDomain(
                                                        strName, strNewName, strError), strError);
}

bool CParameterMgr::deleteAllDomains(string& strError)
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

bool CParameterMgr::setSequenceAwareness(const string& strName, bool bSequenceAware, string& strResult)
{
    LOG_CONTEXT("Making domain '" + strName +
                "' sequence " + (bSequenceAware ? "aware" : "unaware"));
    // Check tuning mode
    if (!checkTuningModeOn(strResult)) {

        warning() << "Fail: " << strResult;
        return false;
    }

    return logResult(getConfigurableDomains()->setSequenceAwareness(
                                                    strName, bSequenceAware, strResult), strResult);
}

bool CParameterMgr::getSequenceAwareness(const string& strName, bool& bSequenceAware,
                                         string& strResult)
{
    return getConfigurableDomains()->getSequenceAwareness(strName, bSequenceAware, strResult);
}

bool CParameterMgr::createConfiguration(const string& strDomain, const string& strConfiguration, string& strError)
{
    LOG_CONTEXT("Creating domain configuration '" + strConfiguration +
                "' into domain '" + strDomain + "'");
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail: " << strError;
        return false;
    }

    // Delegate to configurable domains
    return logResult(getConfigurableDomains()->createConfiguration(
                    strDomain, strConfiguration, _pMainParameterBlackboard, strError), strError);
}
bool CParameterMgr::renameConfiguration(const string& strDomain, const string& strConfiguration,
                                        const string& strNewConfiguration, string& strError)
{
    LOG_CONTEXT("Renaming domain '" + strDomain + "''s configuration '" +
                strConfiguration + "' to '" + strNewConfiguration + "'");

    return logResult(getConfigurableDomains()->renameConfiguration(
                            strDomain, strConfiguration, strNewConfiguration, strError), strError);
}

bool CParameterMgr::deleteConfiguration(const string& strDomain, const string& strConfiguration, string& strError)
{
    LOG_CONTEXT("Deleting configuration '" + strConfiguration +
                "' from domain '" + strDomain + "'");

    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail:" << strError;
        return false;
    }

    // Delegate to configurable domains
    return logResult(getConfigurableDomains()->deleteConfiguration(
                                                 strDomain, strConfiguration, strError), strError);
}

bool CParameterMgr::restoreConfiguration(const string& strDomain,
                                         const string& strConfiguration,
                                         core::Results& errors)
{
    string strError;
    LOG_CONTEXT("Restoring domain '" + strDomain + "''s configuration '" +
                strConfiguration + "' to parameter blackboard");
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        errors.push_back(strError);
        warning() << "Fail:" << strError;
        return false;
    }

    // Delegate to configurable domains
    return logResult(getConstConfigurableDomains()->restoreConfiguration(
                strDomain, strConfiguration, _pMainParameterBlackboard, _bAutoSyncOn, errors),
                     strError);
}

bool CParameterMgr::saveConfiguration(const string& strDomain, const string& strConfiguration, string& strError)
{
    LOG_CONTEXT("Saving domain '" + strDomain + "' configuration '" +
                strConfiguration + "' from parameter blackboard");
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail:" << strError;
        return false;
    }

    // Delegate to configurable domains
    return logResult(getConfigurableDomains()->saveConfiguration(
                strDomain, strConfiguration, _pMainParameterBlackboard, strError), strError);
}

// Configurable element - domain association
bool CParameterMgr::addConfigurableElementToDomain(const string& strDomain, const string& strConfigurableElementPath, string& strError)
{
    LOG_CONTEXT("Adding configurable element '" + strConfigurableElementPath +
                "to domain '" + strDomain + "'");
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail: " << strError;
        return false;
    }

    CElementLocator elementLocator(getSystemClass());

    CElement* pLocatedElement = NULL;

    if (!elementLocator.locate(strConfigurableElementPath, &pLocatedElement, strError)) {

        warning() << "Fail: " << strError;
        return false;
    }

    // Convert element
    CConfigurableElement* pConfigurableElement = static_cast<CConfigurableElement*>(pLocatedElement);

    // Delegate
    core::Results infos;
    bool isSuccess = getConfigurableDomains()->addConfigurableElementToDomain(
            strDomain, pConfigurableElement, _pMainParameterBlackboard, infos);

    if (isSuccess) {
        info() << infos;
    } else {
        warning() << infos;
    }

    CUtility::asString(infos, strError);
    return isSuccess;
}

bool CParameterMgr::removeConfigurableElementFromDomain(const string& strDomain, const string& strConfigurableElementPath, string& strError)
{
    LOG_CONTEXT("Removing configurable element '" + strConfigurableElementPath +
                "' from domain '" + strDomain + "'");

    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail:" << strError;
        return false;
    }

    CElementLocator elementLocator(getSystemClass());

    CElement* pLocatedElement = NULL;

    if (!elementLocator.locate(strConfigurableElementPath, &pLocatedElement, strError)) {

        warning() << "Fail:" << strError;
        return false;
    }

    // Convert element
    CConfigurableElement* pConfigurableElement = static_cast<CConfigurableElement*>(pLocatedElement);

    // Delegate
    return logResult(getConfigurableDomains()->removeConfigurableElementFromDomain(
                                            strDomain, pConfigurableElement, strError), strError);
}

bool CParameterMgr::split(const string& strDomain, const string& strConfigurableElementPath, string& strError)
{
    LOG_CONTEXT("Splitting configurable element '" + strConfigurableElementPath +
                "' domain '" + strDomain + "'");
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        warning() << "Fail:" << strError;
        return false;
    }

    CElementLocator elementLocator(getSystemClass());

    CElement* pLocatedElement = NULL;

    if (!elementLocator.locate(strConfigurableElementPath, &pLocatedElement, strError)) {

        warning() << "Fail: " << strError;
        return false;
    }

    // Convert element
    CConfigurableElement* pConfigurableElement = static_cast<CConfigurableElement*>(pLocatedElement);

    // Delegate
    core::Results infos;
    bool isSuccess = getConfigurableDomains()->split(strDomain, pConfigurableElement, infos);

    if (isSuccess) {
        info() << infos;
    } else {
        warning() << infos;
    }

    CUtility::asString(infos, strError);
    return isSuccess;
}

bool CParameterMgr::setElementSequence(const string& strDomain, const string& strConfiguration,
                                       const std::vector<string>& astrNewElementSequence,
                                       string& strError)
{
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        return false;
    }

    return getConfigurableDomains()->setElementSequence(strDomain, strConfiguration,
            astrNewElementSequence, strError);
}

bool CParameterMgr::getApplicationRule(const string& strDomain, const string& strConfiguration,
                                       string& strResult)
{
    return getConfigurableDomains()->getApplicationRule(strDomain, strConfiguration, strResult);
}

bool CParameterMgr::setApplicationRule(const string& strDomain, const string& strConfiguration,
                                       const string& strApplicationRule, string& strError)
{
    return getConfigurableDomains()->setApplicationRule(strDomain, strConfiguration,
            strApplicationRule, getConstSelectionCriteria()->getSelectionCriteriaDefinition(),
            strError);
}

bool CParameterMgr::clearApplicationRule(const string& strDomain, const string& strConfiguration,
                                         string& strError)
{
    return getConfigurableDomains()->clearApplicationRule(strDomain, strConfiguration, strError);
}

bool CParameterMgr::importDomainsXml(const string& strXmlSource, bool bWithSettings,
                                     bool bFromFile, string& strError)
{
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        return false;
    }

    // check path is absolute
    if (bFromFile && strXmlSource[0] != '/') {

        strError = "Please provide absolute path";

        return false;
    }
    // Root element
    CConfigurableDomains* pConfigurableDomains = getConfigurableDomains();

    // Context
    CXmlDomainImportContext xmlDomainImportContext(strError, bWithSettings, *getSystemClass());

    // Selection criteria definition for rule creation
    xmlDomainImportContext.setSelectionCriteriaDefinition(
            getConstSelectionCriteria()->getSelectionCriteriaDefinition());

    // Init serializing context
    xmlDomainImportContext.set(
            _pElementLibrarySet->getElementLibrary(EParameterConfigurationLibrary),
            "", _strSchemaFolderLocation);

    // Get Schema file associated to root element
    string strXmlSchemaFilePath = _strSchemaFolderLocation + "/" +
                                  pConfigurableDomains->getKind() + ".xsd";

    // Xml Source
    CXmlDocSource* pSource;

    if (bFromFile) {

        // when importing from a file strXmlSource is the file name
        pSource = new CXmlFileDocSource(strXmlSource, strXmlSchemaFilePath,
                                        pConfigurableDomains->getKind(),
                                        pConfigurableDomains->getName(), "SystemClassName",
                                        _bValidateSchemasOnStart);

    } else {

        // when importing from an xml string, strXmlSource contains the string
        pSource = new CXmlStringDocSource(strXmlSource, strXmlSchemaFilePath,
                                          pConfigurableDomains->getKind(),
                                          pConfigurableDomains->getName(), "SystemClassName",
                                          _bValidateSchemasOnStart);

    }
    // Start clean
    pConfigurableDomains->clean();

    // Use a doc sink that instantiate Configurable Domains from the given doc source
    CXmlMemoryDocSink memorySink(pConfigurableDomains);

    bool bProcessSuccess = memorySink.process(*pSource, xmlDomainImportContext);

    if (!bProcessSuccess) {

        //Cleanup
        pConfigurableDomains->clean();

    } else {

        // Validate domains after XML import
        pConfigurableDomains->validate(_pMainParameterBlackboard);

    }

    delete pSource;

    return bProcessSuccess;
}

bool CParameterMgr::importSingleDomainXml(const string& strXmlSource, bool bOverwrite,
                                          string& strError)
{
    if (!checkTuningModeOn(strError)) {

        return false;
    }

    // check path is absolute
    if (strXmlSource[0] != '/') {

        strError = "Please provide absolute path";

        return false;
    }

    return importDomainFromFile(strXmlSource, bOverwrite, strError);
}

bool CParameterMgr::serializeElement(string& strXmlDest,
                                     CXmlSerializingContext& xmlSerializingContext, bool bToFile,
                                     const CElement& element, string& strError) const
{
    // check path is absolute
    if (bToFile && strXmlDest[0] != '/') {

        strError = "Please provide absolute path";

        return false;
    }

    // Get Schema file associated to root element
    string strXmlSchemaFilePath = _strSchemaFolderLocation + "/" +
                                  element.getKind() + ".xsd";

    // Use a doc source by loading data from instantiated Configurable Domains
    CXmlMemoryDocSource memorySource(&element, element.getKind(),
                                     strXmlSchemaFilePath, "parameter-framework",
                                     getVersion(), _bValidateSchemasOnStart);

    // Xml Sink
    CXmlDocSink* pSink;

    if (bToFile) {

        // Use a doc sink to write the doc data in a file
        pSink = new CXmlFileDocSink(strXmlDest);

    } else {

        // Use a doc sink to write the doc data in a string
        // TODO: use a stream rather than a string
        pSink = new CXmlStringDocSink(strXmlDest);
    }

    bool bProcessSuccess = pSink->process(memorySource, xmlSerializingContext);

    delete pSink;
    return bProcessSuccess;
}

bool CParameterMgr::exportDomainsXml(string& strXmlDest, bool bWithSettings, bool bToFile,
                                     string& strError) const
{
    const CConfigurableDomains* pConfigurableDomains = getConstConfigurableDomains();

    CXmlDomainExportContext xmlDomainExportContext(strError, bWithSettings);

    xmlDomainExportContext.setValueSpaceRaw(_bValueSpaceIsRaw);

    xmlDomainExportContext.setOutputRawFormat(_bOutputRawFormatIsHex);


    return serializeElement(strXmlDest, xmlDomainExportContext, bToFile,
                                    *pConfigurableDomains, strError);
}

bool CParameterMgr::exportSingleDomainXml(string& strXmlDest, const string& strDomainName,
                                          bool bWithSettings, bool bToFile, string& strError) const
{
    const CConfigurableDomains* pAllDomains = getConstConfigurableDomains();

    // Element to be serialized
    const CConfigurableDomain* pRequestedDomain =
        pAllDomains->findConfigurableDomain(strDomainName, strError);

    if (!pRequestedDomain) {
        return false;
    }

    CXmlDomainExportContext xmlDomainExportContext(strError, bWithSettings);

    xmlDomainExportContext.setValueSpaceRaw(_bValueSpaceIsRaw);

    xmlDomainExportContext.setOutputRawFormat(_bOutputRawFormatIsHex);

    return serializeElement(strXmlDest, xmlDomainExportContext, bToFile,
                                    *pRequestedDomain, strError);
}

// Binary Import/Export
bool CParameterMgr::importDomainsBinary(const string& strFileName, string& strError)
{
    // Check tuning mode
    if (!checkTuningModeOn(strError)) {

        return false;
    }
    // check path is absolute
    if (strFileName[0] != '/') {

        strError = "Please provide absolute path";

        return false;
    }
    // Root element
    CConfigurableDomains* pConfigurableDomains = getConfigurableDomains();

    // Serialize in
    return pConfigurableDomains->serializeSettings(strFileName, false, _uiStructureChecksum, strError);
}

bool CParameterMgr::exportDomainsBinary(const string& strFileName, string& strError)
{
    // check path is absolute
    if (strFileName[0] != '/') {

        strError = "Please provide absolute path";

        return false;
    }

    // Root element
    CConfigurableDomains* pConfigurableDomains = getConfigurableDomains();

    // Serialize out
    return pConfigurableDomains->serializeSettings(strFileName, true, _uiStructureChecksum, strError);
}

// For tuning, check we're in tuning mode
bool CParameterMgr::checkTuningModeOn(string& strError) const
{
    // Tuning Mode on?
    if (!_bTuningModeIsOn) {

        strError = "Tuning Mode must be on";

        return false;
    }
    return true;
}

// Tuning mutex dynamic parameter handling
pthread_mutex_t* CParameterMgr::getBlackboardMutex()
{
    return &_blackboardMutex;
}

// Blackboard reference (dynamic parameter handling)
CParameterBlackboard* CParameterMgr::getParameterBlackboard()
{
    return _pMainParameterBlackboard;
}

// Dynamic creation library feeding
void CParameterMgr::feedElementLibraries()
{
    // Global Configuration handling
    CElementLibrary* pFrameworkConfigurationLibrary = new CElementLibrary;

    pFrameworkConfigurationLibrary->addElementBuilder("ParameterFrameworkConfiguration", new TElementBuilderTemplate<CParameterFrameworkConfiguration>());
    pFrameworkConfigurationLibrary->addElementBuilder("SubsystemPlugins", new TKindElementBuilderTemplate<CSubsystemPlugins>());
    pFrameworkConfigurationLibrary->addElementBuilder("Location", new TKindElementBuilderTemplate<CPluginLocation>());
    pFrameworkConfigurationLibrary->addElementBuilder("StructureDescriptionFileLocation", new TKindElementBuilderTemplate<CFrameworkConfigurationLocation>());
    pFrameworkConfigurationLibrary->addElementBuilder("SettingsConfiguration", new TKindElementBuilderTemplate<CFrameworkConfigurationGroup>());
    pFrameworkConfigurationLibrary->addElementBuilder("ConfigurableDomainsFileLocation", new TKindElementBuilderTemplate<CFrameworkConfigurationLocation>());
    pFrameworkConfigurationLibrary->addElementBuilder("BinarySettingsFileLocation", new TKindElementBuilderTemplate<CFrameworkConfigurationLocation>());

    _pElementLibrarySet->addElementLibrary(pFrameworkConfigurationLibrary);

    // Parameter creation
    CElementLibrary* pParameterCreationLibrary = new CElementLibrary;

    pParameterCreationLibrary->addElementBuilder("Subsystem", new CSubsystemElementBuilder(getSystemClass()->getSubsystemLibrary()));
    pParameterCreationLibrary->addElementBuilder("ComponentType", new TNamedElementBuilderTemplate<CComponentType>());
    pParameterCreationLibrary->addElementBuilder("Component", new TNamedElementBuilderTemplate<CComponentInstance>());
    pParameterCreationLibrary->addElementBuilder("BitParameter", new TNamedElementBuilderTemplate<CBitParameterType>());
    pParameterCreationLibrary->addElementBuilder("BitParameterBlock", new TNamedElementBuilderTemplate<CBitParameterBlockType>());
    pParameterCreationLibrary->addElementBuilder("StringParameter", new TNamedElementBuilderTemplate<CStringParameterType>());
    pParameterCreationLibrary->addElementBuilder("ParameterBlock", new TNamedElementBuilderTemplate<CParameterBlockType>());
    pParameterCreationLibrary->addElementBuilder("BooleanParameter", new TNamedElementBuilderTemplate<CBooleanParameterType>());
    pParameterCreationLibrary->addElementBuilder("IntegerParameter", new TNamedElementBuilderTemplate<CIntegerParameterType>());
    pParameterCreationLibrary->addElementBuilder("LinearAdaptation", new TElementBuilderTemplate<CLinearParameterAdaptation>());
    pParameterCreationLibrary->addElementBuilder("LogarithmicAdaptation", new TElementBuilderTemplate<CLogarithmicParameterAdaptation>());
    pParameterCreationLibrary->addElementBuilder("EnumParameter", new TNamedElementBuilderTemplate<CEnumParameterType>());
    pParameterCreationLibrary->addElementBuilder("ValuePair", new TElementBuilderTemplate<CEnumValuePair>());
    pParameterCreationLibrary->addElementBuilder("FixedPointParameter", new TNamedElementBuilderTemplate<CFixedPointParameterType>());
    pParameterCreationLibrary->addElementBuilder("SubsystemInclude", new CFileIncluderElementBuilder(_bValidateSchemasOnStart));

    _pElementLibrarySet->addElementLibrary(pParameterCreationLibrary);

    // Parameter Configuration Domains creation
    CElementLibrary* pParameterConfigurationLibrary = new CElementLibrary;

    pParameterConfigurationLibrary->addElementBuilder("ConfigurableDomain", new TElementBuilderTemplate<CConfigurableDomain>());
    pParameterConfigurationLibrary->addElementBuilder("Configuration", new TNamedElementBuilderTemplate<CDomainConfiguration>());
    pParameterConfigurationLibrary->addElementBuilder("CompoundRule", new TElementBuilderTemplate<CCompoundRule>());
    pParameterConfigurationLibrary->addElementBuilder("SelectionCriterionRule", new TElementBuilderTemplate<CSelectionCriterionRule>());

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

// Remote Processor Server connection handling
bool CParameterMgr::handleRemoteProcessingInterface(string& strError)
{
    LOG_CONTEXT("Handling remote processing interface");

    if (_bForceNoRemoteInterface) {
        // The user requested not to start the remote interface
        return true;
    }

    // Start server if tuning allowed
    if (getConstFrameworkConfiguration()->isTuningAllowed()) {

        info() << "Loading remote processor library";

        // Load library
        _pvLibRemoteProcessorHandle = dlopen("libremote-processor.so", RTLD_NOW);

        if (!_pvLibRemoteProcessorHandle) {

            // Return error
            const char* pcError = dlerror();

            if (pcError) {

                strError = pcError;
            } else {

                strError = "Unable to load libremote-processor.so library";
            }

            return false;
        }

        CreateRemoteProcessorServer pfnCreateRemoteProcessorServer = (CreateRemoteProcessorServer)dlsym(_pvLibRemoteProcessorHandle, "createRemoteProcessorServer");

        if (!pfnCreateRemoteProcessorServer) {

            strError = "libremote-process.so does not contain createRemoteProcessorServer symbol.";

            return false;
        }

        // Create server
        _pRemoteProcessorServer =
            pfnCreateRemoteProcessorServer(getConstFrameworkConfiguration()->getServerPort(),
                                           _commandParser.getCommandHandler());

        info() << "Starting remote processor server on port "
               << getConstFrameworkConfiguration()->getServerPort();
        // Start
        if (!_pRemoteProcessorServer->start()) {

            ostringstream oss;
            oss << "ParameterMgr: Unable to start remote processor server on port "
                << getConstFrameworkConfiguration()->getServerPort();
            strError = oss.str();

            return false;
        }
    }

    return true;
}

// Children typwise access
CParameterFrameworkConfiguration* CParameterMgr::getFrameworkConfiguration()
{
    return static_cast<CParameterFrameworkConfiguration*>(getChild(EFrameworkConfiguration));
}

const CParameterFrameworkConfiguration* CParameterMgr::getConstFrameworkConfiguration()
{
    return getFrameworkConfiguration();
}

CSelectionCriteria* CParameterMgr::getSelectionCriteria()
{
    return static_cast<CSelectionCriteria*>(getChild(ESelectionCriteria));
}

const CSelectionCriteria* CParameterMgr::getConstSelectionCriteria()
{
    return static_cast<const CSelectionCriteria*>(getChild(ESelectionCriteria));
}

CSystemClass* CParameterMgr::getSystemClass()
{
    return static_cast<CSystemClass*>(getChild(ESystemClass));
}

const CSystemClass* CParameterMgr::getConstSystemClass() const
{
    return static_cast<const CSystemClass*>(getChild(ESystemClass));
}

// Configurable Domains
CConfigurableDomains* CParameterMgr::getConfigurableDomains()
{
    return static_cast<CConfigurableDomains*>(getChild(EConfigurableDomains));
}

const CConfigurableDomains* CParameterMgr::getConstConfigurableDomains()
{
    return static_cast<const CConfigurableDomains*>(getChild(EConfigurableDomains));
}

const CConfigurableDomains* CParameterMgr::getConstConfigurableDomains() const
{
    return static_cast<const CConfigurableDomains*>(getChild(EConfigurableDomains));
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

    // Reset the modified status of the current criteria to indicate that a new configuration has been applied
    getSelectionCriteria()->resetModifiedStatus();
}

// Export to XML string
bool CParameterMgr::exportElementToXMLString(const IXmlSource* pXmlSource,
                                             const string& strRootElementType,
                                             string& strResult) const
{
    string strError;

    CXmlSerializingContext xmlSerializingContext(strError);

    // Use a doc source by loading data from instantiated Configurable Domains
    CXmlMemoryDocSource memorySource(pXmlSource, strRootElementType, false);

    // Use a doc sink that write the doc data in a string
    CXmlStringDocSink stringSink(strResult);

    // Do the export
    bool bProcessSuccess = stringSink.process(memorySource, xmlSerializingContext);

    if (!bProcessSuccess) {

        strResult = strError;
    }

    return bProcessSuccess;
}

bool CParameterMgr::logResult(bool isSuccess, const std::string& result)
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
