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
#include "SelectionCriteria.h"
#include "SelectionCriterionLibrary.h"
#include "SelectionCriteriaDefinition.h"

#define base CElement

CSelectionCriteria::CSelectionCriteria()
{
    addChild(new CSelectionCriterionLibrary);
    addChild(new CSelectionCriteriaDefinition);
}

string CSelectionCriteria::getKind() const
{
    return "SelectionCriteria";
}

// Selection Criteria/Type creation
CSelectionCriterionType* CSelectionCriteria::createSelectionCriterionType(bool bIsInclusive)
{
    return getSelectionCriterionLibrary()->createSelectionCriterionType(bIsInclusive);
}

CSelectionCriterion* CSelectionCriteria::createSelectionCriterion(const string& strName, const CSelectionCriterionType* pSelectionCriterionType)
{
    return getSelectionCriteriaDefinition()->createSelectionCriterion(strName, pSelectionCriterionType);
}

// Selection criterion retrieval
CSelectionCriterion* CSelectionCriteria::getSelectionCriterion(const string& strName)
{
    return getSelectionCriteriaDefinition()->getSelectionCriterion(strName);
}

// List available criteria
void CSelectionCriteria::listSelectionCriteria(list<string>& lstrResult, bool bWithTypeInfo, bool bHumanReadable) const
{
    getSelectionCriteriaDefinition()->listSelectionCriteria(lstrResult, bWithTypeInfo, bHumanReadable);
}

// Reset the modified status of the children
void CSelectionCriteria::resetModifiedStatus()
{
    getSelectionCriteriaDefinition()->resetModifiedStatus();
}

// Children access
CSelectionCriterionLibrary* CSelectionCriteria::getSelectionCriterionLibrary()
{
    return static_cast<CSelectionCriterionLibrary*>(getChild(ESelectionCriterionLibrary));
}

CSelectionCriteriaDefinition* CSelectionCriteria::getSelectionCriteriaDefinition()
{
    return static_cast<CSelectionCriteriaDefinition*>(getChild(ESelectionCriteriaDefinition));
}

const CSelectionCriteriaDefinition* CSelectionCriteria::getSelectionCriteriaDefinition() const
{
    return static_cast<const CSelectionCriteriaDefinition*>(getChild(ESelectionCriteriaDefinition));
}
