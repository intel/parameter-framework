/* <auto_header>
 * <FILENAME>
 * 
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
 *  AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 * 
 * 
 * </auto_header>
 */
#pragma once

#include <pthread.h>
#include "RemoteCommandHandler.h"
#include "PathNavigator.h"
#include "SelectionCriterionType.h"
#include "SelectionCriterion.h"
#include "Element.h"
#include "SelectionCriterionObserver.h"
#include <map>
#include <vector>

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

class CParameterMgr : private CElement, private IRemoteCommandHandler, private ISelectionCriterionObserver
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
    // Remote command execution status
    enum CommandStatus {
        EDone,
        ESucceeded,
        EFailed,
        EShowUsgae
    };

    // Remote command parsers
    typedef CommandStatus (CParameterMgr::*RemoteCommandParser)(const IRemoteCommand& remoteCommand, string& strResult);

    // Parser descriptions
    struct SRemoteCommandParserItem
    {
        const char* _pcCommandName;
        CParameterMgr::RemoteCommandParser _pfnParser;
        uint32_t _uiMinArgumentCount;
        const char* _pcHelp;
        const char* _pcDescription;

        // Usage
        string usage() const
        {
            return string(_pcCommandName) + " " + _pcHelp;
        }
    };
public:
    // Logger interface
    class ILogger
    {
    public:
        virtual void log(const std::string& strLog) = 0;
    };

    // Construction
    CParameterMgr(const string& strParameterFrameworkConfigurationFolderPath, const string& strSystemClassName);
    virtual ~CParameterMgr();

    // Logging
    void setLogger(ILogger* pLogger);

    // Init
    bool load(string& strError);
    virtual bool init(string& strError);

    // Selection Criteria
    CSelectionCriterionType* createSelectionCriterionType(bool bIsInclusive);
    CSelectionCriterion* createSelectionCriterion(const string& strName, const CSelectionCriterionType* pSelectionCriterionType);

    //////////// Tuning /////////////
    // Tuning mode
    bool setTuningMode(bool bOn, string& strError);
    bool tuningModeOn() const;

    // Current value space for user set/get value interpretation
    void setValueSpace(bool bIsRaw);
    bool valueSpaceIsRaw();

    // Automatic hardware synchronization control (during tuning session)
    bool setAutoSync(bool bAutoSyncOn, string& strError);
    bool autoSyncOn() const;
    bool sync(string& strError);

    // User set/get parameters
    bool setValue(const string& strPath, const string& strValue, string& strError);
    bool getValue(const string& strPath, string& strValue, string& strError) const;

    ////////// Configuration/Domains handling //////////////
    // Creation/Deletion
    bool createDomain(const string& strName, string& strError);
    bool deleteDomain(const string& strName, string& strError);
    bool createConfiguration(const string& strDomain, const string& strConfiguration, string& strError);
    bool deleteConfiguration(const string& strDomain, const string& strConfiguration, string& strError);

    // Save/Restore
    bool restoreConfiguration(const string& strDomain, const string& strConfiguration, string& strError);
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

    // Introspect
    void logStructureContent(string& strContent) const;

    // CElement
    virtual string getKind() const;
private:
    // Logging (done by root)
    virtual void doLog(const string& strLog) const;
    virtual void nestLog() const;
    virtual void unnestLog() const;

    // From ISelectionCriterionObserver: selection criteria changed event
    virtual void selectionCriterionChanged(const CSelectionCriterion* pSelectionCriterion);

    // From IRemoteCommandHandler: return true on success, fill result in any cases
    virtual bool remoteCommandProcess(const IRemoteCommand& remoteCommand, string& strResult);

    ////////////////:: Remote command parsers
    /// Help
    CommandStatus helpCommandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Status
    CommandStatus statusCommandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Tuning Mode
    CommandStatus setTuningModeCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus getTuningModeCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Value Space
    CommandStatus setValueSpaceCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus getValueSpaceCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Sync
    CommandStatus setAutoSyncCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus getAutoSyncCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus syncCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Domains
    CommandStatus listDomainsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus createDomainCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus deleteDomainCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus renameDomainCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus listDomainElementsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus addElementCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus removeElementCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus splitDomainCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Configurations
    CommandStatus listConfigurationsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus createConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus deleteConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus renameConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus saveConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus restoreConfigurationCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Elements/Parameters
    CommandStatus listElementsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus listElementsRecursiveCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus dumpElementCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus getElementSizeCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus getParameterCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus setParameterCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus listBelongingDomainsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus listAssociatedDomainsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Browse
    CommandStatus listAssociatedElementsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus listConflictingElementsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus listRogueElementsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    /// Settings Import/Export
    CommandStatus exportConfigurableDomainsToXMLCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus importConfigurableDomainsFromXMLCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus exportConfigurableDomainsWithSettingsToXMLCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus importConfigurableDomainsWithSettingsFromXMLCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus exportSettingsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);
    CommandStatus importSettingsCommmandProcess(const IRemoteCommand& remoteCommand, string& strResult);

    // Max command usage length, use for formatting
    void setMaxCommandUsageLength();

    // For tuning, check we're in tuning mode
    bool checkTuningModeOn(string& strError) const;

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
    CBackSynchronizer* createBackSynchronizer(string& strError) const;

    // Tuning
    bool _bTuningModeIsOn;

    // Value Space
    bool _bValueSpaceIsRaw;

    // Automatic synchronization to HW during Tuning session
    bool _bAutoSyncOn;

    // Current Parameter Settings
    CParameterBlackboard* _pMainParameterBlackboard;

    // Dynamic object creation
    CElementLibrarySet* _pElementLibrarySet;

    // XML parsing, object creation handling
    string _strParameterFrameworkConfigurationFolderPath; // Root folder for framework configuration
    string _strSchemaFolderLocation; // Place where schemas stand

    // Subsystem plugin location
    vector<string> _astrPluginFolderPaths;

    // System Class Configuration
    const CSystemClassConfiguration* _pSystemClassConfiguration;

    // Whole system structure checksum
    uint8_t _uiStructureChecksum;

    // Remote Processor Server
    IRemoteProcessorServerInterface* _pRemoteProcessorServer;

    // Parser description array
    static const SRemoteCommandParserItem gaRemoteCommandParserItems[];

    // Parser description array size
    static const uint32_t guiNbRemoteCommandParserItems;

    // Maximum command usage length
    uint32_t _uiMaxCommandUsageLength;

    // Tuning mode mutex
    pthread_mutex_t _tuningModeMutex;

    // Logging
    ILogger* _pLogger;
    uint32_t _uiLogDepth;
};

