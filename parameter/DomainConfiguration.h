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
#include <list>
#include <string>

class CConfigurableElement;
class CAreaConfiguration;
class CParameterBlackboard;
class CConfigurationAccessContext;
class CCompoundRule;
class CSyncerSet;
class CSelectionCriteriaDefinition;

class CDomainConfiguration : public CBinarySerializableElement
{
    enum ChildElementType {
        ECompoundRule
    };
    typedef std::list<CAreaConfiguration*>::const_iterator AreaConfigurationListIterator;
public:
    CDomainConfiguration(const std::string& strName);
    virtual ~CDomainConfiguration();

    // Configurable Elements association
    void addConfigurableElement(const CConfigurableElement* pConfigurableElement, const CSyncerSet* pSyncerSet);
    void removeConfigurableElement(const CConfigurableElement* pConfigurableElement);

    // Sequence management
    bool setElementSequence(const std::vector<std::string>& astrNewElementSequence, std::string& strError);
    void getElementSequence(std::string& strResult) const;

    // Application rule
    bool setApplicationRule(const std::string& strApplicationRule, const CSelectionCriteriaDefinition* pSelectionCriteriaDefinition, std::string& strError);
    void clearApplicationRule();
    void getApplicationRule(std::string& strResult) const;

    // Get Blackboard for an element of the domain
    CParameterBlackboard* getBlackboard(const CConfigurableElement* pConfigurableElement) const;

    // Save data from current
    void save(const CParameterBlackboard* pMainBlackboard);
    // Apply data to current
    bool restore(CParameterBlackboard* pMainBlackboard, bool bSync, std::list<std::string>* plstrError = NULL) const;
    // Ensure validity for configurable element area configuration
    void validate(const CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard);
    // Ensure validity of all area configurations
    void validate(const CParameterBlackboard* pMainBlackboard);
    // Return configuration validity for given configurable element
    bool isValid(const CConfigurableElement* pConfigurableElement) const;
    // Ensure validity of configurable element's area configuration by copying in from a valid one
    void validateAgainst(const CDomainConfiguration* pValidDomainConfiguration, const CConfigurableElement* pConfigurableElement);
    // Ensure validity of all configurable element's area configuration by copying in from a valid ones
    void validateAgainst(const CDomainConfiguration* pValidDomainConfiguration);
    // Applicability checking
    bool isApplicable() const;
    // Merge existing configurations to given configurable element ones
    void merge(CConfigurableElement* pToConfigurableElement, CConfigurableElement* pFromConfigurableElement);
    // Domain splitting
    void split(CConfigurableElement* pFromConfigurableElement);

    // XML configuration settings parsing/composing
    bool parseSettings(CXmlElement& xmlConfigurationSettingsElement, CXmlSerializingContext& serializingContext);
    void composeSettings(CXmlElement& xmlConfigurationSettingsElement, CXmlSerializingContext& serializingContext) const;

    // Serialization
    virtual void binarySerialize(CBinaryStream& binaryStream);

    // Data size
    virtual size_t getDataSize() const;

    // Class kind
    virtual std::string getKind() const;

private:
    // Returns true if children dynamic creation is to be dealt with (here, will allow child deletion upon clean)
    virtual bool childrenAreDynamic() const;
    // XML configuration settings serializing
    bool serializeConfigurableElementSettings(CAreaConfiguration* pAreaConfiguration, CXmlElement& xmlConfigurableElementSettingsElement, CXmlSerializingContext& serializingContext, bool bSerializeOut);
    // AreaConfiguration retrieval from configurable element
    CAreaConfiguration* getAreaConfiguration(const CConfigurableElement* pConfigurableElement) const;
    // AreaConfiguration retrieval from present area configurations
    CAreaConfiguration* findAreaConfiguration(const std::string& strConfigurableElementPath) const;
    // AreaConfiguration retrieval from given area configuration std::list
    CAreaConfiguration* findAreaConfiguration(const std::string& strConfigurableElementPath, const std::list<CAreaConfiguration*>& areaConfigurationList) const;
    // Area configuration ordering
    void reorderAreaConfigurations(const std::list<CAreaConfiguration*>& areaConfigurationList);
    // Find area configuration rank from regular std::list: for ordered std::list maintainance
    uint32_t getAreaConfigurationRank(const CAreaConfiguration* pAreaConfiguration) const;
    // Find area configuration from regular std::list based on rank: for ordered std::list maintainance
    CAreaConfiguration* getAreaConfiguration(uint32_t uiAreaConfigurationRank) const;

    // Rule
    const CCompoundRule* getRule() const;
    CCompoundRule* getRule();
    void setRule(CCompoundRule* pRule);

    // AreaConfigurations
    std::list<CAreaConfiguration*> _areaConfigurationList;
    std::list<CAreaConfiguration*> _orderedAreaConfigurationList;
};
