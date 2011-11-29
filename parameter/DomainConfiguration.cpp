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
#include "DomainConfiguration.h"
#include "AreaConfiguration.h"
#include "ConfigurableElement.h"
#include "CompoundRule.h"
#include "Subsystem.h"
#include "XmlDomainSerializingContext.h"
#include "ConfigurationAccessContext.h"
#include <assert.h>
#include "RuleParser.h"

#define base CBinarySerializableElement

CDomainConfiguration::CDomainConfiguration(const string& strName) : base(strName)
{
}

CDomainConfiguration::~CDomainConfiguration()
{
    AreaConfigurationListIterator it;

    for (it = _areaConfigurationList.begin(); it != _areaConfigurationList.end(); ++it) {

        delete *it;
    }
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
    CXmlDomainSerializingContext& xmlDomainSerializingContext = static_cast<CXmlDomainSerializingContext&>(serializingContext);

    // Take care of configurable elements / area configurations ranks
    list<CAreaConfiguration*> areaConfigurationList;

    // Parse configurable element's configuration settings
    CXmlElement::CChildIterator it(xmlConfigurationSettingsElement);

    CXmlElement xmlConfigurableElementSettingsElement;

    while (it.next(xmlConfigurableElementSettingsElement)) {

        // Retrieve area configuration
        string strConfigurableElementPath = xmlConfigurableElementSettingsElement.getAttributeString("Path");

        CAreaConfiguration* pAreaConfiguration = findAreaConfiguration(strConfigurableElementPath);

        if (!pAreaConfiguration) {

            xmlDomainSerializingContext.setError("Configurable Element " + strConfigurableElementPath  + " referred to by Configuration " + getPath() + " not associated to Domain");

            return false;
        }
        // Ranks
        areaConfigurationList.push_back(pAreaConfiguration);

        // Parse
        if (!serializeConfigurableElementSettings(pAreaConfiguration, xmlConfigurableElementSettingsElement, xmlDomainSerializingContext, false)) {

            return false;
        }
    }

    // Reorder area configurations according to XML content
    reorderAreaConfigurations(areaConfigurationList);

    return true;
}

// XML configuration settings composing
void CDomainConfiguration::composeSettings(CXmlElement& xmlConfigurationSettingsElement, CXmlSerializingContext& serializingContext) const
{
    // Go through all are configurations
    AreaConfigurationListIterator it;

    for (it = _orderedAreaConfigurationList.begin(); it != _orderedAreaConfigurationList.end(); ++it) {

        const CAreaConfiguration* pAreaConfiguration = *it;

        // Retrieve configurable element
        const CConfigurableElement* pConfigurableElement = pAreaConfiguration->getConfigurableElement();

        // Create configurable element child element
        CXmlElement xmlConfigurableElementSettingsElement;

        xmlConfigurationSettingsElement.createChild(xmlConfigurableElementSettingsElement, "ConfigurableElement");

        // Set Path attribute
        xmlConfigurableElementSettingsElement.setAttributeString("Path", pConfigurableElement->getPath());

        // Delegate composing to area configuration
        ((CDomainConfiguration&)(*this)).serializeConfigurableElementSettings((CAreaConfiguration*)pAreaConfiguration, xmlConfigurableElementSettingsElement, serializingContext, true);
    }
}

// Serialize one configuration for one configurable element
bool CDomainConfiguration::serializeConfigurableElementSettings(CAreaConfiguration* pAreaConfiguration, CXmlElement& xmlConfigurableElementSettingsElement, CXmlSerializingContext& serializingContext, bool bSerializeOut)
{
    // Actual XML context
    CXmlDomainSerializingContext& xmlDomainSerializingContext = static_cast<CXmlDomainSerializingContext&>(serializingContext);

    // Configurable Element
    const CConfigurableElement* pConfigurableElement = pAreaConfiguration->getConfigurableElement();

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
    configurationAccessContext.setValueSpaceRaw(xmlDomainSerializingContext.valueSpaceIsRaw());

    // Provide current output raw format
    configurationAccessContext.setOutputRawFormat(xmlDomainSerializingContext.outputRawFormatIsHex());

    // Get subsystem
    const CSubsystem* pSubsystem = pConfigurableElement->getBelongingSubsystem();

    if (pSubsystem && pSubsystem != pConfigurableElement) {

        // Element is a descendant of subsystem

        // Deal with Endianness
        configurationAccessContext.setBigEndianSubsystem(pSubsystem->isBigEndian());
    }

    // Have domain configuration parse settings for configurable element
    if (!pAreaConfiguration->serializeXmlSettings(xmlConfigurableElementSettingsElementContent, configurationAccessContext)) {

        // Forward error
        xmlDomainSerializingContext.setError(strError);

        return false;
    }
    return true;
}

// Configurable Elements association
void CDomainConfiguration::addConfigurableElement(const CConfigurableElement* pConfigurableElement, const CSyncerSet* pSyncerSet)
{
    CAreaConfiguration* pAreaConfiguration = new CAreaConfiguration(pConfigurableElement, pSyncerSet);

    _areaConfigurationList.push_back(pAreaConfiguration);
    _orderedAreaConfigurationList.push_back(pAreaConfiguration);
}

void CDomainConfiguration::removeConfigurableElement(const CConfigurableElement* pConfigurableElement)
{
    CAreaConfiguration* pAreaConfigurationToRemove = getAreaConfiguration(pConfigurableElement);

    _areaConfigurationList.remove(pAreaConfigurationToRemove);
    _orderedAreaConfigurationList.remove(pAreaConfigurationToRemove);

    delete pAreaConfigurationToRemove;
}

// Sequence management
bool CDomainConfiguration::setElementSequence(const vector<string>& astrNewElementSequence, string& strError)
{
    // Build a new list of AreaConfiguration objects
    list<CAreaConfiguration*> areaConfigurationList;

    uint32_t uiConfigurableElement;

    for (uiConfigurableElement = 0; uiConfigurableElement < astrNewElementSequence.size(); uiConfigurableElement++) {

        string strConfigurableElementPath = astrNewElementSequence[uiConfigurableElement];

        CAreaConfiguration* pAreaConfiguration = findAreaConfiguration(strConfigurableElementPath);

        if (!pAreaConfiguration) {

            strError = "Element " + strConfigurableElementPath + " not found in domain";

            return false;
        }
        // Check not already present in the list
        if (findAreaConfiguration(strConfigurableElementPath, areaConfigurationList)) {

            strError = "Element " + strConfigurableElementPath + " provided more than once";

            return false;
        }

        // Store new ordered area configuration
        areaConfigurationList.push_back(pAreaConfiguration);
    }

    // Reorder area configurations according to given path list
    reorderAreaConfigurations(areaConfigurationList);

    return true;
}

void CDomainConfiguration::getElementSequence(string& strResult) const
{
    strResult = "\n";

    AreaConfigurationListIterator it;

    // List configurable element paths out of ordered area configuration list
    for (it = _orderedAreaConfigurationList.begin(); it != _orderedAreaConfigurationList.end(); ++it) {

        const CAreaConfiguration* pAreaConfiguration = *it;

        const CConfigurableElement* pConfigurableElement = pAreaConfiguration->getConfigurableElement();

        strResult += pConfigurableElement->getPath() + "\n";
    }
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

void CDomainConfiguration::getApplicationRule(string& strResult) const
{
    // Rule
    const CCompoundRule* pRule = getRule();

    if (pRule) {
        // Start clear
        strResult.clear();

        // Dump rule
        pRule->dump(strResult);

    } else {

        strResult = "<none>";
    }
}

// Save data from current
void CDomainConfiguration::save(const CParameterBlackboard* pMainBlackboard)
{
    AreaConfigurationListIterator it;

    // Just propagate to areas
    for (it = _areaConfigurationList.begin(); it != _areaConfigurationList.end(); ++it) {

        CAreaConfiguration* pAreaConfiguration = *it;

        pAreaConfiguration->save(pMainBlackboard);
    }
}

// Apply data to current
bool CDomainConfiguration::restore(CParameterBlackboard* pMainBlackboard, bool bSync, string& strError) const
{
    AreaConfigurationListIterator it;

    // Just propagate to areas
    for (it = _orderedAreaConfigurationList.begin(); it != _orderedAreaConfigurationList.end(); ++it) {

        const CAreaConfiguration* pAreaConfiguration = *it;

        if (!pAreaConfiguration->restore(pMainBlackboard, bSync, strError)) {

            return false;
        }
    }

    return true;
}

// Ensure validity for configurable element area configuration
void CDomainConfiguration::validate(const CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard)
{
    CAreaConfiguration* pAreaConfigurationToValidate = getAreaConfiguration(pConfigurableElement);

    // Delegate
    pAreaConfigurationToValidate->validate(pMainBlackboard);
}

// Ensure validity of all area configurations
void CDomainConfiguration::validate(const CParameterBlackboard* pMainBlackboard)
{
    AreaConfigurationListIterator it;

    for (it = _areaConfigurationList.begin(); it != _areaConfigurationList.end(); ++it) {

        CAreaConfiguration* pAreaConfiguration = *it;

        pAreaConfiguration->validate(pMainBlackboard);
    }
}

// Return configuration validity for given configurable element
bool CDomainConfiguration::isValid(const CConfigurableElement* pConfigurableElement) const
{
    // Get child configurable elemnt's area configuration
    CAreaConfiguration* pAreaConfiguration = getAreaConfiguration(pConfigurableElement);

    assert(pAreaConfiguration);

    return pAreaConfiguration->isValid();
}

// Ensure validity of configurable element's area configuration by copying in from a valid one
void CDomainConfiguration::validateAgainst(const CDomainConfiguration* pValidDomainConfiguration, const CConfigurableElement* pConfigurableElement)
{
    // Retrieve related area configurations
    CAreaConfiguration* pAreaConfigurationToValidate = getAreaConfiguration(pConfigurableElement);
    const CAreaConfiguration* pAreaConfigurationToValidateAgainst = pValidDomainConfiguration->getAreaConfiguration(pConfigurableElement);

    // Delegate to area
    pAreaConfigurationToValidate->validateAgainst(pAreaConfigurationToValidateAgainst);
}

// Ensure validity of all configurable element's area configuration by copying in from a valid ones
void CDomainConfiguration::validateAgainst(const CDomainConfiguration* pValidDomainConfiguration)
{
    // Copy in configuration data from against domain
    AreaConfigurationListIterator it, itAgainst;

    for (it = _areaConfigurationList.begin(), itAgainst = pValidDomainConfiguration->_areaConfigurationList.begin(); it != _areaConfigurationList.end(); ++it, ++itAgainst) {

        CAreaConfiguration* pAreaConfigurationToValidate = *it;
        const CAreaConfiguration* pAreaConfigurationToValidateAgainst = *itAgainst;

        // Delegate to area
        pAreaConfigurationToValidate->validateAgainst(pAreaConfigurationToValidateAgainst);
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
    CAreaConfiguration* pAreaConfigurationToMergeTo = getAreaConfiguration(pToConfigurableElement);
    const CAreaConfiguration* pAreaConfigurationToMergeFrom = getAreaConfiguration(pFromConfigurableElement);

    // Do the merge
    pAreaConfigurationToMergeTo->copyFromInner(pAreaConfigurationToMergeFrom);
}

// Domain splitting
void CDomainConfiguration::split(CConfigurableElement* pFromConfigurableElement)
{
    // Retrieve related area configuration
    const CAreaConfiguration* pAreaConfigurationToSplitFrom = getAreaConfiguration(pFromConfigurableElement);

    // Go through children areas to copy configuration data to them
    uint32_t uiNbConfigurableElementChildren = pFromConfigurableElement->getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurableElementChildren; uiChild++) {

        CConfigurableElement* pToChildConfigurableElement = static_cast<CConfigurableElement*>(pFromConfigurableElement->getChild(uiChild));

        // Get child configurable elemnt's area configuration
        CAreaConfiguration* pChildAreaConfiguration = getAreaConfiguration(pToChildConfigurableElement);

        // Do the copy
        pAreaConfigurationToSplitFrom->copyToInner(pChildAreaConfiguration);
    }
}

// AreaConfiguration retrieval from configurable element
CAreaConfiguration* CDomainConfiguration::getAreaConfiguration(const CConfigurableElement* pConfigurableElement) const
{
    AreaConfigurationListIterator it;

    for (it = _areaConfigurationList.begin(); it != _areaConfigurationList.end(); ++it) {

        CAreaConfiguration* pAreaConfiguration = *it;

        if (pAreaConfiguration->getConfigurableElement() == pConfigurableElement) {

            return pAreaConfiguration;
        }
    }
    // Not found?
    assert(0);

    return NULL;
}

// AreaConfiguration retrieval from present area configurations
CAreaConfiguration* CDomainConfiguration::findAreaConfiguration(const string& strConfigurableElementPath) const
{
    return findAreaConfiguration(strConfigurableElementPath, _areaConfigurationList);
}

// AreaConfiguration retrieval from given area configuration list
CAreaConfiguration* CDomainConfiguration::findAreaConfiguration(const string& strConfigurableElementPath, const list<CAreaConfiguration*>& areaConfigurationList) const
{
    AreaConfigurationListIterator it;

    for (it = areaConfigurationList.begin(); it != areaConfigurationList.end(); ++it) {

        CAreaConfiguration* pAreaConfiguration = *it;

        if (pAreaConfiguration->getConfigurableElement()->getPath() == strConfigurableElementPath) {

            return pAreaConfiguration;
        }
    }

    // Not found
    return NULL;
}

// Area configuration ordering
void CDomainConfiguration::reorderAreaConfigurations(const list<CAreaConfiguration*>& areaConfigurationList)
{
    // Ensure elements in provided list appear first and ordered the same way in internal one

    // Remove all elements present in the provided list from the internal one
    AreaConfigurationListIterator it;

    for (it = areaConfigurationList.begin(); it != areaConfigurationList.end(); ++it) {

        _orderedAreaConfigurationList.remove(*it);
    }

    // Prepended provided elements into internal list
    _orderedAreaConfigurationList.insert(_orderedAreaConfigurationList.begin(), areaConfigurationList.begin(), areaConfigurationList.end());
}

// Find area configuration rank from regular list: for ordered list maintainance
uint32_t CDomainConfiguration::getAreaConfigurationRank(const CAreaConfiguration* pAreaConfiguration) const
{
    uint32_t uiAreaConfigurationRank;
    AreaConfigurationListIterator it;

    // Propagate request to areas
    for (it = _areaConfigurationList.begin(), uiAreaConfigurationRank = 0; it != _areaConfigurationList.end(); ++it, ++uiAreaConfigurationRank) {

        if (*it == pAreaConfiguration) {

            return uiAreaConfigurationRank;
        }
    }

    assert(0);

    return 0;
}

// Find area configuration from regular list based on rank: for ordered list maintainance
CAreaConfiguration* CDomainConfiguration::getAreaConfiguration(uint32_t uiAreaConfigurationRank) const
{
    AreaConfigurationListIterator it;
    uint32_t uiCurrentAreaConfigurationRank;

    // Propagate request to areas
    for (it = _areaConfigurationList.begin(), uiCurrentAreaConfigurationRank = 0; it != _areaConfigurationList.end(); ++it, ++uiCurrentAreaConfigurationRank) {

        if (uiCurrentAreaConfigurationRank == uiAreaConfigurationRank) {

            return *it;
        }
    }

    assert(0);

    return NULL;
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

// Serialization
void CDomainConfiguration::binarySerialize(CBinaryStream& binaryStream)
{
    AreaConfigurationListIterator it;

    // Area configurations order
    if (binaryStream.isOut()) {

        for (it = _orderedAreaConfigurationList.begin(); it != _orderedAreaConfigurationList.end(); ++it) {

            // Get rank
            uint32_t uiAreaConfigurationRank = getAreaConfigurationRank(*it);

            // Store it
            binaryStream.write((const uint8_t*)&uiAreaConfigurationRank, sizeof(uiAreaConfigurationRank));
        }
    } else {

        // Empty ordered list first
        _orderedAreaConfigurationList.resize(0);

        uint32_t uiAreaConfiguration;

        for (uiAreaConfiguration = 0; uiAreaConfiguration < _areaConfigurationList.size(); uiAreaConfiguration++) {

            // Get rank
            uint32_t uiAreaConfigurationRank;

            binaryStream.read((uint8_t*)&uiAreaConfigurationRank, sizeof(uiAreaConfigurationRank));

            _orderedAreaConfigurationList.push_back(getAreaConfiguration(uiAreaConfigurationRank));
        }
    }

    // Propagate to areas
    for (it = _areaConfigurationList.begin(); it != _areaConfigurationList.end(); ++it) {

        CAreaConfiguration* pAreaConfiguration = *it;

        pAreaConfiguration->serialize(binaryStream);
    }
}

// Data size
uint32_t CDomainConfiguration::getDataSize() const
{
    uint32_t uiDataSize;

    // Add necessary size to store area configurations order
    uiDataSize = _areaConfigurationList.size() * sizeof(uint32_t);

    // Propagate request to areas
    AreaConfigurationListIterator it;

    for (it = _areaConfigurationList.begin(); it != _areaConfigurationList.end(); ++it) {

        const CAreaConfiguration* pAreaConfiguration = *it;

        uiDataSize += pAreaConfiguration->getSize();
    }
    return uiDataSize;
}
