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
#include <assert.h>

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
bool CDomainConfiguration::serializeXmlSettings(const CConfigurableElement* pConfigurableElement, CXmlElement& xmlConfigurationSettingsElement, CConfigurationAccessContext& configurationAccessContext)
{
    // Find related AreaConfiguration
    CAreaConfiguration* pAreaConfiguration = getAreaConfiguration(pConfigurableElement);

    assert(pAreaConfiguration);

    // Delegate to corresponding area configuration
    return pAreaConfiguration->serializeXmlSettings(xmlConfigurationSettingsElement, configurationAccessContext);
}

// Configurable Elements association
void CDomainConfiguration::addConfigurableElement(const CConfigurableElement* pConfigurableElement)
{
    _areaConfigurationList.push_back(new CAreaConfiguration(pConfigurableElement));
}

void CDomainConfiguration::removeConfigurableElement(const CConfigurableElement* pConfigurableElement)
{
    CAreaConfiguration* pAreaConfigurationToRemove = getAreaConfiguration(pConfigurableElement);

    _areaConfigurationList.remove(pAreaConfigurationToRemove);

    delete pAreaConfigurationToRemove;
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
void CDomainConfiguration::restore(CParameterBlackboard* pMainBlackboard) const
{
    AreaConfigurationListIterator it;

    // Just propagate to areas
    for (it = _areaConfigurationList.begin(); it != _areaConfigurationList.end(); ++it) {

        const CAreaConfiguration* pAreaConfiguration = *it;

        pAreaConfiguration->restore(pMainBlackboard);
    }
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

// Presence of application condition
bool CDomainConfiguration::hasRule() const
{
    return !!getRule();
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

// Serialization
void CDomainConfiguration::binarySerialize(CBinaryStream& binaryStream)
{
    AreaConfigurationListIterator it;

    // Just propagate to areas
    for (it = _areaConfigurationList.begin(); it != _areaConfigurationList.end(); ++it) {

        CAreaConfiguration* pAreaConfiguration = *it;

        pAreaConfiguration->serialize(binaryStream);
    }
}

// Data size
uint32_t CDomainConfiguration::getDataSize() const
{
    uint32_t uiDataSize = 0;
    AreaConfigurationListIterator it;

    // Just propagate request to areas
    for (it = _areaConfigurationList.begin(); it != _areaConfigurationList.end(); ++it) {

        const CAreaConfiguration* pAreaConfiguration = *it;

        uiDataSize += pAreaConfiguration->getSize();
    }
    return uiDataSize;
}
