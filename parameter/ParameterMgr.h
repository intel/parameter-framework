/*
 * Copyright (c) 2011-2014, Intel Corporation
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
#include <list>
#include "RemoteCommandHandlerTemplate.h"
#include "PathNavigator.h"
#include "SelectionCriterionType.h"
#include "SelectionCriterion.h"
#include "Element.h"
#include "XmlDocSink.h"
#include "XmlDocSource.h"


class CElementLibrarySet;
class CSubsystemLibrary;
class CSystemClass;
class CSelectionCriteria;
class CParameterFrameworkConfiguration;
class CSystemClassConfiguration;
class CParameterBlackboard;
class CConfigurableDomains;
class IRemoteProcessorServerInterface;
class CBackSynchronizer;
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

    typedef CCommandHandler::CommandStatus (CParameterMgr::*RemoteCommandParser)(const IRemoteCommand& remoteCommand, string& strResult);

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
    static const uint32_t guiEditionMinor = 0x2;
    static const uint32_t guiRevision = 0x0;

    // Parameter handle friendship
    friend class CParameterHandle;
public:
    // Logger interface
    class ILogger
    {
    public:
        virtual void log(bool bIsWarning, const std::string& strLog) = 0;
    protected:
        virtual ~ILogger() {}
    };

    // Construction
    CParameterMgr(const string& strConfigurationFilePath);
    virtual ~CParameterMgr();

    // Logging
    void setLogger(ILogger* pLogger);

    /** Load plugins, structures and settings from the config file given.
      *
      * @param[out] strError is a string describing the error if an error occurred
      *                      undefined otherwise.
      *
      * @return true if no error occurred, false otherwise.
      */
    bool load(string& strError);
    virtual bool init(string& strError);

    // Selection Criteria
    CSelectionCriterionType* createSelectionCriterionType(bool bIsInclusive);
    CSelectionCriterion* createSelectionCriterion(const string& strName, const CSelectionCriterionType* pSelectionCriterionType);
    // Selection criterion retrieval
    CSelectionCriterion* getSelectionCriterion(const string& strName);

    // Configuration application
    void applyConfigurations();

    /**
     * Returns the CConfigurableElement corresponding to the path given in argument.
     *
     * @param[in] strPath A string representing a path to an element.
     * @param[out] strError Error message
     *
     * @return A const pointer to the corresponding CConfigurableElement.
     * On error, NULL is returned and the error is explained in strError.
     */
    const CConfigurableElement* getConfigurableElement(const string& strPath,
                                                       string& strError) const;
    // Dynamic parameter handling
    CParameterHandle* createParameterHandle(const string& strPath, string& strError);

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
    bool setTuningMode(bool bOn, string& strError);
    bool tuningModeOn() const;

    // Current value space for user set/get value interpretation
    void setValueSpace(bool bIsRaw);
    bool valueSpaceIsRaw();

    // Current Output Raw Format for user get value interpretation
    void setOutputRawFormat(bool bIsHex);
    bool outputRawFormatIsHex();

    // Automatic hardware synchronization control (during tuning session)
    bool setAutoSync(bool bAutoSyncOn, string& strError);
    bool autoSyncOn() const;
    bool sync(string& strError);

    // User set/get parameters
    bool accessValue(CParameterAccessContext& parameterAccessContext, const string& strPath, string& strValue, bool bSet, string& strError);
    bool accessParameterValue(const string& strPath, string& strValue, bool bSet, string& strError);
    /**
     * Returns the element mapping corresponding to the path given in parameter.
     *
     * @param[in] strPath Path of an element
     * @param[out] strValue A sting containing the mapping
     *
     * @return true if a mapping was found for this element
     */
    bool getParameterMapping(const string& strPath, string& strValue) const;
    bool accessConfigurationValue(const string &strDomain, const string &stConfiguration, const string& strPath, string& strValue, bool bSet, string& strError);

    ////////// Configuration/Domains handling //////////////
    // Creation/Deletion
    bool createDomain(const string& strName, string& strError);
    bool deleteDomain(const string& strName, string& strError);
    bool deleteAllDomains(string& strError);
    bool createConfiguration(const string& strDomain, const string& strConfiguration, string& strError);
    bool deleteConfiguration(const string& strDomain, const string& strConfiguration, string& strError);

    // Save/Restore
    bool restoreConfiguration(const string& strDomain, const string& strConfiguration, list<string>& strError);
    bool saveConfiguration(const string& strDomain, const string& strConfiguration, string& strError);

    // Configurable element - domain association
    bool addConfigurableElementToDomain(const string& strDomain, const string& strConfigurableElementPath, string& strError);
    bool removeConfigurableElementFromDomain(const string& strDomain, const string& strConfigurableElementPath, string& strError);
    bool split(const string& strDomain, const string& strConfigurableElementPath, string& strError);

    /**
      * Method that imports Configurable Domains from an Xml source.
      *
      * @param[in] strXmlSource a string containing an xml description or a path to an xml file
      * @param[in] bWithSettings a boolean that determines if the settings should be used in the
      * xml description
      * @param[in] bFromFile a boolean that determines if the source is an xml description in
      * strXmlSource or contained in a file. In that case strXmlSource is just the file path.
      * @param[out] strError is used as the error output
      *
      * @return false if any error occures
      */
    bool importDomainsXml(const string& strXmlSource, bool bWithSettings, bool bFromFile,
                          string& strError);

    /**
      * Method that exports Configurable Domains to an Xml destination.
      * If bToFile is false, the xml description from the xml document will be written
      * in strXmlDest. Otherwise it will be written in a file located at the path in strXmlDest
      *
      * @param[in:out] strXmlDest a string containing an xml description or a path to an xml file
      * @param[in] bWithSettings a boolean that determines if the settings should be used in the
      * xml description
      * @param[in] bToFile a boolean that determines if the destination is an xml description in
      * strXmlDest or contained in a file. In that case strXmlDest is just the file path.
      * @param[out] strError is used as the error output
      *
      * @return false if any error occures
      */
    bool exportDomainsXml(string& strXmlDest, bool bWithSettings, bool bToFile,
                          string& strError) const;

    // Binary Import/Export
    bool importDomainsBinary(const string& strFileName, string& strError);
    bool exportDomainsBinary(const string& strFileName, string& strError);

    /**
      * Method that creates an Xml description of the instanciated parameter structure contained
      * in SystemClass.
      *
      * @param[out] strResult contains the xml description of SystemClass or the errors if any
      *
      * @return false if any error occures during the creation of the xml description
      */
    bool getSystemClassXMLString(string& strResult);

    // Introspect
    void logStructureContent(string& strContent) const;

    // CElement
    virtual string getKind() const;

private:
    CParameterMgr(const CParameterMgr&);
    CParameterMgr& operator=(const CParameterMgr&);

    // Logging (done by root)
    virtual void doLog(bool bIsWarning, const string& strLog) const;
    virtual void nestLog() const;
    virtual void unnestLog() const;

    // Version
    string getVersion() const;

    ////////////////:: Remote command parsers
    /// Version
    CCommandHandler::CommandStatus versionCommandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Status
    CCommandHandler::CommandStatus statusCommandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Tuning Mode
    CCommandHandler::CommandStatus setTuningModeCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus getTuningModeCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Value Space
    CCommandHandler::CommandStatus setValueSpaceCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus getValueSpaceCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Output Raw Format
    CCommandHandler::CommandStatus setOutputRawFormatCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus getOutputRawFormatCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Sync
    CCommandHandler::CommandStatus setAutoSyncCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus getAutoSyncCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus syncCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Criteria
    CCommandHandler::CommandStatus listCriteriaCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Domains
    CCommandHandler::CommandStatus listDomainsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus createDomainCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus deleteDomainCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus deleteAllDomainsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus renameDomainCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus setSequenceAwarenessCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus getSequenceAwarenessCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus listDomainElementsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus addElementCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus removeElementCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus splitDomainCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Configurations
    CCommandHandler::CommandStatus listConfigurationsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus dumpDomainsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus createConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus deleteConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus renameConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus saveConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus restoreConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus setElementSequenceCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus getElementSequenceCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus setRuleCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus clearRuleCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus getRuleCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Elements/Parameters
    CCommandHandler::CommandStatus listElementsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus listParametersCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus dumpElementCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus getElementSizeCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus showPropertiesCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus getParameterCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus setParameterCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus getConfigurationParameterCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus setConfigurationParameterCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus listBelongingDomainsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus listAssociatedDomainsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus showMappingCommmandProcess(const IRemoteCommand& remoteCommand,
                                                              string& strResult);
    /// Browse
    CCommandHandler::CommandStatus listAssociatedElementsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus listConflictingElementsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus listRogueElementsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Settings Import/Export
    CCommandHandler::CommandStatus exportConfigurableDomainsToXMLCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus importConfigurableDomainsFromXMLCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus exportConfigurableDomainsWithSettingsToXMLCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus importConfigurableDomainsWithSettingsFromXMLCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus exportSettingsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus importSettingsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);

    /**
      * Command handler method for getConfigurableDomainWithSettings command.
      *
      * @param[in] remoteCommand contains the arguments of the received command.
      * @param[out] strResult a string containing the result of the command
      *
      * @return CCommandHandler::ESucceeded if command succeeded or CCommandHandler::EFailed
      * in the other case
      */
    CCommandHandler::CommandStatus getConfigurableDomainsWithSettingsXMLCommmandProcess(
            const IRemoteCommand& remoteCommand, string& strResult);

    /**
      * Command handler method for setConfigurableDomainWithSettings command.
      *
      * @param[in] remoteCommand contains the arguments of the received command.
      * @param[out] strResult a string containing the result of the command
      *
      * @return CCommandHandler::ESucceeded if command succeeded or CCommandHandler::EFailed
      * in the other case
      */
    CCommandHandler::CommandStatus setConfigurableDomainsWithSettingsXMLCommmandProcess(
            const IRemoteCommand& remoteCommand, string& strResult);

    /**
      * Command handler method for getSystemClass command.
      *
      * @param[in] remoteCommand contains the arguments of the received command.
      * @param[out] strResult a string containing the result of the command
      *
      * @return CCommandHandler::ESucceeded if command succeeded or CCommandHandler::EFailed
      * in the other case
      */
    CCommandHandler::CommandStatus getSystemClassXMLCommmandProcess(
            const IRemoteCommand& remoteCommand, string& strResult);

    // Max command usage length, use for formatting
    void setMaxCommandUsageLength();

    // For tuning, check we're in tuning mode
    bool checkTuningModeOn(string& strError) const;

    // Blackboard (dynamic parameter handling)
    pthread_mutex_t* getBlackboardMutex();

    // Blackboard reference (dynamic parameter handling)
    CParameterBlackboard* getParameterBlackboard();

    // Parameter access
    bool doSetValue(const string& strPath, const string& strValue, bool bRawValueSpace, bool bDynamicAccess, string& strError) const;
    bool doGetValue(const string& strPath, string& strValue, bool bRawValueSpace, bool bHexOutputRawFormat, bool bDynamicAccess, string& strError) const;

    // Framework global configuration loading
    bool loadFrameworkConfiguration(string& strError);

    // System class Structure loading
    bool loadStructure(string& strError);

    // System class Structure loading
    bool loadSettings(string& strError);
    bool loadSettingsFromConfigFile(string& strError);

    // Parse XML file into Root element
    bool xmlParse(CXmlElementSerializingContext& elementSerializingContext, CElement* pRootElement, const string& strXmlFilePath, const string& strXmlFolder, ElementLibrary eElementLibrary, const string& strNameAttrituteName = "Name");

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
    bool handleRemoteProcessingInterface(string& strError);

    // Back synchronization
    CBackSynchronizer* createBackSynchronizer() const;

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
    string _strXmlConfigurationFilePath; // Configuration file path
    string _strXmlConfigurationFolderPath; // Root folder for configuration file
    string _strSchemaFolderLocation; // Place where schemas stand

    // Subsystem plugin location
    const CSubsystemPlugins* _pSubsystemPlugins;

    // Remote processor library handle
    void *_handleLibRemoteProcessor;

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

    // Logging
    ILogger* _pLogger;
    mutable uint32_t _uiLogDepth;

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

