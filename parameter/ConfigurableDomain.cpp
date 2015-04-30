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
#include "ConfigurableDomain.h"
#include "DomainConfiguration.h"
#include "ConfigurableElement.h"
#include "ConfigurationAccessContext.h"
#include "XmlDomainSerializingContext.h"
#include "XmlDomainImportContext.h"
#include "XmlDomainExportContext.h"
#include <assert.h>

#define base CBinarySerializableElement

using std::string;

CConfigurableDomain::CConfigurableDomain() :
    _bSequenceAware(false), _pLastAppliedConfiguration(NULL)
{
}

CConfigurableDomain::CConfigurableDomain(const string& strName) : base(strName), _bSequenceAware(false), _pLastAppliedConfiguration(NULL)
{
}

CConfigurableDomain::~CConfigurableDomain()
{
    // Remove all configurable elements
    ConfigurableElementListIterator it;

    for (it = _configurableElementList.begin(); it != _configurableElementList.end(); ++it) {

        CConfigurableElement* pConfigurableElement = *it;

        // Remove from configurable element
        pConfigurableElement->removeAttachedConfigurableDomain(this);
    }

    // Remove all associated syncer sets
    ConfigurableElementToSyncerSetMapIterator mapIt;

    for (mapIt = _configurableElementToSyncerSetMap.begin(); mapIt != _configurableElementToSyncerSetMap.end(); ++mapIt) {

        delete mapIt->second;
    }
}

string CConfigurableDomain::getKind() const
{
    return "ConfigurableDomain";
}

bool CConfigurableDomain::childrenAreDynamic() const
{
    return true;
}

// Content dumping
void CConfigurableDomain::logValue(string& strValue, CErrorContext& errorContext) const
{
    (void)errorContext;

    strValue = "{";

    // Sequence awareness
    strValue += "Sequence aware: ";
    strValue += _bSequenceAware ? "yes" : "no";

    // Last applied configuration
    strValue += ", Last applied configuration: ";
    strValue += _pLastAppliedConfiguration ? _pLastAppliedConfiguration->getName() : "<none>";

    strValue += "}";
}

// Sequence awareness
void CConfigurableDomain::setSequenceAwareness(bool bSequenceAware)
{
    if (_bSequenceAware != bSequenceAware) {

        log_info("Making domain \"%s\" sequence %s", getName().c_str(), bSequenceAware ? "aware" : "unaware");

        _bSequenceAware = bSequenceAware;
    }
}

bool CConfigurableDomain::getSequenceAwareness() const
{
    return _bSequenceAware;
}

// From IXmlSource
void CConfigurableDomain::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    base::toXml(xmlElement, serializingContext);

    // Sequence awareness
    xmlElement.setAttributeBoolean("SequenceAware", _bSequenceAware);
}

void CConfigurableDomain::childrenToXml(CXmlElement& xmlElement,
                                        CXmlSerializingContext& serializingContext) const
{
    // Configurations
    composeDomainConfigurations(xmlElement, serializingContext);

    // Configurable Elements
    composeConfigurableElements(xmlElement);

    // Settings
    composeSettings(xmlElement, serializingContext);
}

// XML composing
void CConfigurableDomain::composeDomainConfigurations(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Create Configurations element
    CXmlElement xmlConfigurationsElement;

    xmlElement.createChild(xmlConfigurationsElement, "Configurations");

    // Delegate to base
    base::childrenToXml(xmlConfigurationsElement, serializingContext);
}

void CConfigurableDomain::composeConfigurableElements(CXmlElement& xmlElement) const
{
    // Create ConfigurableElements element
    CXmlElement xmlConfigurableElementsElement;

    xmlElement.createChild(xmlConfigurableElementsElement, "ConfigurableElements");

    // Serialize out all configurable elements settings
    ConfigurableElementListIterator it;

    for (it = _configurableElementList.begin(); it != _configurableElementList.end(); ++it) {

        const CConfigurableElement* pConfigurableElement = *it;

        // Create corresponding XML child element
        CXmlElement xmlChildConfigurableElement;

        xmlConfigurableElementsElement.createChild(xmlChildConfigurableElement, "ConfigurableElement");

        // Set Path attribute
        xmlChildConfigurableElement.setAttributeString("Path", pConfigurableElement->getPath());
    }
}

void CConfigurableDomain::composeSettings(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Context
    const CXmlDomainExportContext& xmlDomainExportContext =
        static_cast<const CXmlDomainExportContext&>(serializingContext);

    if (!xmlDomainExportContext.withSettings()) {

        return;
    }

    // Create Settings element
    CXmlElement xmlSettingsElement;

    xmlElement.createChild(xmlSettingsElement, "Settings");

    // Serialize out all configurations settings
    size_t uiNbConfigurations = getNbChildren();
    size_t uiChildConfiguration;

    for (uiChildConfiguration = 0; uiChildConfiguration < uiNbConfigurations; uiChildConfiguration++) {

        const CDomainConfiguration* pDomainConfiguration = static_cast<const CDomainConfiguration*>(getChild(uiChildConfiguration));

        // Create child xml element for that configuration
        CXmlElement xmlConfigurationSettingsElement;

        xmlSettingsElement.createChild(xmlConfigurationSettingsElement, pDomainConfiguration->getKind());

        // Set its name attribute
        xmlConfigurationSettingsElement.setNameAttribute(pDomainConfiguration->getName());

        // Serialize out configuration settings
        pDomainConfiguration->composeSettings(xmlConfigurationSettingsElement, serializingContext);
    }
}

// From IXmlSink
bool CConfigurableDomain::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Context
    CXmlDomainImportContext& xmlDomainImportContext =
        static_cast<CXmlDomainImportContext&>(serializingContext);

    // Sequence awareness (optional)
    _bSequenceAware = xmlElement.hasAttribute("SequenceAware") && xmlElement.getAttributeBoolean("SequenceAware");

    setName(xmlElement.getAttributeString("Name"));

    // Local parsing. Do not dig
    if (!parseDomainConfigurations(xmlElement, xmlDomainImportContext) ||
        !parseConfigurableElements(xmlElement, xmlDomainImportContext) ||
        !parseSettings(xmlElement, xmlDomainImportContext)) {

        return false;
    }

    // All provided configurations are parsed
    // Attempt validation on areas of non provided configurations for all configurable elements if required
    if (xmlDomainImportContext.autoValidationRequired()) {

        autoValidateAll();
    }

    return true;
}

// XML parsing
bool CConfigurableDomain::parseDomainConfigurations(const CXmlElement& xmlElement,
                                                    CXmlDomainImportContext& serializingContext)
{
    // We're supposedly clean
    assert(_configurableElementList.empty());

    // Get Configurations element
    CXmlElement xmlConfigurationsElement;

    xmlElement.getChildElement("Configurations", xmlConfigurationsElement);

    // Parse it and create domain configuration objects
    return base::fromXml(xmlConfigurationsElement, serializingContext);
}

// Parse configurable elements
bool CConfigurableDomain::parseConfigurableElements(const CXmlElement& xmlElement,
                                                    CXmlDomainImportContext& serializingContext)
{
    CSystemClass& systemClass = serializingContext.getSystemClass();

    // Get ConfigurableElements element
    CXmlElement xmlConfigurableElementsElement;
    xmlElement.getChildElement("ConfigurableElements", xmlConfigurableElementsElement);

    // Parse it and associate found configurable elements to it
    CXmlElement::CChildIterator it(xmlConfigurableElementsElement);

    CXmlElement xmlConfigurableElementElement;

    while (it.next(xmlConfigurableElementElement)) {

        // Locate configurable element
        string strConfigurableElementPath = xmlConfigurableElementElement.getAttributeString("Path");

        CPathNavigator pathNavigator(strConfigurableElementPath);
        string strError;

        // Is there an element and does it match system class name?
        if (!pathNavigator.navigateThrough(systemClass.getName(), strError)) {

            serializingContext.setError("Could not find configurable element of path " + strConfigurableElementPath + " from ConfigurableDomain description " + getName() + " (" + strError + ")");

            return false;
        }
        // Browse system class for configurable element
        CConfigurableElement* pConfigurableElement =
            static_cast<CConfigurableElement*>(systemClass.findDescendant(pathNavigator));

        if (!pConfigurableElement) {

            serializingContext.setError("Could not find configurable element of path " + strConfigurableElementPath + " from ConfigurableDomain description " + getName());

            return false;
        }
        // Add found element to domain
        if (!addConfigurableElement(pConfigurableElement, NULL, strError)) {

            serializingContext.setError(strError);

            return false;
        }
    }

    return true;
}

// Parse settings
bool CConfigurableDomain::parseSettings(const CXmlElement& xmlElement,
                                        CXmlDomainImportContext& serializingContext)
{
    // Check we actually need to parse configuration settings
    if (!serializingContext.withSettings()) {

        // No parsing required
        return true;
    }

    // Get Settings element
    CXmlElement xmlSettingsElement;
    if (!xmlElement.getChildElement("Settings", xmlSettingsElement)) {

        // No settings, bail out successfully
        return true;
    }

    // Parse configuration settings
    CXmlElement::CChildIterator it(xmlSettingsElement);

    CXmlElement xmlConfigurationSettingsElement;

    while (it.next(xmlConfigurationSettingsElement)) {
        // Get domain configuration
        CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(findChild(xmlConfigurationSettingsElement.getNameAttribute()));

        if (!pDomainConfiguration) {

            serializingContext.setError("Could not find domain configuration referred to by"
                                        " configurable domain \"" + getName() + "\".");

            return false;
        }
        // Have domain configuration parse settings for all configurable elements
        if (!pDomainConfiguration->parseSettings(xmlConfigurationSettingsElement,
                                                 serializingContext)) {

            return false;
        }
    }

    return true;
}
// Configurable elements association
bool CConfigurableDomain::addConfigurableElement(CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard, string& strError)
{
    // Already associated?
    if (containsConfigurableElement(pConfigurableElement)) {

        strError = "Configurable element " + pConfigurableElement->getPath() + " already associated to configuration domain " + getName();

        return false;
    }

    // Already owned?
    if (pConfigurableElement->belongsTo(this)) {

        strError = "Configurable element " + pConfigurableElement->getPath() + " already owned by configuration domain " + getName();

        return false;
    }

    // Do add
    doAddConfigurableElement(pConfigurableElement, pMainBlackboard);

    return true;
}

bool CConfigurableDomain::removeConfigurableElement(CConfigurableElement* pConfigurableElement, string& strError)
{
    // Not associated?
    if (!containsConfigurableElement(pConfigurableElement)) {

        strError = "Configurable element " + pConfigurableElement->getPath() + " not associated to configuration domain " + getName();

        return false;
    }
    log_info("Removing configurable element \"%s\" from domain \"%s\"", pConfigurableElement->getPath().c_str(), getName().c_str());

    // Do remove
    doRemoveConfigurableElement(pConfigurableElement, true);

    return true;
}

/**
* Blackboard Configuration and Base Offset retrieval.
*
* This method fetches the Blackboard associated to the ConfigurableElement
* given in parameter, for a specific Configuration. The ConfigurableElement
* must belong to the Domain. If a Blackboard is found, the base offset of
* the ConfigurableElement is returned as well. This base offset corresponds to
* the offset of the ancestor of the ConfigurableElement associated to the Configuration.
*
* @param[in] strConfiguration                           Name of the Configuration.
* @param[in] pCandidateDescendantConfigurableElement    Pointer to a CConfigurableElement that
*                                                       belongs to the Domain.
* @param[out] uiBaseOffset                              The base offset of the CConfigurableElement.
* @param[out] bIsLastApplied                            Boolean indicating that the Configuration is
*                                                       the last one applied of the Domain.
* @param[out] strError                                  Error message
*
* return Pointer to the Blackboard of the Configuration.
*/
CParameterBlackboard* CConfigurableDomain::findConfigurationBlackboard(const string& strConfiguration,
                                                                       const CConfigurableElement* pCandidateDescendantConfigurableElement,
                                                                       uint32_t& uiBaseOffset,
                                                                       bool& bIsLastApplied,
                                                                       string& strError) const
{
    // Find Configuration
    const CDomainConfiguration* pDomainConfiguration = static_cast<const CDomainConfiguration*>(findChild(strConfiguration));

    if (!pDomainConfiguration) {

        strError = "Domain configuration " + strConfiguration + " not found";

        return NULL;
    }

    // Parse all configurable elements
    ConfigurableElementListIterator it;

    for (it = _configurableElementList.begin(); it != _configurableElementList.end(); ++it) {

        const CConfigurableElement* pAssociatedConfigurableElement = *it;

        // Check if the the associated element is the configurable element or one of its ancestors
        if ((pCandidateDescendantConfigurableElement == pAssociatedConfigurableElement) ||
            (pCandidateDescendantConfigurableElement->isDescendantOf(pAssociatedConfigurableElement))) {

            uiBaseOffset = pAssociatedConfigurableElement->getOffset();
            bIsLastApplied = (pDomainConfiguration == _pLastAppliedConfiguration);

            return pDomainConfiguration->getBlackboard(pAssociatedConfigurableElement);
        }
    }

    strError = "Element not associated to the Domain";

    return NULL;
}

// Domain splitting
bool CConfigurableDomain::split(CConfigurableElement* pConfigurableElement, string& strError)
{
    // Not associated?
    if (!containsConfigurableElement(pConfigurableElement)) {

        strError = "Configurable element " + pConfigurableElement->getPath() + " not associated to configuration domain " + getName();

        return false;
    }
    log_info("Splitting configurable element \"%s\" domain \"%s\"", pConfigurableElement->getPath().c_str(), getName().c_str());

    // Create sub domain areas for all configurable element's children
    size_t uiNbConfigurableElementChildren = pConfigurableElement->getNbChildren();

    if (!uiNbConfigurableElementChildren) {

        strError = "Configurable element " + pConfigurableElement->getPath() + " has no children to split configurable domain to";

        return false;
    }

    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurableElementChildren; uiChild++) {

        CConfigurableElement* pChildConfigurableElement = static_cast<CConfigurableElement*>(pConfigurableElement->getChild(uiChild));

        doAddConfigurableElement(pChildConfigurableElement);
    }

    // Delegate to configurations
    size_t uiNbConfigurations = getNbChildren();

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(getChild(uiChild));

        pDomainConfiguration->split(pConfigurableElement);
    }

    // Remove given configurable element from this domain
    // Note: we shouldn't need to recompute the sync set in that case, as the splitted element should include the syncers of its children elements
    doRemoveConfigurableElement(pConfigurableElement, false);

    return true;
}

// Check if there is a pending configuration for this domain: i.e. an applicable configuration different from the last applied configuration
const CDomainConfiguration* CConfigurableDomain::getPendingConfiguration() const
{
    const CDomainConfiguration* pApplicableDomainConfiguration = findApplicableDomainConfiguration();

    if (pApplicableDomainConfiguration) {

        // Check not the last one before applying
        if (!_pLastAppliedConfiguration || (_pLastAppliedConfiguration != pApplicableDomainConfiguration)) {

            return pApplicableDomainConfiguration;
        }
    }

    return NULL;
}

// Configuration application if required
void CConfigurableDomain::apply(CParameterBlackboard* pParameterBlackboard, CSyncerSet* pSyncerSet, bool bForce) const
{
    // Apply configuration only if the blackboard will
    // be synchronized either now or by syncerSet.
    if(!pSyncerSet ^ _bSequenceAware) {
        // The configuration can not be syncronised
        return;
    }

    if (bForce) {
        // Force a configuration restore by forgetting about last applied configuration
        _pLastAppliedConfiguration = NULL;
    }
    const CDomainConfiguration* pApplicableDomainConfiguration = findApplicableDomainConfiguration();

    if (pApplicableDomainConfiguration) {

        // Check not the last one before applying
        if (!_pLastAppliedConfiguration || _pLastAppliedConfiguration != pApplicableDomainConfiguration) {

            log_info("Applying configuration \"%s\" from domain \"%s\"",
                     pApplicableDomainConfiguration->getName().c_str(),
                     getName().c_str());

            // Check if we need to synchronize during restore
            bool bSync = !pSyncerSet && _bSequenceAware;

            // Do the restore
            pApplicableDomainConfiguration->restore(pParameterBlackboard, bSync, NULL);

            // Record last applied configuration
            _pLastAppliedConfiguration = pApplicableDomainConfiguration;

            // Check we need to provide syncer set to caller
            if (pSyncerSet && !_bSequenceAware) {

                // Since we applied changes, add our own sync set to the given one
                *pSyncerSet += _syncerSet;
            }
        }
    }
}

// Return applicable configuration validity for given configurable element
bool CConfigurableDomain::isApplicableConfigurationValid(const CConfigurableElement* pConfigurableElement) const
{
    const CDomainConfiguration* pApplicableDomainConfiguration = findApplicableDomainConfiguration();

    return pApplicableDomainConfiguration && pApplicableDomainConfiguration->isValid(pConfigurableElement);
}

// In case configurable element was removed
void CConfigurableDomain::computeSyncSet()
{
    // Clean sync set first
    _syncerSet.clear();

    // Add syncer sets for all associated configurable elements
    ConfigurableElementToSyncerSetMapIterator mapIt;

    for (mapIt = _configurableElementToSyncerSetMap.begin(); mapIt != _configurableElementToSyncerSetMap.end(); ++mapIt) {

        const CSyncerSet* pSyncerSet = mapIt->second;

        _syncerSet += *pSyncerSet;
    }
}

// Configuration Management
bool CConfigurableDomain::createConfiguration(const string& strName, const CParameterBlackboard* pMainBlackboard, string& strError)
{
    // Already exists?
    if (findChild(strName)) {

        strError = "Already existing configuration";

        return false;
    }
    log_info("Creating domain configuration \"%s\" into domain \"%s\"", strName.c_str(), getName().c_str());

    // Creation
    CDomainConfiguration* pDomainConfiguration = new CDomainConfiguration(strName);

    // Configurable elements association
    ConfigurableElementListIterator it;

    for (it = _configurableElementList.begin(); it != _configurableElementList.end(); ++it) {

        const CConfigurableElement* pConfigurableElement = *it;;

        // Retrieve associated syncer set
        CSyncerSet* pSyncerSet = getSyncerSet(pConfigurableElement);

        // Associate to configuration
        pDomainConfiguration->addConfigurableElement(pConfigurableElement, pSyncerSet);
    }

    // Hierarchy
    addChild(pDomainConfiguration);

    // Ensure validity of fresh new domain configuration
    // Attempt auto validation, so that the user gets his/her own settings by defaults
    if (!autoValidateConfiguration(pDomainConfiguration)) {

        // No valid configuration found to copy in from, validate againt main blackboard (will concerned remaining invalid parts)
        pDomainConfiguration->validate(pMainBlackboard);
    }

    return true;
}

bool CConfigurableDomain::deleteConfiguration(const string& strName, string& strError)
{
    CDomainConfiguration* pDomainConfiguration = findConfiguration(strName, strError);

    if (!pDomainConfiguration) {

        return false;
    }

    log_info("Deleting configuration \"%s\" from domain \"%s\"", strName.c_str(), getName().c_str());

    // Was the last applied?
    if (pDomainConfiguration == _pLastAppliedConfiguration) {

        // Forget about it
        _pLastAppliedConfiguration = NULL;
    }

    // Hierarchy
    removeChild(pDomainConfiguration);

    // Destroy
    delete pDomainConfiguration;

    return true;
}

void CConfigurableDomain::listAssociatedToElements(string& strResult) const
{
    strResult = "\n";

    ConfigurableElementListIterator it;

    // Browse all configurable elements
    for (it = _configurableElementList.begin(); it != _configurableElementList.end(); ++it) {

        const CConfigurableElement* pConfigurableElement = *it;

        strResult += pConfigurableElement->getPath() + "\n";
    }
}

bool CConfigurableDomain::renameConfiguration(const string& strName, const string& strNewName, string& strError)
{
    CDomainConfiguration* pDomainConfiguration = findConfiguration(strName, strError);

    if (!pDomainConfiguration) {

        return false;
    }
    log_info("Renaming domain \"%s\"'s configuration \"%s\" to \"%s\"", getName().c_str(), strName.c_str(), strNewName.c_str());

    // Rename
    return pDomainConfiguration->rename(strNewName, strError);
}

bool CConfigurableDomain::restoreConfiguration(const string& strName, CParameterBlackboard* pMainBlackboard, bool bAutoSync, std::list<string>& lstrError) const
{
    string strError;

    const CDomainConfiguration* pDomainConfiguration = findConfiguration(strName, strError);

    if (!pDomainConfiguration) {

        lstrError.push_back(strError);
        return false;
    }
    log_info("Restoring domain \"%s\"'s configuration \"%s\" to parameter blackboard", getName().c_str(), pDomainConfiguration->getName().c_str());

    // Delegate
    bool bSuccess = pDomainConfiguration->restore(pMainBlackboard, bAutoSync && _bSequenceAware, &lstrError);

    // Record last applied configuration
    _pLastAppliedConfiguration = pDomainConfiguration;

    // Synchronize
    if (bAutoSync && !_bSequenceAware) {

        bSuccess &= _syncerSet.sync(*pMainBlackboard, false, &lstrError);
    }
    return bSuccess;
}

bool CConfigurableDomain::saveConfiguration(const string& strName, const CParameterBlackboard* pMainBlackboard, string& strError)
{
    // Find Domain configuration
    CDomainConfiguration* pDomainConfiguration = findConfiguration(strName, strError);

    if (!pDomainConfiguration) {

        return false;
    }
    log_info("Saving domain \"%s\"'s configuration \"%s\" from parameter blackboard", getName().c_str(), pDomainConfiguration->getName().c_str());

    // Delegate
    pDomainConfiguration->save(pMainBlackboard);

    return true;
}

bool CConfigurableDomain::setElementSequence(const string& strConfiguration, const std::vector<string>& astrNewElementSequence, string& strError)
{
    // Find Domain configuration
    CDomainConfiguration* pDomainConfiguration = findConfiguration(strConfiguration, strError);

    if (!pDomainConfiguration) {

        return false;
    }

    // Delegate to configuration
    return pDomainConfiguration->setElementSequence(astrNewElementSequence, strError);
}

bool CConfigurableDomain::getElementSequence(const string& strConfiguration, string& strResult) const
{
    // Find Domain configuration
    const CDomainConfiguration* pDomainConfiguration = findConfiguration(strConfiguration, strResult);

    if (!pDomainConfiguration) {

        return false;
    }

    // Delegate to configuration
    pDomainConfiguration->getElementSequence(strResult);

    return true;
}

bool CConfigurableDomain::setApplicationRule(const string& strConfiguration, const string& strApplicationRule, const CSelectionCriteriaDefinition* pSelectionCriteriaDefinition, string& strError)
{
    // Find Domain configuration
    CDomainConfiguration* pDomainConfiguration = findConfiguration(strConfiguration, strError);

    if (!pDomainConfiguration) {

        return false;
    }

    // Delegate to configuration
    return pDomainConfiguration->setApplicationRule(strApplicationRule, pSelectionCriteriaDefinition, strError);
}

bool CConfigurableDomain::clearApplicationRule(const string& strConfiguration, string& strError)
{
    // Find Domain configuration
    CDomainConfiguration* pDomainConfiguration = findConfiguration(strConfiguration, strError);

    if (!pDomainConfiguration) {

        return false;
    }

    // Delegate to configuration
    pDomainConfiguration->clearApplicationRule();

    return true;
}

bool CConfigurableDomain::getApplicationRule(const string& strConfiguration, string& strResult) const
{
    // Find Domain configuration
    const CDomainConfiguration* pDomainConfiguration = findConfiguration(strConfiguration, strResult);

    if (!pDomainConfiguration) {

        return false;
    }

    // Delegate to configuration
    pDomainConfiguration->getApplicationRule(strResult);

    return true;
}

// Last applied configuration
string CConfigurableDomain::getLastAppliedConfigurationName() const
{
    if (_pLastAppliedConfiguration) {

        return _pLastAppliedConfiguration->getName();
    }
    return "<none>";
}

// Pending configuration
string CConfigurableDomain::getPendingConfigurationName() const
{
    const CDomainConfiguration* pPendingConfiguration = getPendingConfiguration();

    if (pPendingConfiguration) {

        return pPendingConfiguration->getName();
    }
    return "<none>";
}

// Ensure validity on whole domain from main blackboard
void CConfigurableDomain::validate(const CParameterBlackboard* pMainBlackboard)
{

    // Propagate
    size_t uiNbConfigurations = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(getChild(uiChild));

        pDomainConfiguration->validate(pMainBlackboard);
    }
}

// Ensure validity on areas related to configurable element
void CConfigurableDomain::validateAreas(const CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard)
{
    log_info("Validating domain \"%s\" against main blackboard for configurable element \"%s\"", getName().c_str(), pConfigurableElement->getPath().c_str());

    // Propagate
    size_t uiNbConfigurations = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(getChild(uiChild));

        pDomainConfiguration->validate(pConfigurableElement, pMainBlackboard);
    }
}

// Attempt validation for all configurable element's areas, relying on already existing valid configuration inside domain
void CConfigurableDomain::autoValidateAll()
{
    // Validate
    ConfigurableElementListIterator it;

    // Browse all configurable elements for configuration validation
    for (it = _configurableElementList.begin(); it != _configurableElementList.end(); ++it) {

        const CConfigurableElement* pConfigurableElement = *it;

        // Auto validate element
        autoValidateAreas(pConfigurableElement);
    }
}

// Attempt validation for configurable element's areas, relying on already existing valid configuration inside domain
void CConfigurableDomain::autoValidateAreas(const CConfigurableElement* pConfigurableElement)
{
    // Find first valid configuration for given configurable element
    const CDomainConfiguration* pValidDomainConfiguration = findValidDomainConfiguration(pConfigurableElement);

    // No valid configuration found, give up
    if (!pValidDomainConfiguration) {

        return;
    }

    // Validate all other configurations against found one, if any
    size_t uiNbConfigurations = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(getChild(uiChild));

        if (pDomainConfiguration != pValidDomainConfiguration && !pDomainConfiguration->isValid(pConfigurableElement)) {
            // Validate
            pDomainConfiguration->validateAgainst(pValidDomainConfiguration, pConfigurableElement);
        }
    }
}

// Attempt configuration validation for all configurable elements' areas, relying on already existing valid configuration inside domain
bool CConfigurableDomain::autoValidateConfiguration(CDomainConfiguration* pDomainConfiguration)
{
    // Find another configuration than this one, that ought to be valid!
    size_t uiNbConfigurations = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        const CDomainConfiguration* pPotententialValidDomainConfiguration = static_cast<const CDomainConfiguration*>(getChild(uiChild));

        if (pPotententialValidDomainConfiguration != pDomainConfiguration) {

            // Validate against it
            pDomainConfiguration->validateAgainst(pPotententialValidDomainConfiguration);

            return true;
        }
    }
    return false;
}

// Search for a valid configuration for given configurable element
const CDomainConfiguration* CConfigurableDomain::findValidDomainConfiguration(const CConfigurableElement* pConfigurableElement) const
{
    size_t uiNbConfigurations = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        const CDomainConfiguration* pDomainConfiguration = static_cast<const CDomainConfiguration*>(getChild(uiChild));

        if (pDomainConfiguration->isValid(pConfigurableElement)) {

            return pDomainConfiguration;
        }
    }
    return NULL;
}

// Search for an applicable configuration
const CDomainConfiguration* CConfigurableDomain::findApplicableDomainConfiguration() const
{
    size_t uiNbConfigurations = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        const CDomainConfiguration* pDomainConfiguration = static_cast<const CDomainConfiguration*>(getChild(uiChild));

        if (pDomainConfiguration->isApplicable()) {

            return pDomainConfiguration;
        }
    }
    return NULL;
}

// Gather set of configurable elements
void CConfigurableDomain::gatherConfigurableElements(std::set<const CConfigurableElement*>& configurableElementSet) const
{
    // Insert all configurable elements
    configurableElementSet.insert(_configurableElementList.begin(), _configurableElementList.end());
}

// Check configurable element already attached
bool CConfigurableDomain::containsConfigurableElement(const CConfigurableElement* pConfigurableCandidateElement) const
{
    ConfigurableElementListIterator it;

    // Browse all configurable elements for comparison
    for (it = _configurableElementList.begin(); it != _configurableElementList.end(); ++it) {

        if (pConfigurableCandidateElement == *it) {

            return true;
        }
    }
    return false;
}

// Merge any descended configurable element to this one with this one
void CConfigurableDomain::mergeAlreadyAssociatedDescendantConfigurableElements(CConfigurableElement* pNewConfigurableElement)
{
    std::list<CConfigurableElement*> mergedConfigurableElementList;

    ConfigurableElementListIterator it;

    // Browse all configurable elements (new one not yet in the list!)
    for (it = _configurableElementList.begin(); it != _configurableElementList.end(); ++it) {

        CConfigurableElement* pConfigurablePotentialDescendantElement = *it;

        if (pConfigurablePotentialDescendantElement->isDescendantOf(pNewConfigurableElement)) {

            log_info("In domain \"%s\", merging descendant configurable element's configurations \"%s\" into its ascendant \"%s\" ones", getName().c_str(), pConfigurablePotentialDescendantElement->getName().c_str(), pNewConfigurableElement->getName().c_str());

            // Merge configuration data
            mergeConfigurations(pNewConfigurableElement, pConfigurablePotentialDescendantElement);

            // Keep track for removal
            mergedConfigurableElementList.push_back(pConfigurablePotentialDescendantElement);
        }
    }

    // Remove all merged elements (new one not yet in the list!)
    for (it = mergedConfigurableElementList.begin(); it != mergedConfigurableElementList.end(); ++it) {

        CConfigurableElement* pMergedConfigurableElement = *it;

        // Remove merged from configurable element from internal tracking list
        // Note: we shouldn't need to recompute the sync set in that case, as the merged to element should include the syncers of merged from elements
        doRemoveConfigurableElement(pMergedConfigurableElement, false);
    }
}

void CConfigurableDomain::mergeConfigurations(CConfigurableElement* pToConfigurableElement, CConfigurableElement* pFromConfigurableElement)
{
    // Propagate to domain configurations
    size_t uiNbConfigurations = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(getChild(uiChild));

        // Do the merge.
        pDomainConfiguration->merge(pToConfigurableElement, pFromConfigurableElement);
    }
}

// Configurable elements association
void CConfigurableDomain::doAddConfigurableElement(CConfigurableElement* pConfigurableElement, const CParameterBlackboard *pMainBlackboard)
{
    // Inform configurable element
    pConfigurableElement->addAttachedConfigurableDomain(this);

    // Create associated syncer set
    CSyncerSet* pSyncerSet = new CSyncerSet;

    // Add to sync set the configurable element one
    pConfigurableElement->fillSyncerSet(*pSyncerSet);

    // Store it
    _configurableElementToSyncerSetMap[pConfigurableElement] = pSyncerSet;

    // Add it to global one
    _syncerSet += *pSyncerSet;

    // Inform configurations
    size_t uiNbConfigurations = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(getChild(uiChild));

        pDomainConfiguration->addConfigurableElement(pConfigurableElement, pSyncerSet);
    }

    // Ensure area validity for that configurable element (if main blackboard provided)
    if (pMainBlackboard) {

        // Need to validate against main blackboard
        validateAreas(pConfigurableElement, pMainBlackboard);
    }

    // Already associated descendend configurable elements need a merge of their configuration data
    mergeAlreadyAssociatedDescendantConfigurableElements(pConfigurableElement);

    // Add to list
    _configurableElementList.push_back(pConfigurableElement);
}

void CConfigurableDomain::doRemoveConfigurableElement(CConfigurableElement* pConfigurableElement, bool bRecomputeSyncSet)
{
    // Remove from list
    _configurableElementList.remove(pConfigurableElement);

    // Remove associated syncer set
    CSyncerSet* pSyncerSet = getSyncerSet(pConfigurableElement);

    _configurableElementToSyncerSetMap.erase(pConfigurableElement);

    delete pSyncerSet;

    // Inform configurable element
    pConfigurableElement->removeAttachedConfigurableDomain(this);

    // Inform configurations
    size_t uiNbConfigurations = getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(getChild(uiChild));

        pDomainConfiguration->removeConfigurableElement(pConfigurableElement);
    }
    // Recompute our sync set if needed
    if (bRecomputeSyncSet) {

        computeSyncSet();
    }
}

// Syncer set retrieval from configurable element
CSyncerSet* CConfigurableDomain::getSyncerSet(const CConfigurableElement* pConfigurableElement) const
{
    ConfigurableElementToSyncerSetMapIterator mapIt = _configurableElementToSyncerSetMap.find(pConfigurableElement);

    assert(mapIt != _configurableElementToSyncerSetMap.end());

    return mapIt->second;
}

// Configuration retrieval
CDomainConfiguration* CConfigurableDomain::findConfiguration(const string& strConfiguration, string& strError)
{
    CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(findChild(strConfiguration));

    if (!pDomainConfiguration) {

        strError = "Domain configuration " + strConfiguration + " not found";

        return NULL;
    }
    return pDomainConfiguration;
}

const CDomainConfiguration* CConfigurableDomain::findConfiguration(const string& strConfiguration, string& strError) const
{
    const CDomainConfiguration* pDomainConfiguration = static_cast<const CDomainConfiguration*>(findChild(strConfiguration));

    if (!pDomainConfiguration) {

        strError = "Domain configuration " + strConfiguration + " not found";

        return NULL;
    }
    return pDomainConfiguration;
}
