/* 
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
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 */
#include <cassert>
#include "ConfigurableDomains.h"
#include "ConfigurableDomain.h"
#include "ConfigurableElement.h"
#include "BinaryStream.h"
#include "AutoLog.h"

#define base CBinarySerializableElement

CConfigurableDomains::CConfigurableDomains()
{
}

string CConfigurableDomains::getKind() const
{
    return "ConfigurableDomains";
}

bool CConfigurableDomains::childrenAreDynamic() const
{
    return true;
}

// Ensure validity on whole domains from main blackboard
void CConfigurableDomains::validate(const CParameterBlackboard* pMainBlackboard)
{
    // Delegate to domains
    uint32_t uiChild;
    uint32_t uiNbConfigurableDomains = getNbChildren();

    for (uiChild = 0; uiChild < uiNbConfigurableDomains; uiChild++) {

        CConfigurableDomain* pChildConfigurableDomain = static_cast<CConfigurableDomain*>(getChild(uiChild));

        pChildConfigurableDomain->validate(pMainBlackboard);
    }
}

// Configuration application if required
void CConfigurableDomains::apply(CParameterBlackboard* pParameterBlackboard, CSyncerSet& syncerSet, bool bForce) const
{
   CAutoLog autoLog(this, "Applying configurations");

    /// Delegate to domains

    // Start with domains that can be synchronized all at once (with passed syncer set)
    uint32_t uiChild;
    uint32_t uiNbConfigurableDomains = getNbChildren();

    for (uiChild = 0; uiChild < uiNbConfigurableDomains; uiChild++) {

        const CConfigurableDomain* pChildConfigurableDomain = static_cast<const CConfigurableDomain*>(getChild(uiChild));

        // Apply and collect syncers when relevant
        pChildConfigurableDomain->apply(pParameterBlackboard, &syncerSet, bForce);
    }
    // Synchronize those collected syncers
    syncerSet.sync(*pParameterBlackboard, false, NULL);

    // Then deal with domains that need to synchronize along apply
    for (uiChild = 0; uiChild < uiNbConfigurableDomains; uiChild++) {

        const CConfigurableDomain* pChildConfigurableDomain = static_cast<const CConfigurableDomain*>(getChild(uiChild));

        // Apply and synchronize when relevant
        pChildConfigurableDomain->apply(pParameterBlackboard, NULL, bForce);
    }
}

// From IXmlSource
void CConfigurableDomains::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Set attribute
    xmlElement.setAttributeString("SystemClassName", getName());

    base::toXml(xmlElement, serializingContext);
}

// Configuration/Domains handling
/// Domains
bool CConfigurableDomains::createDomain(const string& strName, string& strError)
{
    // Already exists?
    if (findChild(strName)) {

        strError = "Already existing configurable domain";

        return false;
    }

    log_info("Creating configurable domain \"%s\"", strName.c_str());

    // Creation/Hierarchy
    addChild(new CConfigurableDomain(strName));

    return true;
}

bool CConfigurableDomains::deleteDomain(const string& strName, string& strError)
{
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strName, strError);

    if (!pConfigurableDomain) {

        return false;
    }

    log_info("Deleting configurable domain \"%s\"", strName.c_str());

    // Hierarchy
    removeChild(pConfigurableDomain);

    // Destroy
    delete pConfigurableDomain;

    return true;
}

void CConfigurableDomains::deleteAllDomains()
{
    log_info("Deleting all configurable domains");

    //remove Children
    clean();
}

bool CConfigurableDomains::renameDomain(const string& strName, const string& strNewName, string& strError)
{
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strName, strError);

    if (!pConfigurableDomain) {

        return false;
    }

    log_info("Renaming configurable domain \"%s\" to \"%s\"", strName.c_str(), strNewName.c_str());

    // Rename
    return pConfigurableDomain->rename(strNewName, strError);
}

bool CConfigurableDomains::setSequenceAwareness(const string& strDomain, bool bSequenceAware, string& strError)
{
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        return false;
    }

    pConfigurableDomain->setSequenceAwareness(bSequenceAware);

    return true;
}

bool CConfigurableDomains::getSequenceAwareness(const string& strDomain, bool& bSequenceAware, string& strError) const
{
    const CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        return false;
    }

    bSequenceAware = pConfigurableDomain->getSequenceAwareness();

    return true;
}

/// Configurations
bool CConfigurableDomains::listConfigurations(const string& strDomain, string& strResult) const
{
    const CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strResult);

    if (!pConfigurableDomain) {

        return false;
    }
    // delegate
    pConfigurableDomain->listChildren(strResult);

    return true;
}

bool CConfigurableDomains::createConfiguration(const string& strDomain, const string& strConfiguration, const CParameterBlackboard* pMainBlackboard, string& strError)
{
    // Find domain
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        return false;
    }
    // Delegate
    return pConfigurableDomain->createConfiguration(strConfiguration, pMainBlackboard, strError);
}

bool CConfigurableDomains::deleteConfiguration(const string& strDomain, const string& strConfiguration, string& strError)
{
    // Find domain
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        return false;
    }
    // Delegate
    return pConfigurableDomain->deleteConfiguration(strConfiguration, strError);
}

bool CConfigurableDomains::renameConfiguration(const string& strDomain, const string& strConfigurationName, const string& strNewConfigurationName, string& strError)
{
    // Find domain
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        return false;
    }
    // Delegate
    return pConfigurableDomain->renameConfiguration(strConfigurationName, strNewConfigurationName, strError);
}

bool CConfigurableDomains::listDomainElements(const string& strDomain, string& strResult) const
{
    // Find domain
    const CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strResult);

    if (!pConfigurableDomain) {

        return false;
    }
    // Delegate
    pConfigurableDomain->listAssociatedToElements(strResult);

    return true;
}

bool CConfigurableDomains::split(const string& strDomain, CConfigurableElement* pConfigurableElement, string& strError)
{
    // Find domain
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        return false;
    }
    // Delegate
    pConfigurableDomain->split(pConfigurableElement, strError);

    return true;
}

void CConfigurableDomains::listAssociatedElements(string& strResult) const
{
    strResult = "\n";

    set<const CConfigurableElement*> configurableElementSet;

    // Get all owned configurable elements
    gatherAllOwnedConfigurableElements(configurableElementSet);

    // Fill result
    set<const CConfigurableElement*>::const_iterator it;

    for (it = configurableElementSet.begin(); it != configurableElementSet.end(); ++it) {

        const CConfigurableElement* pConfigurableElement = *it;

        string strAssociatedDomainList;

        pConfigurableElement->listAssociatedDomains(strAssociatedDomainList, false);

        strResult += pConfigurableElement->getPath() + " [" + strAssociatedDomainList + "]\n";
    }
}

void CConfigurableDomains::listConflictingElements(string& strResult) const
{
    strResult = "\n";

    set<const CConfigurableElement*> configurableElementSet;

    // Get all owned configurable elements
    gatherAllOwnedConfigurableElements(configurableElementSet);

    // Fill result
    set<const CConfigurableElement*>::const_iterator it;

    for (it = configurableElementSet.begin(); it != configurableElementSet.end(); ++it) {

        const CConfigurableElement* pConfigurableElement = *it;

        if (pConfigurableElement->getBelongingDomainCount() > 1) {

            string strBelongingDomainList;

            pConfigurableElement->listBelongingDomains(strBelongingDomainList, false);

            strResult += pConfigurableElement->getPath() + " contained in multiple domains: " + strBelongingDomainList + "\n";
        }
    }
}

void CConfigurableDomains::listDomains(string& strResult) const
{
    strResult = "\n";

    // List domains
    uint32_t uiChild;
    uint32_t uiNbConfigurableDomains = getNbChildren();

    for (uiChild = 0; uiChild < uiNbConfigurableDomains; uiChild++) {

        const CConfigurableDomain* pChildConfigurableDomain = static_cast<const CConfigurableDomain*>(getChild(uiChild));

        // Name
        strResult += pChildConfigurableDomain->getName();

        // Sequence awareness
        if (pChildConfigurableDomain->getSequenceAwareness()) {

            strResult += " [sequence aware]";
        }
        strResult += "\n";
    }
}

// Gather configurable elements owned by any domain
void CConfigurableDomains::gatherAllOwnedConfigurableElements(set<const CConfigurableElement*>& configurableElementSet) const
{
    // Delegate to domains
    uint32_t uiChild;
    uint32_t uiNbConfigurableDomains = getNbChildren();

    for (uiChild = 0; uiChild < uiNbConfigurableDomains; uiChild++) {

        const CConfigurableDomain* pChildConfigurableDomain = static_cast<const CConfigurableDomain*>(getChild(uiChild));

        pChildConfigurableDomain->gatherConfigurableElements(configurableElementSet);
    }
}

// Config restore
bool CConfigurableDomains::restoreConfiguration(const string& strDomain, const string& strConfiguration, CParameterBlackboard* pMainBlackboard, bool bAutoSync, list<string>& lstrError) const
{
    string strError;
    // Find domain
    const CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        lstrError.push_back(strError);
        return false;
    }
    // Delegate
    return pConfigurableDomain->restoreConfiguration(strConfiguration, pMainBlackboard, bAutoSync, lstrError);
}

// Config save
bool CConfigurableDomains::saveConfiguration(const string& strDomain, const string& strConfiguration, const CParameterBlackboard* pMainBlackboard, string& strError)
{
    // Find domain
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        return false;
    }
    // Delegate
    return pConfigurableDomain->saveConfiguration(strConfiguration, pMainBlackboard, strError);
}

bool CConfigurableDomains::setElementSequence(const string& strDomain, const string& strConfiguration, const vector<string>& astrNewElementSequence, string& strError)
{
    // Find domain
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        return false;
    }

    // Delegate to domain
    return pConfigurableDomain->setElementSequence(strConfiguration, astrNewElementSequence, strError);
}

bool CConfigurableDomains::getElementSequence(const string& strDomain, const string& strConfiguration, string& strResult) const
{
    // Find domain
    const CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strResult);

    if (!pConfigurableDomain) {

        return false;
    }
    // Delegate to domain
    return pConfigurableDomain->getElementSequence(strConfiguration, strResult);
}

bool CConfigurableDomains::setApplicationRule(const string& strDomain, const string& strConfiguration, const string& strApplicationRule, const CSelectionCriteriaDefinition* pSelectionCriteriaDefinition, string& strError)
{
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        return false;
    }

    // Delegate to domain
    return pConfigurableDomain->setApplicationRule(strConfiguration, strApplicationRule, pSelectionCriteriaDefinition, strError);
}

bool CConfigurableDomains::clearApplicationRule(const string& strDomain, const string& strConfiguration, string& strError)
{
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        return false;
    }

    // Delegate to domain
    return pConfigurableDomain->clearApplicationRule(strConfiguration, strError);
}

bool CConfigurableDomains::getApplicationRule(const string& strDomain, const string& strConfiguration, string& strResult) const
{
    const CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strResult);

    if (!pConfigurableDomain) {

        return false;
    }

    // Delegate to domain
    return pConfigurableDomain->getApplicationRule(strConfiguration, strResult);
}

// Last applied configurations
void CConfigurableDomains::listLastAppliedConfigurations(string& strResult) const
{
    // Browse domains
    uint32_t uiChild;
    uint32_t uiNbConfigurableDomains = getNbChildren();

    for (uiChild = 0; uiChild < uiNbConfigurableDomains; uiChild++) {

        const CConfigurableDomain* pChildConfigurableDomain = static_cast<const CConfigurableDomain*>(getChild(uiChild));

        strResult += pChildConfigurableDomain->getName() + ": " + pChildConfigurableDomain->getLastAppliedConfigurationName() + " [" + pChildConfigurableDomain->getPendingConfigurationName() + "]\n";
    }
}

// Configurable element - domain association
bool CConfigurableDomains::addConfigurableElementToDomain(const string& strDomain, CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard, string& strError)
{
    // Find domain
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        return false;
    }
    // Delegate
    return pConfigurableDomain->addConfigurableElement(pConfigurableElement, pMainBlackboard, strError);
}

bool CConfigurableDomains::removeConfigurableElementFromDomain(const string& strDomain, CConfigurableElement* pConfigurableElement, string& strError)
{
    // Find domain
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        return false;
    }
    // Delegate
    return pConfigurableDomain->removeConfigurableElement(pConfigurableElement, strError);
}

// Binary settings load/store
bool CConfigurableDomains::serializeSettings(const string& strBinarySettingsFilePath, bool bOut, uint8_t uiStructureChecksum, string& strError)
{
    // Instantiate byte stream
    CBinaryStream binarySettingsStream(strBinarySettingsFilePath, bOut, getDataSize(), uiStructureChecksum);

    // Open file
    if (!binarySettingsStream.open(strError)) {

        strError = "Unable to open binary settings file " + strBinarySettingsFilePath + ": " + strError;

        return false;
    }

    // Serialize
    binarySerialize(binarySettingsStream);

    // Close stream
    binarySettingsStream.close();

    return true;
}

// Domain retrieval
CConfigurableDomain* CConfigurableDomains::findConfigurableDomain(const string& strDomain, string& strError)
{
    // Find domain
    CConfigurableDomain* pConfigurableDomain = static_cast<CConfigurableDomain*>(findChild(strDomain));

    if (!pConfigurableDomain) {

        strError = "Configurable domain " + strDomain + " not found";

        return NULL;
    }

    return pConfigurableDomain;
}

const CConfigurableDomain* CConfigurableDomains::findConfigurableDomain(const string& strDomain, string& strError) const
{
    // Find domain
    const CConfigurableDomain* pConfigurableDomain = static_cast<const CConfigurableDomain*>(findChild(strDomain));

    if (!pConfigurableDomain) {

        strError = "Configurable domain " + strDomain + " not found";

        return NULL;
    }

    return pConfigurableDomain;
}
