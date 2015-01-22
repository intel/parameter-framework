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
#include <string>


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
    bool createDomain(const std::string& strName, std::string& strError);

    /*
     * Adds a domain object to configurable domains. The ConfigurableDomains
     * object takes ownership of the ConfigurableDomain object.
     *
     * @param[in] pDomain the domain object.
     * @param[in] bOverwrite if false, will refuse to overwrite an existing
     * domain, otherwise, an existing domain with the same name will first be
     * removed.
     * @param[in,out] strError error message
     *
     * @returns true if the domain was successfully added
     */
    bool addDomain(CConfigurableDomain& domain, bool bOverwrite, std::string& strError);

    /**
     * Delete a domain by name
     *
     * @param[in] strName name of the domain to be deleted
     * @param[in,out] strError error message
     *
     * @returns true of the domain was sucessfully deleted, false otherwise (i.e.
     * the domain didn't exist).
     */
    bool deleteDomain(const std::string& strName, std::string& strError);
    void deleteAllDomains();
    bool renameDomain(const std::string& strName, const std::string& strNewName, std::string& strError);
    bool setSequenceAwareness(const std::string& strDomain, bool bSequenceAware, std::string& strError);
    bool getSequenceAwareness(const std::string& strDomain, bool& bSequenceAware, std::string& strError) const;
    bool listDomainElements(const std::string& strDomain, std::string& strResult) const;
    bool split(const std::string& strDomain, CConfigurableElement* pConfigurableElement, std::string& strError);
    void listAssociatedElements(std::string& strResult) const;
    void listConflictingElements(std::string& strResult) const;
    void listDomains(std::string& strResult) const;
    /// Configurations
    bool listConfigurations(const std::string& strDomain, std::string& strResult) const;
    bool createConfiguration(const std::string& strDomain, const std::string& strConfiguration, const CParameterBlackboard* pMainBlackboard, std::string& strError);
    bool deleteConfiguration(const std::string& strDomain, const std::string& strConfiguration, std::string& strError);
    bool renameConfiguration(const std::string& strDomain, const std::string& strConfigurationName, const std::string& strNewConfigurationName, std::string& strError);
    bool restoreConfiguration(const std::string& strDomain, const std::string& strConfiguration, CParameterBlackboard* pMainBlackboard, bool bAutoSync, std::list<std::string>& lstrError) const;
    bool saveConfiguration(const std::string& strDomain, const std::string& strConfiguration, const CParameterBlackboard* pMainBlackboard, std::string& strError);
    bool setElementSequence(const std::string& strDomain, const std::string& strConfiguration, const std::vector<std::string>& astrNewElementSequence, std::string& strError);
    bool getElementSequence(const std::string& strDomain, const std::string& strConfiguration, std::string& strResult) const;
    bool setApplicationRule(const std::string& strDomain, const std::string& strConfiguration, const std::string& strApplicationRule, const CSelectionCriteriaDefinition* pSelectionCriteriaDefinition, std::string& strError);
    bool clearApplicationRule(const std::string& strDomain, const std::string& strConfiguration, std::string& strError);
    bool getApplicationRule(const std::string& strDomain, const std::string& strConfiguration, std::string& strResult) const;

    // Last applied configurations
    void listLastAppliedConfigurations(std::string& strResult) const;

    // Configurable element - domain association
    bool addConfigurableElementToDomain(const std::string& strDomain, CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard, std::string& strError);
    bool removeConfigurableElementFromDomain(const std::string& strDomain, CConfigurableElement* pConfigurableElement, std::string& strError);

    // Configuration Blackboard for element
    CParameterBlackboard* findConfigurationBlackboard(const std::string& strDomain,
                                     const std::string& strConfiguration,
                                     const CConfigurableElement* pConfigurableElement,
                                     uint32_t& uiBaseOffset,
                                     bool& bIsLastApplied,
                                     std::string& strError) const;

    const CConfigurableDomain* findConfigurableDomain(const std::string& strDomain,
                                                      std::string& strError) const;

    // Binary settings load/store
    bool serializeSettings(const std::string& strBinarySettingsFilePath, bool bOut, uint8_t uiStructureChecksum, std::string& strError);

    // From IXmlSource
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

    // Ensure validity on whole domains from main blackboard
    void validate(const CParameterBlackboard* pMainBlackboard);

    // Configuration application if required
    void apply(CParameterBlackboard* pParameterBlackboard, CSyncerSet& syncerSet, bool bForce) const;

    // Class kind
    virtual std::string getKind() const;
private:
    /** Delete a domain
     *
     * @param(in] configurableDomain domain to be deleted
     * @param[in,out] strError error message
     * @returns true of the domain was sucessfully deleted, false otherwise (i.e.
     * the domain didn't exist).
     */
    void deleteDomain(CConfigurableDomain& configurableDomain);
    // Returns true if children dynamic creation is to be dealt with
    virtual bool childrenAreDynamic() const;
    // Gather owned configurable elements owned by any domain
    void gatherAllOwnedConfigurableElements(std::set<const CConfigurableElement*>& configurableElementSet) const;
    // Domain retrieval
    CConfigurableDomain* findConfigurableDomain(const std::string& strDomain, std::string& strError);
};

