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

#include "AreaConfiguration.h"
#include "XmlDomainImportContext.h"
#include "XmlDomainExportContext.h"
#include "Element.h"
#include "Results.h"
#include <list>
#include <string>
#include <memory>

class CConfigurableElement;
class CParameterBlackboard;
class CConfigurationAccessContext;
class CCompoundRule;
class CSyncerSet;
class CSelectionCriteriaDefinition;

class CDomainConfiguration : public CElement
{
    enum ChildElementType
    {
        ECompoundRule
    };

public:
    CDomainConfiguration(const std::string &strName);

    // Configurable Elements association
    void addConfigurableElement(const CConfigurableElement *configurableElement,
                                const CSyncerSet *syncerSet);
    void removeConfigurableElement(const CConfigurableElement *pConfigurableElement);

    /**
     * Sequence management: Prepend provided elements into internal list in the same order than
     * they appear in the sequence of element path.
     * @param[in] newElementSequence sequence of path of new element
     * @param[out] error human readable error
     * @return true if the new sequence has been taken into account, false otherwise and error is
     * set accordingly.
     */
    bool setElementSequence(const std::vector<std::string> &newElementSequence, std::string &error);
    void getElementSequence(std::string &strResult) const;

    // Application rule
    bool setApplicationRule(const std::string &strApplicationRule,
                            const CSelectionCriteriaDefinition *pSelectionCriteriaDefinition,
                            std::string &strError);
    void clearApplicationRule();
    std::string getApplicationRule() const;

    // Get Blackboard for an element of the domain
    CParameterBlackboard *getBlackboard(const CConfigurableElement *pConfigurableElement) const;

    // Save data from current
    void save(const CParameterBlackboard *pMainBlackboard);

    /** Restore the configuration
     *
     * @param[in] pMainBlackboard the application main blackboard
     * @param[in] bSync indicates if a synchronisation has to be done
     * @param[out] errors, errors encountered during restoration
     * @return true if success false otherwise
     */
    bool restore(CParameterBlackboard *pMainBlackboard, bool bSync,
                 core::Results *errors = NULL) const;

    // Ensure validity for configurable element area configuration
    void validate(const CConfigurableElement *pConfigurableElement,
                  const CParameterBlackboard *pMainBlackboard);
    // Ensure validity of all area configurations
    void validate(const CParameterBlackboard *pMainBlackboard);
    // Return configuration validity for given configurable element
    bool isValid(const CConfigurableElement *pConfigurableElement) const;
    // Ensure validity of configurable element's area configuration by copying in from a valid one
    void validateAgainst(const CDomainConfiguration *pValidDomainConfiguration,
                         const CConfigurableElement *pConfigurableElement);
    // Ensure validity of all configurable element's area configuration by copying in from a valid
    // ones
    void validateAgainst(const CDomainConfiguration *validDomainConfiguration);
    // Applicability checking
    bool isApplicable() const;
    // Merge existing configurations to given configurable element ones
    void merge(CConfigurableElement *pToConfigurableElement,
               CConfigurableElement *pFromConfigurableElement);
    // Domain splitting
    void split(CConfigurableElement *pFromConfigurableElement);

    // XML configuration settings parsing/composing
    bool parseSettings(CXmlElement &xmlConfigurationSettingsElement,
                       CXmlDomainImportContext &context);
    void composeSettings(CXmlElement &xmlConfigurationSettingsElement,
                         CXmlDomainExportContext &context) const;

    // Class kind
    virtual std::string getKind() const;

private:
    using AreaConfiguration = std::unique_ptr<CAreaConfiguration>;
    using AreaConfigurations = std::list<AreaConfiguration>;

    // Returns true if children dynamic creation is to be dealt with (here, will allow child
    // deletion upon clean)
    virtual bool childrenAreDynamic() const;
    // XML configuration settings serializing
    bool importOneConfigurableElementSettings(CAreaConfiguration *areaConfiguration,
                                              CXmlElement &xmlConfigurableElementSettingsElement,
                                              CXmlDomainImportContext &context);
    bool exportOneConfigurableElementSettings(CAreaConfiguration *areaConfiguration,
                                              CXmlElement &xmlConfigurableElementSettingsElement,
                                              CXmlDomainExportContext &context) const;
    // AreaConfiguration retrieval from configurable element
    const AreaConfiguration &getAreaConfiguration(
        const CConfigurableElement *pConfigurableElement) const;

    /**
     * Returns the AreaConfiguration iterator associated to the Element refered by its path
     * @param[in] configurableElementPath to check if found in current list of areaconfigurations
     * @return iterator on the configuration associated to the Element with the given path,
     *                  last if not found
     */
    AreaConfigurations::iterator findAreaConfigurationByPath(
        const std::string &configurableElementPath);

    // Rule
    const CCompoundRule *getRule() const;
    CCompoundRule *getRule();
    void setRule(CCompoundRule *pRule);

    AreaConfigurations mAreaConfigurationList;
};
