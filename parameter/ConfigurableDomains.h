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

#include "Element.h"
#include "Results.h"
#include <set>
#include <string>

class CParameterBlackboard;
class CConfigurableElement;
class CSyncerSet;
class CConfigurableDomain;
class CSelectionCriteriaDefinition;

class CConfigurableDomains : public CElement
{
public:
    // Configuration/Domains handling
    /// Domains
    bool createDomain(const std::string &strName, std::string &strError);

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
    bool addDomain(CConfigurableDomain &domain, bool bOverwrite, std::string &strError);

    /**
     * Delete a domain by name
     *
     * @param[in] strName name of the domain to be deleted
     * @param[in,out] strError error message
     *
     * @returns true of the domain was sucessfully deleted, false otherwise (i.e.
     * the domain didn't exist).
     */
    bool deleteDomain(const std::string &strName, std::string &strError);
    void deleteAllDomains();
    bool renameDomain(const std::string &strName, const std::string &strNewName,
                      std::string &strError);
    bool setSequenceAwareness(const std::string &strDomain, bool bSequenceAware,
                              std::string &strError);
    bool getSequenceAwareness(const std::string &strDomain, bool &bSequenceAware,
                              std::string &strError) const;
    bool listDomainElements(const std::string &strDomain, std::string &strResult) const;

    /** Split a domain in two.
     * Remove an element of a domain and create a new domain which owns the element.
     *
     * @param[in] domainName the domain name
     * @param[in] element pointer to the element to remove
     * @param[out] infos useful information we can provide to client
     * @return true if succeed false otherwise
     */
    bool split(const std::string &domainName, CConfigurableElement *element, core::Results &infos);

    void listAssociatedElements(std::string &strResult) const;
    void listConflictingElements(std::string &strResult) const;
    void listDomains(std::string &strResult) const;
    /// Configurations
    bool listConfigurations(const std::string &strDomain, std::string &strResult) const;
    bool createConfiguration(const std::string &strDomain, const std::string &strConfiguration,
                             const CParameterBlackboard *pMainBlackboard, std::string &strError);
    bool deleteConfiguration(const std::string &strDomain, const std::string &strConfiguration,
                             std::string &strError);
    bool renameConfiguration(const std::string &strDomain, const std::string &strConfigurationName,
                             const std::string &strNewConfigurationName, std::string &strError);

    /** Restore a configuration
     *
     * @param[in] strDomain the domain name
     * @param[in] strConfiguration the configuration name
     * @param[in] pMainBlackboard the application main blackboard
     * @param[in] bAutoSync boolean which indicates if auto sync mechanism is on
     * @param[out] errors, errors encountered during restoration
     * @return true if success false otherwise
     */
    bool restoreConfiguration(const std::string &strDomain, const std::string &strConfiguration,
                              CParameterBlackboard *pMainBlackboard, bool bAutoSync,
                              core::Results &errors) const;

    bool saveConfiguration(const std::string &strDomain, const std::string &strConfiguration,
                           const CParameterBlackboard *pMainBlackboard, std::string &strError);
    bool setElementSequence(const std::string &strDomain, const std::string &strConfiguration,
                            const std::vector<std::string> &astrNewElementSequence,
                            std::string &strError);
    bool getElementSequence(const std::string &strDomain, const std::string &strConfiguration,
                            std::string &strResult) const;
    bool setApplicationRule(const std::string &strDomain, const std::string &strConfiguration,
                            const std::string &strApplicationRule,
                            const CSelectionCriteriaDefinition *pSelectionCriteriaDefinition,
                            std::string &strError);
    bool clearApplicationRule(const std::string &strDomain, const std::string &strConfiguration,
                              std::string &strError);
    bool getApplicationRule(const std::string &strDomain, const std::string &strConfiguration,
                            std::string &strResult) const;

    // Last applied configurations
    void listLastAppliedConfigurations(std::string &strResult) const;

    /** Associate a configurable element to a domain
     *
     * @param[in] domainName the domain name
     * @param[in] element pointer to the element to add
     * @param[in] mainBlackboard pointer to the application main blackboard
     * @param[out] infos useful information we can provide to client
     * @return true if succeed false otherwise
     */
    bool addConfigurableElementToDomain(const std::string &domainName,
                                        CConfigurableElement *element,
                                        const CParameterBlackboard *mainBlackboard,
                                        core::Results &infos);

    bool removeConfigurableElementFromDomain(const std::string &strDomain,
                                             CConfigurableElement *pConfigurableElement,
                                             std::string &strError);

    // Configuration Blackboard for element
    CParameterBlackboard *findConfigurationBlackboard(
        const std::string &strDomain, const std::string &strConfiguration,
        const CConfigurableElement *pConfigurableElement, size_t &baseOffset, bool &bIsLastApplied,
        std::string &strError) const;

    const CConfigurableDomain *findConfigurableDomain(const std::string &strDomain,
                                                      std::string &strError) const;

    // From IXmlSource
    void toXml(CXmlElement &xmlElement, CXmlSerializingContext &serializingContext) const override;

    // Ensure validity on whole domains from main blackboard
    void validate(const CParameterBlackboard *pMainBlackboard);

    /** Apply the configuration if required
     *
     * @param[in] pParameterBlackboard the blackboard to synchronize
     * @param[in] syncerSet the set containing application syncers
     * @param[in] bForce boolean used to force configuration application
     * @param[out] infos useful information we can provide to client
     */
    void apply(CParameterBlackboard *pParameterBlackboard, CSyncerSet &syncerSet, bool bForce,
               core::Results &infos) const;

    // Class kind
    std::string getKind() const override;

private:
    /** Delete a domain
     *
     * @param(in] configurableDomain domain to be deleted
     * @param[in,out] strError error message
     * @returns true of the domain was sucessfully deleted, false otherwise (i.e.
     * the domain didn't exist).
     */
    void deleteDomain(CConfigurableDomain &configurableDomain);
    // Returns true if children dynamic creation is to be dealt with
    bool childrenAreDynamic() const override;
    // Gather owned configurable elements owned by any domain
    void gatherAllOwnedConfigurableElements(
        std::set<const CConfigurableElement *> &configurableElementSet) const;
    // Domain retrieval
    CConfigurableDomain *findConfigurableDomain(const std::string &strDomain,
                                                std::string &strError);
};
