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

#include "BinarySerializableElement.h"
#include "SyncerSet.h"
#include <list>
#include <set>
#include <map>

class CConfigurableElement;
class CDomainConfiguration;
class CParameterBlackboard;

class CConfigurableDomain : public CBinarySerializableElement
{
    typedef list<CConfigurableElement*>::const_iterator ConfigurableElementListIterator;
    typedef map<const CConfigurableElement*, CSyncerSet*>::const_iterator ConfigurableElementToSyncerSetMapIterator;
public:
    CConfigurableDomain(const string& strName);
    virtual ~CConfigurableDomain();

    // Sequence awareness
    void setSequenceAwareness(bool bSequenceAware);
    bool getSequenceAwareness() const;

    // Configuration Management
    bool createConfiguration(const string& strName, const CParameterBlackboard* pMainBlackboard, string& strError);
    bool deleteConfiguration(const string& strName, string& strError);
    bool renameConfiguration(const string& strName, const string& strNewName, string& strError);
    bool restoreConfiguration(const string& strName, CParameterBlackboard* pMainBlackboard, bool bAutoSync, string& strError);
    bool saveConfiguration(const string& strName, const CParameterBlackboard* pMainBlackboard, string& strError);
    bool setElementSequence(const string& strName, const vector<string>& astrNewElementSequence, string& strError);
    bool getElementSequence(const string& strName, string& strResult) const;

    // Last applied configuration
    string getLastAppliedConfigurationName() const;

    // Associated Configurable elements
    void gatherConfigurableElements(set<const CConfigurableElement*>& configurableElementSet) const;
    void listAssociatedToElements(string& strResult) const;

    // Configurable elements association
    bool addConfigurableElement(CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard, string& strError);
    bool removeConfigurableElement(CConfigurableElement* pConfigurableElement, string& strError);

    // Domain splitting
    bool split(CConfigurableElement* pConfigurableElement, string& strError);

    // Ensure validity on whole domain from main blackboard
    void validate(const CParameterBlackboard* pMainBlackboard);

    // Configuration application if required
    bool apply(CParameterBlackboard* pParameterBlackboard, CSyncerSet& syncerSet, bool bForced, string& strError) const;

    // Return applicable configuration validity for given configurable element
    bool isApplicableConfigurationValid(const CConfigurableElement* pConfigurableElement) const;

    // Presence of application condition on any configuration
    bool hasRules() const;

    // From IXmlSink
    virtual bool fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);

    // From IXmlSource
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

    // Class kind
    virtual string getKind() const;
private:
    // Returns true if children dynamic creation is to be dealt with (here, will allow child deletion upon clean)
    virtual bool childrenAreDynamic() const;

    // Ensure validity on areas related to configurable element
    void validateAreas(const CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard);

    // Attempt validation for all configurable element's areas, relying on already existing valid configuration inside domain
    void autoValidateAll();

    // Attempt validation for one configurable element's areas, relying on already existing valid configuration inside domain
    void autoValidateAreas(const CConfigurableElement* pConfigurableElement);

    // Attempt configuration validation for all configurable elements' areas, relying on already existing valid configuration inside domain
    bool autoValidateConfiguration(CDomainConfiguration* pDomainConfiguration);

    // Search for a valid configuration for given configurable element
    const CDomainConfiguration* findValidDomainConfiguration(const CConfigurableElement* pConfigurableElement) const;

    // Search for an applicable configuration
    const CDomainConfiguration* findApplicableDomainConfiguration() const;

    // In case configurable element was removed
    void computeSyncSet();

    // Check configurable element already attached
    bool containsConfigurableElement(const CConfigurableElement* pConfigurableCandidateElement) const;

    // Merge any descended configurable element to this one
    void mergeAlreadyAssociatedDescendantConfigurableElements(CConfigurableElement* pNewConfigurableElement);
    void mergeConfigurations(CConfigurableElement* pToConfigurableElement, CConfigurableElement* pFromConfigurableElement);

    // Configurable elements association
    void doAddConfigurableElement(CConfigurableElement* pConfigurableElement);
    void doRemoveConfigurableElement(CConfigurableElement* pConfigurableElement, bool bRecomputeSyncSet);

    // XML parsing
    bool parseDomainConfigurations(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);
    bool parseConfigurableElements(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);
    bool parseSettings(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);

    // XML composing
    void composeDomainConfigurations(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;
    void composeConfigurableElements(CXmlElement& xmlElement) const;
    void composeSettings(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

    // Syncer set retrieval from configurable element
    CSyncerSet* getSyncerSet(const CConfigurableElement* pConfigurableElement) const;

    // Configurable elements
    list<CConfigurableElement*> _configurableElementList;

    // Associated syncer sets
    map<const CConfigurableElement*, CSyncerSet*> _configurableElementToSyncerSetMap;

    // Sequence awareness
    bool _bSequenceAware;

    // Syncer set used to ensure propoer synchronization of restored configurable elements
    CSyncerSet _syncerSet;

    // Last applied configuration
    mutable const CDomainConfiguration* _pLastAppliedConfiguration;
};

