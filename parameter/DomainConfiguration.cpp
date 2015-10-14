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
#include "DomainConfiguration.h"
#include "ConfigurableElement.h"
#include "CompoundRule.h"
#include "Subsystem.h"
#include "XmlDomainSerializingContext.h"
#include "XmlDomainImportContext.h"
#include "XmlDomainExportContext.h"
#include "ConfigurationAccessContext.h"
#include "AlwaysAssert.hpp"
#include <assert.h>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include "RuleParser.h"

#define base CElement

using std::string;

CDomainConfiguration::CDomainConfiguration(const string& strName) : base(strName)
{
}

// Class kind
string CDomainConfiguration::getKind() const
{
    return "Configuration";
}

// Child dynamic creation
bool CDomainConfiguration::childrenAreDynamic() const
{
    return true;
}

// XML configuration settings parsing
bool CDomainConfiguration::parseSettings(CXmlElement& xmlConfigurationSettingsElement, CXmlSerializingContext& serializingContext)
{
    // Actual XML context
    CXmlDomainImportContext& xmlDomainImportContext = static_cast<CXmlDomainImportContext&>(serializingContext);

    // Parse configurable element's configuration settings
    CXmlElement::CChildIterator it(xmlConfigurationSettingsElement);

    CXmlElement xmlConfigurableElementSettingsElement;
    auto insertLocation = begin(mAreaConfigurationList);

    while (it.next(xmlConfigurableElementSettingsElement)) {

        // Retrieve area configuration
        string configurableElementPath;
        xmlConfigurableElementSettingsElement.getAttribute("Path", configurableElementPath);

        auto areaConfiguration = findAreaConfigurationByPath(configurableElementPath);
        if (areaConfiguration == end(mAreaConfigurationList)) {

            xmlDomainImportContext.setError("Configurable Element " + configurableElementPath  + " referred to by Configuration " + getPath() + " not associated to Domain");

            return false;
        }
        // Parse
        if (!serializeConfigurableElementSettings(areaConfiguration->get(), xmlConfigurableElementSettingsElement, xmlDomainImportContext, false)) {

            return false;
        }
        // Take into account the new configuration order by moving the configuration associated to
        // the element to the n-th position of the configuration list.
        // It will result in prepending to the configuration list wit the configuration of all
        // elements found in XML, keeping the order of the processing of the XML file.
        mAreaConfigurationList.splice(insertLocation, mAreaConfigurationList, areaConfiguration);
        // areaConfiguration is still valid, but now refer to the reorderer list
        insertLocation = std::next(areaConfiguration);
    }
    return true;
}

// XML configuration settings composing
void CDomainConfiguration::composeSettings(CXmlElement& xmlConfigurationSettingsElement, CXmlSerializingContext& serializingContext) const
{
    // Go through all are configurations
    for (auto &areaConfiguration : mAreaConfigurationList) {

        // Retrieve configurable element
        const CConfigurableElement* pConfigurableElement = areaConfiguration->getConfigurableElement();

        // Create configurable element child element
        CXmlElement xmlConfigurableElementSettingsElement;

        xmlConfigurationSettingsElement.createChild(xmlConfigurableElementSettingsElement, "ConfigurableElement");

        // Set Path attribute
        xmlConfigurableElementSettingsElement.setAttribute("Path", pConfigurableElement->getPath());

        // Delegate composing to area configuration
        ((CDomainConfiguration&)(*this)).serializeConfigurableElementSettings(areaConfiguration.get(), xmlConfigurableElementSettingsElement, serializingContext, true);
    }
}

// Serialize one configuration for one configurable element
bool CDomainConfiguration::serializeConfigurableElementSettings(CAreaConfiguration *areaConfiguration, CXmlElement& xmlConfigurableElementSettingsElement, CXmlSerializingContext& serializingContext, bool bSerializeOut)
{
    // Actual XML context
    CXmlDomainExportContext& xmlDomainExportContext =
        static_cast<CXmlDomainExportContext&>(serializingContext);

    // Configurable Element
    const CConfigurableElement* pConfigurableElement = areaConfiguration->getConfigurableElement();

    // Element content
    CXmlElement xmlConfigurableElementSettingsElementContent;

    // Deal with element itself
    if (!bSerializeOut) {

        // Check structure
        if (xmlConfigurableElementSettingsElement.getNbChildElements() != 1) {

            // Structure error
            serializingContext.setError("Struture error encountered while parsing settings of " + pConfigurableElement->getKind() + " " + pConfigurableElement->getName() + " in Configuration " + getPath());

            return false;
        }

        // Check name and kind
        if (!xmlConfigurableElementSettingsElement.getChildElement(pConfigurableElement->getKind(), pConfigurableElement->getName(), xmlConfigurableElementSettingsElementContent)) {

            serializingContext.setError("Couldn't find settings for " + pConfigurableElement->getKind() + " " + pConfigurableElement->getName() + " for Configuration " + getPath());

            return false;
        }
    } else {

        // Create child XML element
        xmlConfigurableElementSettingsElement.createChild(xmlConfigurableElementSettingsElementContent, pConfigurableElement->getKind());

        // Set Name
        xmlConfigurableElementSettingsElementContent.setNameAttribute(pConfigurableElement->getName());
    }

    // Change context type to parameter settings access
    string strError;

    // Create configuration access context
    CConfigurationAccessContext configurationAccessContext(strError, bSerializeOut);

    // Provide current value space
    configurationAccessContext.setValueSpaceRaw(xmlDomainExportContext.valueSpaceIsRaw());

    // Provide current output raw format
    configurationAccessContext.setOutputRawFormat(xmlDomainExportContext.outputRawFormatIsHex());

    // Have domain configuration parse settings for configurable element
    if (!areaConfiguration->serializeXmlSettings(xmlConfigurableElementSettingsElementContent, configurationAccessContext)) {

        // Forward error
        xmlDomainExportContext.setError(strError);

        return false;
    }
    return true;
}

void CDomainConfiguration::addConfigurableElement(const CConfigurableElement *configurableElement, const CSyncerSet *syncerSet)
{
    mAreaConfigurationList.emplace_back(configurableElement->createAreaConfiguration(syncerSet));
}

void CDomainConfiguration::removeConfigurableElement(const CConfigurableElement* pConfigurableElement)
{
    auto &areaConfigurationToRemove = getAreaConfiguration(pConfigurableElement);

    mAreaConfigurationList.remove(areaConfigurationToRemove);
}

bool CDomainConfiguration::setElementSequence(const std::vector<string> &newElementSequence, string &error)
{
    std::vector<string> elementSequenceSet;
    auto insertLocation = begin(mAreaConfigurationList);

    for (const std::string &elementPath : newElementSequence) {

        auto areaConfiguration = findAreaConfigurationByPath(elementPath);
        if (areaConfiguration == end(mAreaConfigurationList)) {

            error = "Element " + elementPath + " not found in domain";

            return false;
        }
        auto it = find(begin(elementSequenceSet), end(elementSequenceSet), elementPath);
        if (it != end(elementSequenceSet)) {
            error = "Element " + elementPath + " provided more than once";
            return false;
        }
        elementSequenceSet.push_back(elementPath);
        // Take into account the new configuration order by moving the configuration associated to
        // the element to the n-th position of the configuration list.
        // It will result in prepending to the configuration list wit the configuration of all
        // elements found in XML, keeping the order of the processing of the XML file.
        mAreaConfigurationList.splice(insertLocation, mAreaConfigurationList, areaConfiguration);
        // areaConfiguration is still valid, but now refer to the reorderer list
        insertLocation = std::next(areaConfiguration);
    }
    return true;
}

void CDomainConfiguration::getElementSequence(string& strResult) const
{
    // List configurable element paths out of ordered area configuration list
    strResult = accumulate(begin(mAreaConfigurationList), end(mAreaConfigurationList),
                           string("\n"), [](const string &a, const AreaConfiguration &conf) {
        return a + conf->getConfigurableElement()->getPath() + "\n";
    });
}

// Application rule
bool CDomainConfiguration::setApplicationRule(const string& strApplicationRule, const CSelectionCriteriaDefinition* pSelectionCriteriaDefinition, string& strError)
{
    // Parser
    CRuleParser ruleParser(strApplicationRule, pSelectionCriteriaDefinition);

    // Attempt to parse it
    if (!ruleParser.parse(NULL, strError)) {

        return false;
    }
    // Replace compound rule
    setRule(ruleParser.grabRootRule());

    return true;
}

void CDomainConfiguration::clearApplicationRule()
{
    // Replace compound rule
    setRule(NULL);
}

string CDomainConfiguration::getApplicationRule() const
{
    const CCompoundRule* pRule = getRule();
    return pRule ? pRule->dump() : "<none>";
}

/**
 * Get the Configuration Blackboard.
 *
 * Fetch the Configuration Blackboard related to the ConfigurableElement given in parameter. This
 * Element is used to retrieve the correct AreaConfiguration where the Blackboard is stored.
 *
 * @param[in] pConfigurableElement      A pointer to a ConfigurableElement that is part of the
 *                                      Domain. This must have been checked previously, as an
 *                                      assertion is performed.
 *
 * return Pointer to the Blackboard of the Configuration.
 */
CParameterBlackboard* CDomainConfiguration::getBlackboard(const CConfigurableElement* pConfigurableElement) const
{
    const auto &it = find_if(begin(mAreaConfigurationList), end(mAreaConfigurationList),
                      [&](const AreaConfiguration &conf) {
        return conf != nullptr && conf->getConfigurableElement() == pConfigurableElement;
    });
    ALWAYS_ASSERT(it != end(mAreaConfigurationList), "Configurable Element "
                  << pConfigurableElement->getName() << " not found in any area Configuration");
    return &(*it)->getBlackboard();
}

// Save data from current
void CDomainConfiguration::save(const CParameterBlackboard* pMainBlackboard)
{
    // Just propagate to areas
    for(auto &areaConfiguration : mAreaConfigurationList) {
        areaConfiguration->save(pMainBlackboard);
    }
}

// Apply data to current
bool CDomainConfiguration::restore(CParameterBlackboard* pMainBlackboard,
                                   bool bSync,
                                   core::Results* errors) const
{
    return std::accumulate(begin(mAreaConfigurationList), end(mAreaConfigurationList),
                           true, [&](bool accumulator, const AreaConfiguration &conf) {
        return conf->restore(pMainBlackboard, bSync, errors) && accumulator;
    });
}

// Ensure validity for configurable element area configuration
void CDomainConfiguration::validate(const CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard)
{
    auto &areaConfigurationToValidate = getAreaConfiguration(pConfigurableElement);

    // Delegate
    areaConfigurationToValidate->validate(pMainBlackboard);
}

// Ensure validity of all area configurations
void CDomainConfiguration::validate(const CParameterBlackboard* pMainBlackboard)
{
    for(auto &areaConfiguration : mAreaConfigurationList) {
        areaConfiguration->validate(pMainBlackboard);
    }
}

// Return configuration validity for given configurable element
bool CDomainConfiguration::isValid(const CConfigurableElement* pConfigurableElement) const
{
    // Get child configurable elemnt's area configuration
    auto &areaConfiguration = getAreaConfiguration(pConfigurableElement);

    ALWAYS_ASSERT(areaConfiguration != nullptr, "Configurable Element "
                  << pConfigurableElement->getName() << " not found in any area Configuration");

    return areaConfiguration->isValid();
}

// Ensure validity of configurable element's area configuration by copying in from a valid one
void CDomainConfiguration::validateAgainst(const CDomainConfiguration* pValidDomainConfiguration, const CConfigurableElement* pConfigurableElement)
{
    // Retrieve related area configurations
    auto &areaConfigurationToValidate = getAreaConfiguration(pConfigurableElement);
    const auto &areaConfigurationToValidateAgainst = pValidDomainConfiguration->getAreaConfiguration(pConfigurableElement);

    // Delegate to area
    areaConfigurationToValidate->validateAgainst(areaConfigurationToValidateAgainst.get());
}

void CDomainConfiguration::validateAgainst(const CDomainConfiguration *validDomainConfiguration)
{
    ALWAYS_ASSERT(mAreaConfigurationList.size() == validDomainConfiguration->mAreaConfigurationList.size(),
                  "Cannot validate domain configuration " << getPath()
                  << " since area configuration list does not have the same size"
                     "than the configuration list to check against");
    for (const auto &configurationToValidateAgainst : validDomainConfiguration->mAreaConfigurationList) {
        // Get the area configuration associated to the configurable element of the
        // valid area configuration, it will assert if none found.
        auto configurableElement = configurationToValidateAgainst->getConfigurableElement();
        auto &configurationToValidate = getAreaConfiguration(configurableElement);
        // Delegate to area
        configurationToValidate->validateAgainst(configurationToValidateAgainst.get());
    }
}

// Dynamic data application
bool CDomainConfiguration::isApplicable() const
{
    const CCompoundRule* pRule = getRule();

    return pRule && pRule->matches();
}

// Merge existing configurations to given configurable element ones
void CDomainConfiguration::merge(CConfigurableElement* pToConfigurableElement, CConfigurableElement* pFromConfigurableElement)
{
    // Retrieve related area configurations
    auto &areaConfigurationToMergeTo = getAreaConfiguration(pToConfigurableElement);
    const auto &areaConfigurationToMergeFrom = getAreaConfiguration(pFromConfigurableElement);

    // Do the merge
    areaConfigurationToMergeFrom->copyToOuter(areaConfigurationToMergeTo.get());
}

// Domain splitting
void CDomainConfiguration::split(CConfigurableElement* pFromConfigurableElement)
{
    // Retrieve related area configuration
    const auto &areaConfigurationToSplitFrom = getAreaConfiguration(pFromConfigurableElement);

    // Go through children areas to copy configuration data to them
    size_t uiNbConfigurableElementChildren = pFromConfigurableElement->getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurableElementChildren; uiChild++) {

        CConfigurableElement* pToChildConfigurableElement = static_cast<CConfigurableElement*>(pFromConfigurableElement->getChild(uiChild));

        // Get child configurable elemnt's area configuration
        auto &childAreaConfiguration = getAreaConfiguration(pToChildConfigurableElement);

        // Do the copy
        childAreaConfiguration->copyFromOuter(areaConfigurationToSplitFrom.get());
    }
}

const CDomainConfiguration::AreaConfiguration &CDomainConfiguration::getAreaConfiguration(const CConfigurableElement* pConfigurableElement) const
{
    const auto &it = find_if(begin(mAreaConfigurationList), end(mAreaConfigurationList),
                      [&](const AreaConfiguration &conf) {
        return conf->getConfigurableElement() == pConfigurableElement;
    });
    ALWAYS_ASSERT(it != end(mAreaConfigurationList), "Configurable Element "
                  << pConfigurableElement->getName() << " not found in Domain Configuration list");
    return *it;
}

CDomainConfiguration::AreaConfigurations::iterator CDomainConfiguration::findAreaConfigurationByPath(const std::string &configurableElementPath)
{
    auto areaConfiguration = find_if(begin(mAreaConfigurationList), end(mAreaConfigurationList),
                                         [&](const AreaConfiguration &conf) {
        return conf->getConfigurableElement()->getPath() == configurableElementPath;
    });
    return areaConfiguration;
}

// Rule
const CCompoundRule* CDomainConfiguration::getRule() const
{
    if (getNbChildren()) {
        // Rule created
        return static_cast<const CCompoundRule*>(getChild(ECompoundRule));
    }
    return NULL;
}

CCompoundRule* CDomainConfiguration::getRule()
{
    if (getNbChildren()) {
        // Rule created
        return static_cast<CCompoundRule*>(getChild(ECompoundRule));
    }
    return NULL;
}

void CDomainConfiguration::setRule(CCompoundRule* pRule)
{
    CCompoundRule* pOldRule = getRule();

    if (pOldRule) {
        // Remove previous rule
        removeChild(pOldRule);

        delete pOldRule;
    }

    // Set new one
    if (pRule) {
        // Chain
        addChild(pRule);
    }
}
