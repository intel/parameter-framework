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
#include "InstanceConfigurableElement.h"
#include "Mapper.h"
#include "SyncerSet.h"
#include "Syncer.h"
#include "TypeElement.h"
#include "ParameterAccessContext.h"
#include <assert.h>

#define base CConfigurableElement

CInstanceConfigurableElement::CInstanceConfigurableElement(const string& strName, const CTypeElement* pTypeElement) : base(strName), _pTypeElement(pTypeElement), _pSyncer(NULL)
{
}

string CInstanceConfigurableElement::getKind() const
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
bool CInstanceConfigurableElement::getMappingData(const string& strKey, const string*& pStrValue) const
{
    // Delegate
    return getTypeElement()->getMappingData(strKey, pStrValue);
}

bool CInstanceConfigurableElement::map(IMapper& mapper, string& strError)
{
    bool bHasMappingData = getTypeElement()->hasMappingData();

    // Begin
    if (bHasMappingData) {

        if (!mapper.mapBegin(this, strError)) {

            return false;
        }
    }

    // Map children
    uint32_t uiNbChildren = getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        CInstanceConfigurableElement* pInstanceConfigurableChildElement = static_cast<CInstanceConfigurableElement*>(getChild(uiChild));

        if (!pInstanceConfigurableChildElement->map(mapper, strError)) {

            return false;
        }
    }

    // End
    if (bHasMappingData) {

        mapper.mapEnd();
    }
    return true;
}

// Element properties
void CInstanceConfigurableElement::showProperties(string& strResult) const
{
    base::showProperties(strResult);

    // Delegate to type element
    _pTypeElement->showProperties(strResult);
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

// Sync
bool CInstanceConfigurableElement::sync(CParameterAccessContext& parameterAccessContext) const
{
    ISyncer* pSyncer = getSyncer();

    if (!pSyncer) {

        parameterAccessContext.setError("Unable to synchronize modification. No Syncer object associated to configurable element " + getPath());

        return false;
    }
    string strError;

    if (!pSyncer->sync(*parameterAccessContext.getParameterBlackboard(), false, strError)) {

        parameterAccessContext.setError(strError);

        return false;
    }
    return true;
}

// Check parameter access path well formed for leaf elements
bool CInstanceConfigurableElement::checkPathExhausted(CPathNavigator& pathNavigator, CErrorContext& errorContext)
{
    string* pStrChildName = pathNavigator.next();

    if (pStrChildName) {

        // Should be leaf element
        errorContext.setError("Path not found: " + pathNavigator.getCurrentPath());

        return false;
    }
    return true;
}

