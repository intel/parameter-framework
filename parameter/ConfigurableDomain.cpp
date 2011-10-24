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
#include "ConfigurableDomain.h"
#include "DomainConfiguration.h"
#include "ConfigurableElement.h"
#include "ConfigurationAccessContext.h"
#include "Subsystem.h"
#include "XmlDomainSerializingContext.h"
#include <assert.h>

#define base CBinarySerializableElement

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

// Sequence awareness
void CConfigurableDomain::setSequenceAwareness(bool bSequenceAware)
{
    if (_bSequenceAware != bSequenceAware) {

        log("Making domain \"%s\" sequence %s", getName().c_str(), bSequenceAware ? "aware" : "unaware");

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
    // Sequence awareness
    xmlElement.setAttributeBoolean("SequenceAware", _bSequenceAware);

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
    base::toXml(xmlConfigurationsElement, serializingContext);
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
    const CXmlDomainSerializingContext& xmlDomainSerializingContext = static_cast<const CXmlDomainSerializingContext&>(serializingContext);

    if (!xmlDomainSerializingContext.withSettings()) {

        return;
    }

    // Create Settings element
    CXmlElement xmlSettingsElement;

    xmlElement.createChild(xmlSettingsElement, "Settings");

    // Serialize out all configurations settings
    uint32_t uiNbConfigurations = getNbChildren();
    uint32_t uiChildConfiguration;

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
    CXmlDomainSerializingContext& xmlDomainSerializingContext = static_cast<CXmlDomainSerializingContext&>(serializingContext);

    // Sequence awareness (optional)
    _bSequenceAware = xmlElement.hasAttribute("SequenceAware") && xmlElement.getAttributeBoolean("SequenceAware");

    // Local parsing. Do not dig
    if (!parseDomainConfigurations(xmlElement, serializingContext) || !parseConfigurableElements(xmlElement, serializingContext) || !parseSettings(xmlElement, serializingContext)) {

        return false;
    }

    // All provided configurations are parsed
    // Attempt validation on areas of non provided configurations for all configurable elements if required
    if (xmlDomainSerializingContext.autoValidationRequired()) {

        autoValidateAll();
    }

    return true;
}

// XML parsing
bool CConfigurableDomain::parseDomainConfigurations(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
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
bool CConfigurableDomain::parseConfigurableElements(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Get System Class Element
    CElement* pRootElement = getRoot();

    CElement* pSystemClassElement = pRootElement->findChildOfKind("SystemClass");

    assert(pSystemClassElement);

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

        string* pStrChildName = pathNavigator.next();

        // Is there an element and does it match system class name?
        if (!pStrChildName || *pStrChildName != pSystemClassElement->getName()) {

            serializingContext.setError("Could not find configurable element of path " + strConfigurableElementPath + " from ConfigurableDomain description " + getName());

            return false;
        }

        // Browse system class for configurable element
        CConfigurableElement* pConfigurableElement = static_cast<CConfigurableElement*>(pSystemClassElement->findDescendant(pathNavigator));

        if (!pConfigurableElement) {

            serializingContext.setError("Could not find configurable element of path " + strConfigurableElementPath + " from ConfigurableDomain description " + getName());

            return false;
        }
        // Add found element to domain
        string strError;
        if (!addConfigurableElement(pConfigurableElement, NULL, strError)) {

            serializingContext.setError(strError);

            return false;
        }
    }

    return true;
}

// Parse settings
bool CConfigurableDomain::parseSettings(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Context
    CXmlDomainSerializingContext& xmlDomainSerializingContext = static_cast<CXmlDomainSerializingContext&>(serializingContext);

    // Check we actually need to parse configuration settings
    if (!xmlDomainSerializingContext.withSettings()) {

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

            xmlDomainSerializingContext.setError("Could not find domain configuration referred to by configurable domain " + getName());

            return false;
        }
        // Have domain configuration parse settings for all configurable elements
        if (!pDomainConfiguration->parseSettings(xmlConfigurationSettingsElement, xmlDomainSerializingContext)) {

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
    log("Adding configurable element \"%s\" into domain \"%s\"", pConfigurableElement->getPath().c_str(), getName().c_str());

    // Do add
    doAddConfigurableElement(pConfigurableElement);

    // Ensure area validity for that configurable element (if main blackboard provided)
    if (pMainBlackboard) {

        // Need to validate against main blackboard
        validateAreas(pConfigurableElement, pMainBlackboard);
    }

    return true;
}

bool CConfigurableDomain::removeConfigurableElement(CConfigurableElement* pConfigurableElement, string& strError)
{
    // Not associated?
    if (!containsConfigurableElement(pConfigurableElement)) {

        strError = "Configurable element " + pConfigurableElement->getPath() + " not associated to configuration domain " + getName();

        return false;
    }
    log("Removing configurable element \"%s\" from domain \"%s\"", pConfigurableElement->getPath().c_str(), getName().c_str());

    // Do remove
    doRemoveConfigurableElement(pConfigurableElement, true);

    return true;
}

// Domain splitting
bool CConfigurableDomain::split(CConfigurableElement* pConfigurableElement, string& strError)
{
    // Not associated?
    if (!containsConfigurableElement(pConfigurableElement)) {

        strError = "Configurable element " + pConfigurableElement->getPath() + " not associated to configuration domain " + getName();

        return false;
    }
    log("Splitting configurable element \"%s\" domain \"%s\"", pConfigurableElement->getPath().c_str(), getName().c_str());

    // Create sub domain areas for all configurable element's children
    uint32_t uiNbConfigurableElementChildren = pConfigurableElement->getNbChildren();

    if (!uiNbConfigurableElementChildren) {

        strError = "Configurable element " + pConfigurableElement->getPath() + " has no children to split configurable domain to";

        return false;
    }

    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurableElementChildren; uiChild++) {

        CConfigurableElement* pChildConfigurableElement = static_cast<CConfigurableElement*>(pConfigurableElement->getChild(uiChild));

        doAddConfigurableElement(pChildConfigurableElement);
    }

    // Delegate to configurations
    uint32_t uiNbConfigurations = getNbChildren();

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(getChild(uiChild));

        pDomainConfiguration->split(pConfigurableElement);
    }

    // Remove given configurable element from this domain
    // Note: we shouldn't need to recompute the sync set in that case, as the splitted element should include the syncers of its children elements
    doRemoveConfigurableElement(pConfigurableElement, false);

    return true;
}

// Configuration application if required
bool CConfigurableDomain::apply(CParameterBlackboard* pParameterBlackboard, CSyncerSet& syncerSet, bool bForce, string& strError) const
{
    if (bForce) {
        // Force a configuration restore by forgetting about last applied configuration
        _pLastAppliedConfiguration = NULL;
    }
    const CDomainConfiguration* pApplicableDomainConfiguration = findApplicableDomainConfiguration();

    if (pApplicableDomainConfiguration) {

        // Check not the last one before applying
        if (!_pLastAppliedConfiguration || _pLastAppliedConfiguration != pApplicableDomainConfiguration) {

            log("Applying configuration \"%s\" from domain \"%s\"", pApplicableDomainConfiguration->getName().c_str(), getName().c_str());

            // Do the restore
            if (!pApplicableDomainConfiguration->restore(pParameterBlackboard, _bSequenceAware, strError)) {

                return false;
            }

            // Record last applied configuration
            _pLastAppliedConfiguration = pApplicableDomainConfiguration;

            // Check we did not already sync the changes
            if (!_bSequenceAware) {

                // Since we applied changes, add our own sync set to the given one
                syncerSet += _syncerSet;
            }
        }
    }

    return true;
}

// Return applicable configuration validity for given configurable element
bool CConfigurableDomain::isApplicableConfigurationValid(const CConfigurableElement* pConfigurableElement) const
{
    const CDomainConfiguration* pApplicableDomainConfiguration = findApplicableDomainConfiguration();

    return pApplicableDomainConfiguration && pApplicableDomainConfiguration->isValid(pConfigurableElement);
}

// Presence of application condition on any configuration
bool CConfigurableDomain::hasRules() const
{
    // Delegate to configurations
    uint32_t uiNbConfigurations = getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        const CDomainConfiguration* pDomainConfiguration = static_cast<const CDomainConfiguration*>(getChild(uiChild));

        if (pDomainConfiguration->hasRule()) {

            return true;
        }
    }
    return false;
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
    log("Creating domain configuration \"%s\" into domain \"%s\"", strName.c_str(), getName().c_str());

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
    CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(findChild(strName));

    if (!pDomainConfiguration) {

        strError = "Configuration not found";

        return false;
    }

    // Check configuration has no rule (prevent accidental loss of data)
    if (pDomainConfiguration->hasRule()) {

        strError = "Deletion of configuration containing application rules is not supported to prevent any accitental loss of data.\nPlease consider a direct modification of the XML file.";

        return false;
    }

    log("Deleting configuration \"%s\" from domain \"%s\"", strName.c_str(), getName().c_str());

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
    CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(findChild(strName));

    if (!pDomainConfiguration) {

        strError = "Configuration not found";

        return false;
    }
    log("Renaming domain \"%s\"'s configuration \"%s\" to \"%s\"", getName().c_str(), strName.c_str(), strNewName.c_str());

    // Rename
    return pDomainConfiguration->rename(strNewName, strError);
}

bool CConfigurableDomain::restoreConfiguration(const string& strName, CParameterBlackboard* pMainBlackboard, bool bAutoSync, string& strError)
{
    // Find Domain configuration
    const CDomainConfiguration* pDomainConfiguration = static_cast<const CDomainConfiguration*>(findChild(strName));

    if (!pDomainConfiguration) {

        strError = "Domain configuration " + strName + " not found";

        return false;
    }
    log("Restoring domain \"%s\"'s configuration \"%s\" to parameter blackboard", getName().c_str(), pDomainConfiguration->getName().c_str());

    // Delegate
    if (!pDomainConfiguration->restore(pMainBlackboard, _bSequenceAware && bAutoSync, strError)) {

        return false;
    }

    // Record last applied configuration
    _pLastAppliedConfiguration = pDomainConfiguration;

    // Synchronize
    return !bAutoSync || _syncerSet.sync(*pMainBlackboard, false, strError);
}

bool CConfigurableDomain::saveConfiguration(const string& strName, const CParameterBlackboard* pMainBlackboard, string& strError)
{
    // Find Domain configuration
    CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(findChild(strName));

    if (!pDomainConfiguration) {

        strError = "Domain configuration " + strName + " not found";

        return false;
    }
    log("Saving domain \"%s\"'s configuration \"%s\" from parameter blackboard", getName().c_str(), pDomainConfiguration->getName().c_str());

    // Delegate
    pDomainConfiguration->save(pMainBlackboard);

    return true;
}

bool CConfigurableDomain::setElementSequence(const string& strName, const vector<string>& astrNewElementSequence, string& strError)
{
    // Find Domain configuration
    CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(findChild(strName));

    if (!pDomainConfiguration) {

        strError = "Domain configuration " + strName + " not found";

        return false;
    }

    // Delegate to configuration
    return pDomainConfiguration->setElementSequence(astrNewElementSequence, strError);
}

bool CConfigurableDomain::getElementSequence(const string& strName, string& strResult) const
{
    // Find Domain configuration
    const CDomainConfiguration* pDomainConfiguration = static_cast<const CDomainConfiguration*>(findChild(strName));

    if (!pDomainConfiguration) {

        strResult = "Domain configuration " + strName + " not found";

        return false;
    }

    // Delegate to configuration
    pDomainConfiguration->getElementSequence(strResult);

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

// Ensure validity on whole domain from main blackboard
void CConfigurableDomain::validate(const CParameterBlackboard* pMainBlackboard)
{
    log("Validating whole domain \"" + getName() + "\" against main blackboard");

    // Propagate
    uint32_t uiNbConfigurations = getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(getChild(uiChild));

        pDomainConfiguration->validate(pMainBlackboard);
    }
}

// Ensure validity on areas related to configurable element
void CConfigurableDomain::validateAreas(const CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard)
{
    log("Validating domain \"" + getName() + "\" against main blackboard for configurable element \"" + pConfigurableElement->getPath() + "\"");

    // Propagate
    uint32_t uiNbConfigurations = getNbChildren();
    uint32_t uiChild;

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

    log("Auto validating domain \"" + getName() + "\" against configuration \"" + pValidDomainConfiguration->getName() + "\" for configurable element " + pConfigurableElement->getPath());

    // Validate all other configurations against found one, if any
    uint32_t uiNbConfigurations = getNbChildren();
    uint32_t uiChild;

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
    uint32_t uiNbConfigurations = getNbChildren();
    uint32_t uiChild;

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
    uint32_t uiNbConfigurations = getNbChildren();
    uint32_t uiChild;

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
    uint32_t uiNbConfigurations = getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        const CDomainConfiguration* pDomainConfiguration = static_cast<const CDomainConfiguration*>(getChild(uiChild));

        if (pDomainConfiguration->isApplicable()) {

            return pDomainConfiguration;
        }
    }
    return NULL;
}

// Gather set of configurable elements
void CConfigurableDomain::gatherConfigurableElements(set<const CConfigurableElement*>& configurableElementSet) const
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
    list<CConfigurableElement*> mergedConfigurableElementList;

    ConfigurableElementListIterator it;

    // Browse all configurable elements (new one not yet in the list!)
    for (it = _configurableElementList.begin(); it != _configurableElementList.end(); ++it) {

        CConfigurableElement* pConfigurablePotentialDescendantElement = *it;

        if (pConfigurablePotentialDescendantElement->isDescendantOf(pNewConfigurableElement)) {

            log("In domain \"%s\", merging descendant configurable element's configurations \"%s\" into its ascendant \"%s\" ones", getName().c_str(), pConfigurablePotentialDescendantElement->getName().c_str(), pNewConfigurableElement->getName().c_str());

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
    uint32_t uiNbConfigurations = getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(getChild(uiChild));

        // Do the merge.
        pDomainConfiguration->merge(pToConfigurableElement, pFromConfigurableElement);
    }
}

// Configurable elements association
void CConfigurableDomain::doAddConfigurableElement(CConfigurableElement* pConfigurableElement)
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
    uint32_t uiNbConfigurations = getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurations; uiChild++) {

        CDomainConfiguration* pDomainConfiguration = static_cast<CDomainConfiguration*>(getChild(uiChild));

        pDomainConfiguration->addConfigurableElement(pConfigurableElement, pSyncerSet);
    }

    // Already associated descended configurable elements need a merge of their configuration data
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
    uint32_t uiNbConfigurations = getNbChildren();
    uint32_t uiChild;

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
