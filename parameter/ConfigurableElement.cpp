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
#include "ConfigurableElement.h"
#include "MappingData.h"
#include "SyncerSet.h"
#include "ConfigurableDomain.h"
#include "ConfigurationAccessContext.h"
#include "ConfigurableElementAggregator.h"
#include <assert.h>

#define base CElement

CConfigurableElement::CConfigurableElement(const string& strName) : base(strName), _uiOffset(0)
{
}

CConfigurableElement::~CConfigurableElement()
{
}

// XML configuration settings parsing
bool CConfigurableElement::serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const
{
    uint32_t uiIndex;
    uint32_t uiNbChildren = getNbChildren();

    if (!configurationAccessContext.serializeOut()) {
        // Just do basic checks and propagate to children
        CXmlElement::CChildIterator it(xmlConfigurationSettingsElementContent);

        CXmlElement xmlChildConfigurableElementSettingsElement;

        // Propagate to children
        for (uiIndex = 0; uiIndex < uiNbChildren; uiIndex++) {

            // Get child
            const CConfigurableElement* pChildConfigurableElement = static_cast<const CConfigurableElement*>(getChild(uiIndex));

            if (!it.next(xmlChildConfigurableElementSettingsElement)) {

                // Structure error
                configurationAccessContext.setError("Configuration settings parsing: Settings don't conform to structure of configurable element " + getName());

                return false;
            }

            // Check element type matches in type
            if (xmlChildConfigurableElementSettingsElement.getType() != pChildConfigurableElement->getKind()) {

                // Type error
                configurationAccessContext.setError("Configuration settings parsing: Settings for configurable element " + pChildConfigurableElement->getName() + " does not match expected type: " + xmlChildConfigurableElementSettingsElement.getType() + " instead of " + pChildConfigurableElement->getKind());

                return false;
            }

            // Check element type matches in name
            if (xmlChildConfigurableElementSettingsElement.getNameAttribute() != pChildConfigurableElement->getName()) {

                // Name error
                configurationAccessContext.setError("Configuration settings parsing: Under configurable elememnt " + getName() + ", expected element name " + pChildConfigurableElement->getName() + " but found " + xmlChildConfigurableElementSettingsElement.getNameAttribute() + " instead");

                return false;
            }

            // Parse child configurable element's settings
            if (!pChildConfigurableElement->serializeXmlSettings(xmlChildConfigurableElementSettingsElement, configurationAccessContext)) {

                return false;
            }
        }
        // There should remain no configurable element to parse
        if (it.next(xmlChildConfigurableElementSettingsElement)) {

            // Structure error
            configurationAccessContext.setError("Configuration settings parsing: Settings don't conform to structure of configurable element " + getName());

            return false;
        }
    } else {
        // Propagate to children
        for (uiIndex = 0; uiIndex < uiNbChildren; uiIndex++) {

            const CConfigurableElement* pChildConfigurableElement = static_cast<const CConfigurableElement*>(getChild(uiIndex));

            // Create corresponding child element
            CXmlElement xmlChildConfigurableElementSettingsElement;

            xmlConfigurationSettingsElementContent.createChild(xmlChildConfigurableElementSettingsElement, pChildConfigurableElement->getKind());

            // Handle element name attribute
            xmlChildConfigurableElementSettingsElement.setNameAttribute(pChildConfigurableElement->getName());

            // Propagate
            pChildConfigurableElement->serializeXmlSettings(xmlChildConfigurableElementSettingsElement, configurationAccessContext);
        }
    }
    // Done
    return true;
}

// Parameter access
bool CConfigurableElement::accessValue(CPathNavigator& pathNavigator, string& strValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    string* pStrChildName = pathNavigator.next();

    if (!pStrChildName) {

        parameterAccessContext.setError("Non accessible element");

        return false;
    }

    const CConfigurableElement* pChild = static_cast<const CConfigurableElement*>(findChild(*pStrChildName));

    if (!pChild) {

        parameterAccessContext.setError("Path not found: " + pathNavigator.getCurrentPath());

        return false;
    }

    return pChild->accessValue(pathNavigator, strValue, bSet, parameterAccessContext);
}

// Used for simulation and virtual subsystems
void CConfigurableElement::setDefaultValues(CParameterAccessContext& parameterAccessContext) const
{
    // Propagate to children
    uint32_t uiIndex;
    uint32_t uiNbChildren = getNbChildren();

    for (uiIndex = 0; uiIndex < uiNbChildren; uiIndex++) {

        const CConfigurableElement* pConfigurableElement = static_cast<const CConfigurableElement*>(getChild(uiIndex));

        pConfigurableElement->setDefaultValues(parameterAccessContext);
    }
}

// Element properties
void CConfigurableElement::showProperties(string& strResult) const
{
    base::showProperties(strResult);

    strResult += "Total size: " + getFootprintAsString() + "\n";
}

// Offset
void CConfigurableElement::setOffset(uint32_t uiOffset)
{
    // Assign offset locally
    _uiOffset = uiOffset;

    // Propagate to children
    uint32_t uiIndex;
    uint32_t uiNbChildren = getNbChildren();

    for (uiIndex = 0; uiIndex < uiNbChildren; uiIndex++) {

        CConfigurableElement* pConfigurableElement = static_cast<CConfigurableElement*>(getChild(uiIndex));

        pConfigurableElement->setOffset(uiOffset);

        uiOffset += pConfigurableElement->getFootPrint();
    }
}

uint32_t CConfigurableElement::getOffset() const
{
    return _uiOffset;
}

// Memory
uint32_t CConfigurableElement::getFootPrint() const
{
    uint32_t uiSize = 0;
    uint32_t uiIndex;
    uint32_t uiNbChildren = getNbChildren();

    for (uiIndex = 0; uiIndex < uiNbChildren; uiIndex++) {

        const CConfigurableElement* pConfigurableElement = static_cast<const CConfigurableElement*>(getChild(uiIndex));

        uiSize += pConfigurableElement->getFootPrint();
    }

    return uiSize;
}

// Browse parent path to find syncer
ISyncer* CConfigurableElement::getSyncer() const
{
    // Check parent
    const CElement* pParent = getParent();

    if (isOfConfigurableElementType(pParent)) {

        return static_cast<const CConfigurableElement*>(pParent)->getSyncer();
    }
    return false;
}

// Syncer set (me, ascendant or descendant ones)
void CConfigurableElement::fillSyncerSet(CSyncerSet& syncerSet) const
{
    //  Try me or ascendants
    ISyncer* pMineOrAscendantSyncer = getSyncer();

    if (pMineOrAscendantSyncer) {

        // Provide found syncer object
        syncerSet += pMineOrAscendantSyncer;

        // Done
        return;
    }
    // Fetch descendant ones
    fillSyncerSetFromDescendant(syncerSet);
}

// Syncer set (descendant)
void CConfigurableElement::fillSyncerSetFromDescendant(CSyncerSet& syncerSet) const
{
    // Dig
    uint32_t uiIndex;
    uint32_t uiNbChildren = getNbChildren();

    for (uiIndex = 0; uiIndex < uiNbChildren; uiIndex++) {

        const CConfigurableElement* pConfigurableElement = static_cast<const CConfigurableElement*>(getChild(uiIndex));

        pConfigurableElement->fillSyncerSetFromDescendant(syncerSet);
    }
}

// Configurable domain association
void CConfigurableElement::addAttachedConfigurableDomain(const CConfigurableDomain* pConfigurableDomain)
{
    _configurableDomainList.push_back(pConfigurableDomain);
}

void CConfigurableElement::removeAttachedConfigurableDomain(const CConfigurableDomain* pConfigurableDomain)
{
    _configurableDomainList.remove(pConfigurableDomain);
}

// Belonging domain
bool CConfigurableElement::belongsTo(const CConfigurableDomain* pConfigurableDomain) const
{
    if (containsConfigurableDomain(pConfigurableDomain)) {

        return true;
    }
    return belongsToDomainAscending(pConfigurableDomain);
}

// Belonging domains
void CConfigurableElement::getBelongingDomains(list<const CConfigurableDomain*>& configurableDomainList) const
{
    configurableDomainList.insert(configurableDomainList.end(), _configurableDomainList.begin(), _configurableDomainList.end());

    // Check parent
    const CElement* pParent = getParent();

    if (isOfConfigurableElementType(pParent)) {

        static_cast<const CConfigurableElement*>(pParent)->getBelongingDomains(configurableDomainList);
    }
}

void CConfigurableElement::listBelongingDomains(string& strResult, bool bVertical) const
{
    // Get belonging domain list
    list<const CConfigurableDomain*> configurableDomainList;

    getBelongingDomains(configurableDomainList);

    // Fill list
    listDomains(configurableDomainList, strResult, bVertical);
}

// Elements with no domains
void CConfigurableElement::listRogueElements(string& strResult) const
{
    strResult = "\n";

    // Get rogue element aggregate list (no associated domain)
    list<const CConfigurableElement*> rogueElementList;

    CConfigurableElementAggregator configurableElementAggregator(rogueElementList, &CConfigurableElement::hasNoDomainAssociated);

    configurableElementAggregator.aggegate(this);

    // Build list as string
    list<const CConfigurableElement*>::const_iterator it;

    for (it = rogueElementList.begin(); it != rogueElementList.end(); ++it) {

        const CConfigurableElement* pConfigurableElement = *it;

        strResult += pConfigurableElement->getPath() + "\n";
    }
}

// Belonging to no domains
bool CConfigurableElement::isRogue() const
{
    return !getBelongingDomainCount();
}

// Footprint as string
string CConfigurableElement::getFootprintAsString() const
{
    // Get size as string
    return toString(getFootPrint()) + " byte(s)";
}

// Matching check for no domain association
bool CConfigurableElement::hasNoDomainAssociated() const
{
    return _configurableDomainList.empty();
}

// Matching check for no valid associated domains
bool CConfigurableElement::hasNoValidDomainAssociated() const
{
    if (_configurableDomainList.empty()) {

        // No domains associated
        return true;
    }

    ConfigurableDomainListConstIterator it;

    // Browse all configurable domains for validity checking
    for (it = _configurableDomainList.begin(); it != _configurableDomainList.end(); ++it) {

        const CConfigurableDomain* pConfigurableDomain = *it;

        if (pConfigurableDomain->isApplicableConfigurationValid(this)) {

            return false;
        }
    }

    return true;
}

// Owning domains
void CConfigurableElement::listAssociatedDomains(string& strResult, bool bVertical) const
{
    // Fill list
    listDomains(_configurableDomainList, strResult, bVertical);
}

uint32_t CConfigurableElement::getBelongingDomainCount() const
{
    // Get belonging domain list
    list<const CConfigurableDomain*> configurableDomainList;

    getBelongingDomains(configurableDomainList);

    return configurableDomainList.size();
}

void CConfigurableElement::listDomains(const list<const CConfigurableDomain*>& configurableDomainList, string& strResult, bool bVertical) const
{
    if (bVertical && configurableDomainList.empty()) {

        strResult = "\n";
    }

    // Fill list
    ConfigurableDomainListConstIterator it;
    bool bFirst = true;

    // Browse all configurable domains for comparison
    for (it = configurableDomainList.begin(); it != configurableDomainList.end(); ++it) {

        const CConfigurableDomain* pConfigurableDomain = *it;

        if (!bVertical && !bFirst) {

            strResult += ", ";
        }

        strResult += pConfigurableDomain->getName();

        if (bVertical) {

            strResult += "\n";
        } else {

            bFirst = false;
        }
    }
}

bool CConfigurableElement::containsConfigurableDomain(const CConfigurableDomain* pConfigurableDomain) const
{
    ConfigurableDomainListConstIterator it;

    // Browse all configurable domains for comparison
    for (it = _configurableDomainList.begin(); it != _configurableDomainList.end(); ++it) {

        if (pConfigurableDomain == *it) {

            return true;
        }
    }
    return false;
}

// Belonging domain ascending search
bool CConfigurableElement::belongsToDomainAscending(const CConfigurableDomain* pConfigurableDomain) const
{
    // Check parent
    const CElement* pParent = getParent();

    if (isOfConfigurableElementType(pParent)) {

        return static_cast<const CConfigurableElement*>(pParent)->belongsTo(pConfigurableDomain);
    }
    return false;
}

// Belonging subsystem
const CSubsystem* CConfigurableElement::getBelongingSubsystem() const
{
    const CElement* pParent = getParent();

    // Stop at sytem class
    if (!pParent->getParent()) {

        return NULL;
    }

    return static_cast<const CConfigurableElement*>(pParent)->getBelongingSubsystem();
}

// Check element is a parameter
bool CConfigurableElement::isParameter() const
{
    return false;
}


// Check parent is still of current type (by structure knowledge)
bool CConfigurableElement::isOfConfigurableElementType(const CElement* pParent) const
{
    assert(pParent);

    // Up to system class
    return !!pParent->getParent();
}
