/* 
 * INTEL CONFIDENTIAL
 * Copyright © 2011 Intel 
 * Corporation All Rights Reserved.
 * 
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel’s prior express written permission.
 * 
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 * 
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
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
    static const uint32_t guiEditionMinor = 0x1;
    static const uint32_t guiRevision = 0x0;

    // Parameter handle friendship
    friend class CParameterHandle;
public:
    // Logger interface
    class ILogger
    {
    public:
        virtual void log(bool bIsWarning, const std::string& strLog) = 0;
    };

    // Construction
    CParameterMgr(const string& strConfigurationFilePath);
    virtual ~CParameterMgr();

    // Logging
    void setLogger(ILogger* pLogger);

    // Init
    bool load(string& strError);
    virtual bool init(string& strError);

    // Selection Criteria
    CSelectionCriterionType* createSelectionCriterionType(bool bIsInclusive);
    CSelectionCriterion* createSelectionCriterion(const string& strName, const CSelectionCriterionType* pSelectionCriterionType);
    // Selection criterion retrieval
    CSelectionCriterion* getSelectionCriterion(const string& strName);

    // Configuration application
    void applyConfigurations();

    // Dynamic parameter handling
    CParameterHandle* createParameterHandle(const string& strPath, string& strError);

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
    bool accessValue(const string& strPath, string& strValue, bool bSet, string& strError);

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

    // XML Import/Export
    bool importDomainsXml(const string& strFileName, bool bWithSettings, string& strError);
    bool exportDomainsXml(const string& strFileName, bool bWithSettings, string& strError) const;

    // Binary Import/Export
    bool importDomainsBinary(const string& strFileName, string& strError);
    bool exportDomainsBinary(const string& strFileName, string& strError);

    // GUI command XML send
    bool getDomainsXMLString(string& strResult, bool bWithSettings);
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
    CCommandHandler::CommandStatus listBelongingDomainsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus listAssociatedDomainsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
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
    /// GUI commands
    CCommandHandler::CommandStatus getSystemClassXMLCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CCommandHandler::CommandStatus getDomainsXMLCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);

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
};

