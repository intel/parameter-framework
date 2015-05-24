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
#include <cassert>
#include "ConfigurableDomains.h"
#include "ConfigurableDomain.h"
#include "ConfigurableElement.h"
#include "BinaryStream.h"
#include "AutoLog.h"

#define base CBinarySerializableElement

using std::string;

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
    size_t uiChild;
    size_t uiNbConfigurableDomains = getNbChildren();

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
    size_t uiChild;
    size_t uiNbConfigurableDomains = getNbChildren();

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

    base::childrenToXml(xmlElement, serializingContext);
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

bool CConfigurableDomains::addDomain(CConfigurableDomain& domain, bool bOverwrite,
                                     string& strError)
{
    string strErrorDrop;

    string strDomainName(domain.getName());
    CConfigurableDomain* pExistingDomain = findConfigurableDomain(strDomainName, strErrorDrop);

    if (pExistingDomain) {
        if (!bOverwrite) {
            strError = "Can't add domain \"" + strDomainName +
                "\" because it already exists and overwrite was not requested.";
            return false;
        }

        deleteDomain(*pExistingDomain);
    }

    log_info("Adding configurable domain \"%s\"", strDomainName.c_str());

    addChild(&domain);

    return true;
}

void CConfigurableDomains::deleteDomain(CConfigurableDomain& configurableDomain)
{
    log_info("Deleting configurable domain \"%s\"", configurableDomain.getName().c_str() );

    removeChild(&configurableDomain);

    delete &configurableDomain;
}

bool CConfigurableDomains::deleteDomain(const string& strName, string& strError)
{
    CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strName, strError);

    if (pConfigurableDomain) {
        deleteDomain(*pConfigurableDomain);
        return true;
    }

    return false;
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

    std::set<const CConfigurableElement*> configurableElementSet;

    // Get all owned configurable elements
    gatherAllOwnedConfigurableElements(configurableElementSet);

    // Fill result
    std::set<const CConfigurableElement*>::const_iterator it;

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

    std::set<const CConfigurableElement*> configurableElementSet;

    // Get all owned configurable elements
    gatherAllOwnedConfigurableElements(configurableElementSet);

    // Fill result
    std::set<const CConfigurableElement*>::const_iterator it;

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
    size_t uiChild;
    size_t uiNbConfigurableDomains = getNbChildren();

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
void CConfigurableDomains::gatherAllOwnedConfigurableElements(std::set<const CConfigurableElement*>& configurableElementSet) const
{
    // Delegate to domains
    size_t uiChild;
    size_t uiNbConfigurableDomains = getNbChildren();

    for (uiChild = 0; uiChild < uiNbConfigurableDomains; uiChild++) {

        const CConfigurableDomain* pChildConfigurableDomain = static_cast<const CConfigurableDomain*>(getChild(uiChild));

        pChildConfigurableDomain->gatherConfigurableElements(configurableElementSet);
    }
}

// Config restore
bool CConfigurableDomains::restoreConfiguration(const string& strDomain, const string& strConfiguration, CParameterBlackboard* pMainBlackboard, bool bAutoSync, std::list<string>& lstrError) const
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

bool CConfigurableDomains::setElementSequence(const string& strDomain, const string& strConfiguration, const std::vector<string>& astrNewElementSequence, string& strError)
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
    size_t uiChild;
    size_t uiNbConfigurableDomains = getNbChildren();

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

CParameterBlackboard* CConfigurableDomains::findConfigurationBlackboard(const string& strDomain,
                                                       const string& strConfiguration,
                                                       const CConfigurableElement* pConfigurableElement,
                                                       uint32_t& uiBaseOffset,
                                                       bool& bIsLastApplied,
                                                       string& strError) const
{
    log_info("Find configuration blackboard for Domain:%s, Configuration:%s, Element:%s",
             strDomain.c_str(), strConfiguration.c_str(), pConfigurableElement->getPath().c_str());

    // Find domain
    const CConfigurableDomain* pConfigurableDomain = findConfigurableDomain(strDomain, strError);

    if (!pConfigurableDomain) {

        return NULL;
    }

    // Check that element belongs to the domain
    if (!pConfigurableElement->belongsTo(pConfigurableDomain)) {

        strError = "Element \"" + pConfigurableElement->getPath() + "\" does not belong to domain \"" + strDomain + "\"";

        return NULL;
    }

    // Find Configuration Blackboard and Base Offset
    return pConfigurableDomain->findConfigurationBlackboard(strConfiguration, pConfigurableElement, uiBaseOffset, bIsLastApplied, strError);
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
    // Call the const equivalent
    return const_cast<CConfigurableDomain*>(
        static_cast<const CConfigurableDomains*>(this)->findConfigurableDomain(strDomain, strError)
        );
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
