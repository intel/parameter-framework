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
#include "InstanceConfigurableElement.h"
#include "Mapper.h"
#include "SyncerSet.h"
#include "Syncer.h"
#include "TypeElement.h"
#include "ParameterAccessContext.h"
#include <assert.h>

#define base CConfigurableElementWithMapping

CInstanceConfigurableElement::CInstanceConfigurableElement(const std::string& strName, const CTypeElement* pTypeElement) : base(strName), _pTypeElement(pTypeElement), _pSyncer(NULL)
{
}

std::string CInstanceConfigurableElement::getKind() const
{
    // Delegate
    return _pTypeElement->getKind();
}

// Type element
const CTypeElement* CInstanceConfigurableElement::getTypeElement() const
{
    return _pTypeElement;
}

// Mapping
bool CInstanceConfigurableElement::getMappingData(const std::string& strKey, const std::string*& pStrValue) const
{
    // Delegate
    return getTypeElement()->getMappingData(strKey, pStrValue);
}

// Returns the formatted mapping
std::string CInstanceConfigurableElement::getFormattedMapping() const
{
    // Delegate
    return getTypeElement()->getFormattedMapping();
}

bool CInstanceConfigurableElement::map(IMapper& mapper, std::string& strError)
{
    bool bHasMappingData = getTypeElement()->hasMappingData();
    bool bKeepDiving = true;

    // Begin
    if (bHasMappingData && !mapper.mapBegin(this, bKeepDiving, strError)) {

        return false;
    }

    // Go on through children?
    if (bKeepDiving) {

        // Map children
        size_t uiNbChildren = getNbChildren();
        size_t uiChild;

        for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

            CInstanceConfigurableElement* pInstanceConfigurableChildElement =
                    static_cast<CInstanceConfigurableElement*>(getChild(uiChild));

            if (!pInstanceConfigurableChildElement->map(mapper, strError)) {

                return false;
            }
        }
    }

    // End
    if (bHasMappingData) {

        mapper.mapEnd();
    }
    return true;
}

void CInstanceConfigurableElement::getListOfElementsWithMapping(
        std::list<const CConfigurableElement*>& configurableElementPath) const
{
    const CTypeElement* pTypeElement = getTypeElement();

    if (pTypeElement && pTypeElement->hasMappingData()) {

        configurableElementPath.push_back(this);
    }

    base::getListOfElementsWithMapping(configurableElementPath);
}

// Element properties
void CInstanceConfigurableElement::showProperties(std::string& strResult) const
{
    base::showProperties(strResult);

    // Delegate to type element
    _pTypeElement->showProperties(strResult);
}

// Scalar or Array?
bool CInstanceConfigurableElement::isScalar() const
{
    return _pTypeElement->isScalar();
}

// Array Length
uint32_t CInstanceConfigurableElement::getArrayLength() const
{
    return _pTypeElement->getArrayLength();
}

// Sync to HW
void CInstanceConfigurableElement::setSyncer(ISyncer* pSyncer)
{
    assert(!_pSyncer);

    _pSyncer = pSyncer;
}

void CInstanceConfigurableElement::unsetSyncer()
{
    _pSyncer = NULL;
}

// Syncer
ISyncer* CInstanceConfigurableElement::getSyncer() const
{
    if (_pSyncer) {

        return _pSyncer;
    }
    // Check parent
    return base::getSyncer();
}

// Syncer set (descendant)
void CInstanceConfigurableElement::fillSyncerSetFromDescendant(CSyncerSet& syncerSet) const
{
    if (_pSyncer) {

        syncerSet += _pSyncer;
    } else {
        // Continue digging
        base::fillSyncerSetFromDescendant(syncerSet);
    }
}

bool CInstanceConfigurableElement::sync(CParameterAccessContext& parameterAccessContext) const
{
    if (!parameterAccessContext.getAutoSync()) {

        // AutoSync is disabled, do not perform the sync.
        // This is not an error, but the expected behavior so return true anyway.
        return true;
    }
    ISyncer* pSyncer = getSyncer();

    if (!pSyncer) {

        parameterAccessContext.setError("Unable to synchronize modification. No Syncer object associated to configurable element:");

        return false;
    }
    std::string strError;

    if (!pSyncer->sync(*parameterAccessContext.getParameterBlackboard(), false, strError)) {

        parameterAccessContext.setError(strError);

        return false;
    }
    return true;
}

// Check parameter access path well formed for leaf elements
bool CInstanceConfigurableElement::checkPathExhausted(CPathNavigator& pathNavigator, CErrorContext& errorContext)
{
    std::string* pStrChildName = pathNavigator.next();

    if (pStrChildName) {

        // Should be leaf element
        errorContext.setError("Path not found: " + pathNavigator.getCurrentPath());

        return false;
    }
    return true;
}

void CInstanceConfigurableElement::toXml(CXmlElement &xmlElement, CXmlSerializingContext &serializingContext) const
{
    base::toXml(xmlElement, serializingContext);
    // Since Description belongs to the Type of Element, delegate it to the type element.
    getTypeElement()->setXmlDescriptionAttribute(xmlElement);
}
