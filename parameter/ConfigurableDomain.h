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

#include "BinarySerializableElement.h"
#include "SyncerSet.h"
#include <list>
#include <set>
#include <map>

class CConfigurableElement;
class CDomainConfiguration;
class CParameterBlackboard;
class CSelectionCriteriaDefinition;

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
    bool restoreConfiguration(const string& strName, CParameterBlackboard* pMainBlackboard, bool bAutoSync, list<string>& strError) const;
    bool saveConfiguration(const string& strName, const CParameterBlackboard* pMainBlackboard, string& strError);
    bool setElementSequence(const string& strConfiguration, const vector<string>& astrNewElementSequence, string& strError);
    bool getElementSequence(const string& strConfiguration, string& strResult) const;
    bool setApplicationRule(const string& strConfiguration, const string& strApplicationRule, const CSelectionCriteriaDefinition* pSelectionCriteriaDefinition, string& strError);
    bool clearApplicationRule(const string& strConfiguration, string& strError);
    bool getApplicationRule(const string& strConfiguration, string& strResult) const;

    // Last applied configuration name
    string getLastAppliedConfigurationName() const;

    // Pending configuration name
    string getPendingConfigurationName() const;

    // Associated Configurable elements
    void gatherConfigurableElements(set<const CConfigurableElement*>& configurableElementSet) const;
    void listAssociatedToElements(string& strResult) const;

    // Configurable elements association
    bool addConfigurableElement(CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard, string& strError);
    bool removeConfigurableElement(CConfigurableElement* pConfigurableElement, string& strError);

    // Blackboard Configuration and Base Offset retrieval
    CParameterBlackboard* findConfigurationBlackboard(const string& strConfiguration,
                                                      const CConfigurableElement* pConfigurableElement,
                                                      uint32_t& uiBaseOffset,
                                                      bool& bIsLastApplied,
                                                      string& strError) const;

    // Domain splitting
    bool split(CConfigurableElement* pConfigurableElement, string& strError);

    // Ensure validity on whole domain from main blackboard
    void validate(const CParameterBlackboard* pMainBlackboard);

    // Configuration application if required
    void apply(CParameterBlackboard* pParameterBlackboard, CSyncerSet* pSyncerSet, bool bForced) const;

    // Return applicable configuration validity for given configurable element
    bool isApplicableConfigurationValid(const CConfigurableElement* pConfigurableElement) const;

    // From IXmlSink
    virtual bool fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);

    // From IXmlSource
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

    // Class kind
    virtual string getKind() const;

protected:
    // Content dumping
    virtual void logValue(string& strValue, CErrorContext& errorContext) const;

private:
    // Get pending configuration
    const CDomainConfiguration* getPendingConfiguration() const;

    // Search for an applicable configuration
    const CDomainConfiguration* findApplicableDomainConfiguration() const;

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


    // In case configurable element was removed
    void computeSyncSet();

    // Check configurable element already attached
    bool containsConfigurableElement(const CConfigurableElement* pConfigurableCandidateElement) const;

    // Merge any descended configurable element to this one
    void mergeAlreadyAssociatedDescendantConfigurableElements(CConfigurableElement* pNewConfigurableElement);
    void mergeConfigurations(CConfigurableElement* pToConfigurableElement, CConfigurableElement* pFromConfigurableElement);

    // Configurable elements association
    void doAddConfigurableElement(CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard = NULL);
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

    // Configuration retrieval
    CDomainConfiguration* findConfiguration(const string& strConfiguration, string& strError);
    const CDomainConfiguration* findConfiguration(const string& strConfiguration, string& strError) const;

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

