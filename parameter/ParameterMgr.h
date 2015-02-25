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
#pragma once

#include <pthread.h>
#include <map>
#include <vector>
#include "RemoteCommandHandlerTemplate.h"
#include "PathNavigator.h"
#include "SelectionCriterionType.h"
#include "SelectionCriterion.h"
#include "Element.h"
#include "XmlDocSink.h"
#include "XmlDocSource.h"
#include "Results.h"
#include <log/LogWrapper.h>
#include <log/Context.h>

#include <string>
#include <memory>

class CElementLibrarySet;
class CSubsystemLibrary;
class CSystemClass;
class CSelectionCriteria;
class CParameterFrameworkConfiguration;
class CSystemClassConfiguration;
class CParameterBlackboard;
class CConfigurableDomains;
class IRemoteProcessorServerInterface;
class CParameterHandle;
class CSubsystemPlugins;
class CParameterAccessContext;
class CConfigurableElement;

class CParameterMgr : private CElement
{
    enum ChildElement {
        EFrameworkConfiguration,
        ESelectionCriteria,
        ESystemClass,
        EConfigurableDomains
    };
    enum ElementLibrary {
        EFrameworkConfigurationLibrary,
        EParameterCreationLibrary,
        EParameterConfigurationLibrary
    };

    // Remote command parsers
    typedef TRemoteCommandHandlerTemplate<CParameterMgr> CCommandHandler;

    typedef CCommandHandler::CommandStatus (CParameterMgr::*RemoteCommandParser)(const IRemoteCommand& remoteCommand, std::string& strResult);

    // Parser descriptions
    struct SRemoteCommandParserItem
    {
        const char* _pcCommandName;
        CParameterMgr::RemoteCommandParser _pfnParser;
        uint32_t _uiMinArgumentCount;
        const char* _pcHelp;
        const char* _pcDescription;
    };
    // Version
    static const uint32_t guiEditionMajor = 0x2;
    static const uint32_t guiEditionMinor = 0x4;
    static const uint32_t guiRevision = 0x3;

    // Parameter handle friendship
    friend class CParameterHandle;
public:

    // Construction
    CParameterMgr(const std::string& strConfigurationFilePath, core::log::ILogger& logger);
    virtual ~CParameterMgr();

    /** Load plugins, structures and settings from the config file given.
      *
      * @param[out] strError is a std::string describing the error if an error occurred
      *                      undefined otherwise.
      *
      * @return true if no error occurred, false otherwise.
      */
    bool load(std::string& strError);

    // Selection Criteria
    CSelectionCriterionType* createSelectionCriterionType(bool bIsInclusive);
    CSelectionCriterion* createSelectionCriterion(const std::string& strName, const CSelectionCriterionType* pSelectionCriterionType);
    // Selection criterion retrieval
    CSelectionCriterion* getSelectionCriterion(const std::string& strName);

    // Configuration application
    void applyConfigurations();

    /**
     * Returns the CConfigurableElement corresponding to the path given in argument.
     *
     * @param[in] strPath A std::string representing a path to an element.
     * @param[out] strError Error message
     *
     * @return A const pointer to the corresponding CConfigurableElement.
     * On error, NULL is returned and the error is explained in strError.
     */
    const CConfigurableElement* getConfigurableElement(const std::string& strPath,
                                                       std::string& strError) const;
    // Dynamic parameter handling
    CParameterHandle* createParameterHandle(const std::string& strPath, std::string& strError);

    /** Is the remote interface forcefully disabled ?
     */
    bool getForceNoRemoteInterface() const;

    /**
     * Forcefully disable the remote interface or cancel this policy
     *
     * @param[in] bForceNoRemoteInterface disable the remote interface if true.
     */
    void setForceNoRemoteInterface(bool bForceNoRemoteInterface);

    /** Should start fail in case of missing subsystems.
      *
      * @param[in] bFail: If set to true,  parameterMgr start will fail on missing subsystems.
      *                   If set to false, missing subsystems will fallback on virtual subsystem.
      */
    void setFailureOnMissingSubsystem(bool bFail);

    /** Would start fail in case of missing subsystems.
      *
      * @return true if the subsystem will fail on missing subsystem, false otherwise.
      */
    bool getFailureOnMissingSubsystem() const;

    /** Should start fail in failed settings load.
      *
      * @param[in] bFail: If set to true, parameterMgr start will fail on failed settings load.
      *                   If set to false, failed settings load will be ignored.
      */
    void setFailureOnFailedSettingsLoad(bool bFail);
    /** Would start fail in case of failed settings load.
      *
      * @return failure on failed settings load policy state.
      */
    bool getFailureOnFailedSettingsLoad();

    /** Get the path to the directory containing the XML Schemas
     *
     * @returns the directory containing the XML Schemas
     */
    const std::string& getSchemaFolderLocation() const;

    /** Override the directory containing the XML Schemas
     *
     * @param[in] strSchemaFolderLocation directory containing the XML Schemas
     */
    void setSchemaFolderLocation(const std::string& strSchemaFolderLocation);

    /** Should .xml files be validated on start ?
     *
     * @param[in] bValidate:
     *     If set to true, parameterMgr will report an error
     *         when being unable to validate .xml files
     *     If set to false, no .xml/xsd validation will happen
     *     (default behaviour)
     *
     * @return false if unable to set, true otherwise.
     */
    void setValidateSchemasOnStart(bool bValidate);

    /** Would .xml files be validated on start?
     *
     * @return areSchemasValidated
     */
    bool getValidateSchemasOnStart() const;

    //////////// Tuning /////////////
    // Tuning mode
    bool setTuningMode(bool bOn, std::string& strError);
    bool tuningModeOn() const;

    // Current value space for user set/get value interpretation
    void setValueSpace(bool bIsRaw);
    bool valueSpaceIsRaw();

    // Current Output Raw Format for user get value interpretation
    void setOutputRawFormat(bool bIsHex);
    bool outputRawFormatIsHex();

    // Automatic hardware synchronization control (during tuning session)
    bool setAutoSync(bool bAutoSyncOn, std::string& strError);
    bool autoSyncOn() const;
    bool sync(std::string& strError);

    // User set/get parameters
    bool accessParameterValue(const std::string& strPath, std::string& strValue, bool bSet, std::string& strError);
    /**
     * Returns the element mapping corresponding to the path given in parameter.
     *
     * @param[in] strPath Path of an element
     * @param[out] strValue A sting containing the mapping
     *
     * @return true if a mapping was found for this element
     */
    bool getParameterMapping(const std::string& strPath, std::string& strValue) const;
    bool accessConfigurationValue(const std::string &strDomain, const std::string &stConfiguration, const std::string& strPath, std::string& strValue, bool bSet, std::string& strError);

    ////////// Configuration/Domains handling //////////////
    // Creation/Deletion
    bool createDomain(const std::string& strName, std::string& strError);
    bool renameDomain(const std::string& strName, const std::string& strNewName,
                      std::string& strError);
    bool deleteDomain(const std::string& strName, std::string& strError);
    bool deleteAllDomains(std::string& strError);
    bool setSequenceAwareness(const std::string& strName, bool bSequenceAware,
                              std::string& strResult);
    bool getSequenceAwareness(const std::string& strName, bool& bSequenceAware,
                              std::string& strResult);
    bool createConfiguration(const std::string& strDomain, const std::string& strConfiguration, std::string& strError);
    bool deleteConfiguration(const std::string& strDomain, const std::string& strConfiguration, std::string& strError);
    bool renameConfiguration(const std::string& strDomain, const std::string& strConfiguration, const std::string& strNewConfiguration, std::string& strError);

    /** Restore a configuration
     *
     * @param[in] strDomain the domain name
     * @param[in] strConfiguration the configuration name
     * @param[out] errors errors encountered during restoration
     * @return true if success false otherwise
     */
    bool restoreConfiguration(const std::string& strDomain,
                              const std::string& strConfiguration,
                              core::Results& errors);

    bool saveConfiguration(const std::string& strDomain, const std::string& strConfiguration, std::string& strError);

    // Configurable element - domain association
    bool addConfigurableElementToDomain(const std::string& strDomain, const std::string& strConfigurableElementPath, std::string& strError);
    bool removeConfigurableElementFromDomain(const std::string& strDomain, const std::string& strConfigurableElementPath, std::string& strError);
    bool split(const std::string& strDomain, const std::string& strConfigurableElementPath, std::string& strError);
    bool setElementSequence(const std::string& strDomain, const std::string& strConfiguration,
                            const std::vector<std::string>& astrNewElementSequence,
                            std::string& strError);

    bool getApplicationRule(const std::string& strDomain, const std::string& strConfiguration,
                            std::string& strResult);
    bool setApplicationRule(const std::string& strDomain, const std::string& strConfiguration,
                            const std::string& strApplicationRule, std::string& strError);
    bool clearApplicationRule(const std::string& strDomain, const std::string& strConfiguration,
                              std::string& strError);

    /**
      * Method that imports Configurable Domains from an Xml source.
      *
      * @param[in] strXmlSource a std::string containing an xml description or a path to an xml file
      * @param[in] bWithSettings a boolean that determines if the settings should be used in the
      * xml description
      * @param[in] bFromFile a boolean that determines if the source is an xml description in
      * strXmlSource or contained in a file. In that case strXmlSource is just the file path.
      * @param[out] strError is used as the error output
      *
      * @return false if any error occures
      */
    bool importDomainsXml(const std::string& strXmlSource, bool bWithSettings, bool bFromFile,
                          std::string& strError);

    /**
      * Method that imports a single Configurable Domain from an Xml source.
      *
      * @param[in] strXmlSource a string containing an xml description or a path to an xml file
      * @param[in] bWithSettings a boolean that determines if the settings should be used in the
      * xml description
      * @param[in] bFromFile a boolean that determines if the source is an xml description in
      * strXmlSource or contained in a file. In that case strXmlSource is just the file path.
      * @param[out] strError is used as the error output
      *
      * @return false if any error occurs
      */
    bool importSingleDomainXml(const std::string& strXmlSource, bool bOverwrite,
                               std::string& strError);

    /**
      * Method that exports Configurable Domains to an Xml destination.
      *
      * @param[in,out] strXmlDest a string containing an xml description or a path to an xml file
      * @param[in] bWithSettings a boolean that determines if the settings should be used in the
      * xml description
      * @param[in] bToFile a boolean that determines if the destination is an xml description in
      * strXmlDest or contained in a file. In that case strXmlDest is just the file path.
      * @param[out] strError is used as the error output
      *
      * @return false if any error occurs, true otherwise.
      */
    bool exportDomainsXml(std::string& strXmlDest, bool bWithSettings, bool bToFile,
                          std::string& strError) const;

    /**
      * Method that exports a given Configurable Domain to an Xml destination.
      *
      * @param[in,out] strXmlDest a string containing an xml description or a path to an xml file
      * @param[in] strDomainName the name of the domain to be exported
      * @param[in] bWithSettings a boolean that determines if the settings should be used in the
      * xml description
      * @param[in] bToFile a boolean that determines if the destination is an xml description in
      * strXmlDest or contained in a file. In that case strXmlDest is just the file path.
      * @param[out] strError is used as the error output
      *
      * @return false if any error occurs, true otherwise.
      */
    bool exportSingleDomainXml(std::string& strXmlDest, const std::string& strDomainName,
                               bool bWithSettings, bool bToFile, std::string& strError) const;

    // Binary Import/Export
    bool importDomainsBinary(const std::string& strFileName, std::string& strError);
    bool exportDomainsBinary(const std::string& strFileName, std::string& strError);

    /**
      * Method that exports an Xml description of the passed element into a string
      *
      * @param[in] pXmlSource The source element to export
      * @param[in] strRootElementType The XML root element name of the exported instance document
      * @param[out] strResult contains the xml description or the error description in case false is returned
      *
      * @return true for success, false if any error occurs during the creation of the xml description (validation or encoding)
      */
    bool exportElementToXMLString(const IXmlSource* pXmlSource,
                                  const std::string& strRootElementType,
                                  std::string& strResult) const;

    // CElement
    virtual std::string getKind() const;

private:
    CParameterMgr(const CParameterMgr&);
    CParameterMgr& operator=(const CParameterMgr&);

    // Init
    virtual bool init(std::string& strError);

    // Version
    std::string getVersion() const;

    ////////////////:: Remote command parsers
    /// Version
    CCommandHandler::CommandStatus versionCommandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    /// Status
    CCommandHandler::CommandStatus statusCommandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    /// Tuning Mode
    CCommandHandler::CommandStatus setTuningModeCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus getTuningModeCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    /// Value Space
    CCommandHandler::CommandStatus setValueSpaceCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus getValueSpaceCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    /// Output Raw Format
    CCommandHandler::CommandStatus setOutputRawFormatCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus getOutputRawFormatCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    /// Sync
    CCommandHandler::CommandStatus setAutoSyncCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus getAutoSyncCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus syncCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    /// Criteria
    CCommandHandler::CommandStatus listCriteriaCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    /// Domains
    CCommandHandler::CommandStatus listDomainsCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus createDomainCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus deleteDomainCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus deleteAllDomainsCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus renameDomainCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus setSequenceAwarenessCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus getSequenceAwarenessCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus listDomainElementsCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus addElementCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus removeElementCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus splitDomainCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    /// Configurations
    CCommandHandler::CommandStatus listConfigurationsCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus dumpDomainsCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus createConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus deleteConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus renameConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus saveConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus restoreConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus setElementSequenceCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus getElementSequenceCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus setRuleCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus clearRuleCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus getRuleCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    /// Elements/Parameters
    CCommandHandler::CommandStatus listElementsCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus listParametersCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus dumpElementCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus getElementSizeCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus showPropertiesCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus getParameterCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus setParameterCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus getConfigurationParameterCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus setConfigurationParameterCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus listBelongingDomainsCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus listAssociatedDomainsCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus showMappingCommmandProcess(const IRemoteCommand& remoteCommand,
                                                              std::string& strResult);
    /// Browse
    CCommandHandler::CommandStatus listAssociatedElementsCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus listConflictingElementsCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus listRogueElementsCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    /// Settings Import/Export
    CCommandHandler::CommandStatus exportConfigurableDomainsToXMLCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus importConfigurableDomainsFromXMLCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus exportConfigurableDomainsWithSettingsToXMLCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus importConfigurableDomainsWithSettingsFromXMLCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus importConfigurableDomainWithSettingsFromXMLCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus exportSettingsCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);
    CCommandHandler::CommandStatus importSettingsCommmandProcess(const IRemoteCommand& remoteCommand, std::string& strResult);

    /**
      * Command handler method for getConfigurableDomainsWithSettings command.
      *
      * @param[in] remoteCommand contains the arguments of the received command.
      * @param[out] strResult a std::string containing the result of the command
      *
      * @return CCommandHandler::ESucceeded if command succeeded or CCommandHandler::EFailed
      * in the other case
      */
    CCommandHandler::CommandStatus getConfigurableDomainsWithSettingsXMLCommmandProcess(
            const IRemoteCommand& remoteCommand, std::string& strResult);

    /**
      * Command handler method for getConfigurableDomainWithSettings command.
      *
      * @param[in] remoteCommand contains the arguments of the received command.
      * @param[out] strResult a string containing the result of the command
      *
      * @return CCommandHandler::ESucceeded if command succeeded or CCommandHandler::EFailed
      * in the other case
      */
    CCommandHandler::CommandStatus getConfigurableDomainWithSettingsXMLCommmandProcess(
            const IRemoteCommand& remoteCommand, std::string& strResult);

    /**
      * Command handler method for setConfigurableDomainWithSettings command.
      *
      * @param[in] remoteCommand contains the arguments of the received command.
      * @param[out] strResult a std::string containing the result of the command
      *
      * @return CCommandHandler::ESucceeded if command succeeded or CCommandHandler::EFailed
      * in the other case
      */
    CCommandHandler::CommandStatus setConfigurableDomainsWithSettingsXMLCommmandProcess(
            const IRemoteCommand& remoteCommand, std::string& strResult);

    /**
      * Command handler method for getSystemClass command.
      *
      * @param[in] remoteCommand contains the arguments of the received command.
      * @param[out] strResult a std::string containing the result of the command
      *
      * @return CCommandHandler::ESucceeded if command succeeded or CCommandHandler::EFailed
      * in the other case
      */
    CCommandHandler::CommandStatus getSystemClassXMLCommmandProcess(
            const IRemoteCommand& remoteCommand, std::string& strResult);

    // Max command usage length, use for formatting
    void setMaxCommandUsageLength();

    // For tuning, check we're in tuning mode
    bool checkTuningModeOn(std::string& strError) const;

    // Blackboard (dynamic parameter handling)
    pthread_mutex_t* getBlackboardMutex();

    // Blackboard reference (dynamic parameter handling)
    CParameterBlackboard* getParameterBlackboard();

    // Parameter access
    bool accessValue(CParameterAccessContext& parameterAccessContext, const std::string& strPath, std::string& strValue, bool bSet, std::string& strError);
    bool doSetValue(const std::string& strPath, const std::string& strValue, bool bRawValueSpace, bool bDynamicAccess, std::string& strError) const;
    bool doGetValue(const std::string& strPath, std::string& strValue, bool bRawValueSpace, bool bHexOutputRawFormat, bool bDynamicAccess, std::string& strError) const;

    // Framework global configuration loading
    bool loadFrameworkConfiguration(std::string& strError);

    /** Load required subsystems
     *
     * @param[out] error error description if there is one
     * @return true if succeed false otherwise
     */
    bool loadSubsystems(std::string& error);

    // System class Structure loading
    bool loadStructure(std::string& strError);

    // System class Structure loading
    bool loadSettings(std::string& strError);
    bool loadSettingsFromConfigFile(std::string& strError);

    // Parse XML file into Root element
    bool xmlParse(CXmlElementSerializingContext& elementSerializingContext, CElement* pRootElement, const std::string& strXmlFilePath, const std::string& strXmlFolder, ElementLibrary eElementLibrary, const std::string& strNameAttrituteName = "Name");

    /**
     * Export an element object to an Xml destination.
     *
     *
     * @param[in,out] strXmlDest a string containing an xml description or a path to an xml file.
     * @param[in] xmlSerializingContext the serializing context
     * @param[in] bToFile a boolean that determines if the destination is an xml description in
     * strXmlDest or contained in a file. In that case strXmlDest is just the file path.
     * @param[in] element object to be serialized.
     * @param[out] strError is used as the error output.
     *
     * @return false if any error occurs, true otherwise.
     */
    bool serializeElement(std::string& strXmlDest, CXmlSerializingContext& xmlSerializingContext,
                          bool bToFile, const CElement& element, std::string& strError) const;

    /**
      * Method that imports a single Configurable Domain, with settings, from an Xml file.
      *
      * @param[in] strXmlFilePath absolute path to the xml file containing the domain
      * @param[out] strError is used as the error output
      *
      * @return false if any error occurs
      */
    bool importDomainFromFile(const std::string& strXmlFilePath, bool bOverwrite,
                              std::string& strError);


    // Framework Configuration
    CParameterFrameworkConfiguration* getFrameworkConfiguration();
    const CParameterFrameworkConfiguration* getConstFrameworkConfiguration();

    // Selection Criteria
    CSelectionCriteria* getSelectionCriteria();
    const CSelectionCriteria* getConstSelectionCriteria();

    // System Class
    CSystemClass* getSystemClass();
    const CSystemClass* getConstSystemClass() const;

    // Configurable Domains
    CConfigurableDomains* getConfigurableDomains();
    const CConfigurableDomains* getConstConfigurableDomains();
    const CConfigurableDomains* getConstConfigurableDomains() const;

    // Apply configurations
    void doApplyConfigurations(bool bForce);

    // Dynamic object creation libraries feeding
    void feedElementLibraries();

    // Remote Processor Server connection handling
    bool handleRemoteProcessingInterface(std::string& strError);

    /** Log the result of a function
     *
     * @param[in] isSuccess indicates if the previous function has succeed
     * @param[in] result function provided result string
     * @return isSuccess parameter
     */
    bool logResult(bool isSuccess, const std::string& result);

    /** Info logger call helper */
    inline core::log::details::Info info();

    /** Warning logger call helper */
    inline core::log::details::Warning warning();

    // Tuning
    bool _bTuningModeIsOn;

    // Value Space
    bool _bValueSpaceIsRaw;

    // Output Raw Format
    bool _bOutputRawFormatIsHex;

    // Automatic synchronization to HW during Tuning session
    bool _bAutoSyncOn;

    // Current Parameter Settings
    CParameterBlackboard* _pMainParameterBlackboard;

    // Dynamic object creation
    CElementLibrarySet* _pElementLibrarySet;

    // XML parsing, object creation handling
    std::string _strXmlConfigurationFilePath; // Configuration file path
    std::string _strXmlConfigurationFolderPath; // Root folder for configuration file
    std::string _strSchemaFolderLocation; // Place where schemas stand

    // Subsystem plugin location
    const CSubsystemPlugins* _pSubsystemPlugins;

    /**
     * Remote processor library handle
     */
    void* _pvLibRemoteProcessorHandle;

    // Whole system structure checksum
    uint8_t _uiStructureChecksum;

    // Command Handler
    CCommandHandler* _pCommandHandler;

    // Remote Processor Server
    IRemoteProcessorServerInterface* _pRemoteProcessorServer;

    // Parser description array
    static const SRemoteCommandParserItem gastRemoteCommandParserItems[];

    // Parser description array size
    static const uint32_t guiNbRemoteCommandParserItems;

    // Maximum command usage length
    uint32_t _uiMaxCommandUsageLength;

    // Blackboard access mutex
    pthread_mutex_t _blackboardMutex;

    /** Application main logger based on the one provided by the client */
    core::log::Logger _logger;

    /** If set to false, the remote interface won't be started no matter what.
     * If set to true - the default - it has no impact on the policy for
     * starting the remote interface.
     */
    bool _bForceNoRemoteInterface;

    /** If set to true, missing subsystem will abort parameterMgr start.
      * If set to false, missing subsystem will fallback on virtual subsystem.
      */
    bool _bFailOnMissingSubsystem;
    /** If set to true, unparsable or discording domains will abort parameterMgr start.
      * If set to false, unparsable or discording domains
      *                 will continue the parameterMgr start with no domains.
      */
    bool _bFailOnFailedSettingsLoad;

    /**
     * If set to true, parameterMgr will report an error
     *     when being unable to validate .xml files
     * If set to false, no .xml/xsd validation will happen (default behaviour)
     */
    bool _bValidateSchemasOnStart;
};

