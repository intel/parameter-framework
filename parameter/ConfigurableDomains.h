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
#include <set>
#include <list>


class CParameterBlackboard;
class CConfigurableElement;
class CSyncerSet;
class CConfigurableDomain;
class CSelectionCriteriaDefinition;

class CConfigurableDomains : public CBinarySerializableElement
{
public:
    CConfigurableDomains();

    // Configuration/Domains handling
    /// Domains
    bool createDomain(const string& strName, string& strError);
    bool deleteDomain(const string& strName, string& strError);
    void deleteAllDomains();
    bool renameDomain(const string& strName, const string& strNewName, string& strError);
    bool setSequenceAwareness(const string& strDomain, bool bSequenceAware, string& strError);
    bool getSequenceAwareness(const string& strDomain, bool& bSequenceAware, string& strError) const;
    bool listDomainElements(const string& strDomain, string& strResult) const;
    bool split(const string& strDomain, CConfigurableElement* pConfigurableElement, string& strError);
    void listAssociatedElements(string& strResult) const;
    void listConflictingElements(string& strResult) const;
    void listDomains(string& strResult) const;
    /// Configurations
    bool listConfigurations(const string& strDomain, string& strResult) const;
    bool createConfiguration(const string& strDomain, const string& strConfiguration, const CParameterBlackboard* pMainBlackboard, string& strError);
    bool deleteConfiguration(const string& strDomain, const string& strConfiguration, string& strError);
    bool renameConfiguration(const string& strDomain, const string& strConfigurationName, const string& strNewConfigurationName, string& strError);
    bool restoreConfiguration(const string& strDomain, const string& strConfiguration, CParameterBlackboard* pMainBlackboard, bool bAutoSync, list<string>& lstrError) const;
    bool saveConfiguration(const string& strDomain, const string& strConfiguration, const CParameterBlackboard* pMainBlackboard, string& strError);
    bool setElementSequence(const string& strDomain, const string& strConfiguration, const vector<string>& astrNewElementSequence, string& strError);
    bool getElementSequence(const string& strDomain, const string& strConfiguration, string& strResult) const;
    bool setApplicationRule(const string& strDomain, const string& strConfiguration, const string& strApplicationRule, const CSelectionCriteriaDefinition* pSelectionCriteriaDefinition, string& strError);
    bool clearApplicationRule(const string& strDomain, const string& strConfiguration, string& strError);
    bool getApplicationRule(const string& strDomain, const string& strConfiguration, string& strResult) const;

    // Last applied configurations
    void listLastAppliedConfigurations(string& strResult) const;

    // Configurable element - domain association
    bool addConfigurableElementToDomain(const string& strDomain, CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard, string& strError);
    bool removeConfigurableElementFromDomain(const string& strDomain, CConfigurableElement* pConfigurableElement, string& strError);

    // Configuration Blackboard for element
    CParameterBlackboard* findConfigurationBlackboard(const string& strDomain,
                                     const string& strConfiguration,
                                     const CConfigurableElement* pConfigurableElement,
                                     uint32_t& uiBaseOffset,
                                     bool& bIsLastApplied,
                                     string& strError) const;

    // Binary settings load/store
    bool serializeSettings(const string& strBinarySettingsFilePath, bool bOut, uint8_t uiStructureChecksum, string& strError);

    // From IXmlSource
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

    // Ensure validity on whole domains from main blackboard
    void validate(const CParameterBlackboard* pMainBlackboard);

    // Configuration application if required
    void apply(CParameterBlackboard* pParameterBlackboard, CSyncerSet& syncerSet, bool bForce) const;

    // Class kind
    virtual string getKind() const;
private:
    // Returns true if children dynamic creation is to be dealt with
    virtual bool childrenAreDynamic() const;
    // Gather owned configurable elements owned by any domain
    void gatherAllOwnedConfigurableElements(set<const CConfigurableElement*>& configurableElementSet) const;
    // Domain retrieval
    CConfigurableDomain* findConfigurableDomain(const string& strDomain, string& strError);
    const CConfigurableDomain* findConfigurableDomain(const string& strDomain, string& strError) const;
};

