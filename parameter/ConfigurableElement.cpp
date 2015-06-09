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
#include "ConfigurableElement.h"
#include "MappingData.h"
#include "SyncerSet.h"
#include "ConfigurableDomain.h"
#include "ConfigurationAccessContext.h"
#include "ConfigurableElementAggregator.h"
#include "AreaConfiguration.h"
#include "Utility.h"
#include <assert.h>

#define base CElement

CConfigurableElement::CConfigurableElement(const std::string& strName) : base(strName), _uiOffset(0)
{
}

CConfigurableElement::~CConfigurableElement()
{
}

// XML configuration settings parsing
bool CConfigurableElement::serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const
{
    size_t uiIndex;
    size_t uiNbChildren = getNbChildren();

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

// AreaConfiguration creation
CAreaConfiguration* CConfigurableElement::createAreaConfiguration(const CSyncerSet* pSyncerSet) const
{
    return new CAreaConfiguration(this, pSyncerSet);
}

// Parameter access
bool CConfigurableElement::accessValue(CPathNavigator& pathNavigator, std::string& strValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    std::string* pStrChildName = pathNavigator.next();

    if (!pStrChildName) {

        parameterAccessContext.setError((bSet ? "Can't set " : "Can't get ") + pathNavigator.getCurrentPath() + " because it is not a parameter");

        return false;
    }

    const CConfigurableElement* pChild = static_cast<const CConfigurableElement*>(findChild(*pStrChildName));

    if (!pChild) {

        parameterAccessContext.setError("Path not found: " + pathNavigator.getCurrentPath());

        return false;
    }

    return pChild->accessValue(pathNavigator, strValue, bSet, parameterAccessContext);
}

void CConfigurableElement::getListOfElementsWithMapping(
        std::list<const CConfigurableElement*>& configurableElementPath) const
{
    // Check parent
    const CElement* pParent = getParent();
    if (isOfConfigurableElementType(pParent)) {

        const CConfigurableElement* pConfigurableElement =
                static_cast<const CConfigurableElement*>(pParent);

        pConfigurableElement->getListOfElementsWithMapping(configurableElementPath);
    }
}

// Used for simulation and virtual subsystems
void CConfigurableElement::setDefaultValues(CParameterAccessContext& parameterAccessContext) const
{
    // Propagate to children
    size_t uiIndex;
    size_t uiNbChildren = getNbChildren();

    for (uiIndex = 0; uiIndex < uiNbChildren; uiIndex++) {

        const CConfigurableElement* pConfigurableElement = static_cast<const CConfigurableElement*>(getChild(uiIndex));

        pConfigurableElement->setDefaultValues(parameterAccessContext);
    }
}

// Element properties
void CConfigurableElement::showProperties(std::string& strResult) const
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
    size_t uiIndex;
    size_t uiNbChildren = getNbChildren();

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
    size_t uiIndex;
    size_t uiNbChildren = getNbChildren();

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
    return NULL;
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
    size_t uiIndex;
    size_t uiNbChildren = getNbChildren();

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
void CConfigurableElement::getBelongingDomains(std::list<const CConfigurableDomain*>& configurableDomainList) const
{
    configurableDomainList.insert(configurableDomainList.end(), _configurableDomainList.begin(), _configurableDomainList.end());

    // Check parent
    const CElement* pParent = getParent();

    if (isOfConfigurableElementType(pParent)) {

        static_cast<const CConfigurableElement*>(pParent)->getBelongingDomains(configurableDomainList);
    }
}

void CConfigurableElement::listBelongingDomains(std::string& strResult, bool bVertical) const
{
    // Get belonging domain list
    std::list<const CConfigurableDomain*> configurableDomainList;

    getBelongingDomains(configurableDomainList);

    // Fill list
    listDomains(configurableDomainList, strResult, bVertical);
}

// Elements with no domains
void CConfigurableElement::listRogueElements(std::string& strResult) const
{
    strResult = "\n";

    // Get rogue element aggregate list (no associated domain)
    std::list<const CConfigurableElement*> rogueElementList;

    CConfigurableElementAggregator configurableElementAggregator(rogueElementList, &CConfigurableElement::hasNoDomainAssociated);

    configurableElementAggregator.aggegate(this);

    // Build list as std::string
    std::list<const CConfigurableElement*>::const_iterator it;

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
std::string CConfigurableElement::getFootprintAsString() const
{
    // Get size as string
    return CUtility::toString(getFootPrint()) + " byte(s)";
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
void CConfigurableElement::listAssociatedDomains(std::string& strResult, bool bVertical) const
{
    // Fill list
    listDomains(_configurableDomainList, strResult, bVertical);
}

size_t CConfigurableElement::getBelongingDomainCount() const
{
    // Get belonging domain list
    std::list<const CConfigurableDomain*> configurableDomainList;

    getBelongingDomains(configurableDomainList);

    return configurableDomainList.size();
}

void CConfigurableElement::listDomains(const std::list<const CConfigurableDomain*>& configurableDomainList, std::string& strResult, bool bVertical) const
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

    // Stop at system class
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
