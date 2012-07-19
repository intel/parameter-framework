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
#include "SelectionCriteriaDefinition.h"
#include "SelectionCriterion.h"

CSelectionCriteriaDefinition::CSelectionCriteriaDefinition()
{
}

string CSelectionCriteriaDefinition::getKind() const
{
    return "SelectionCriteriaDefinition";
}

// Selection Criterion creation
CSelectionCriterion* CSelectionCriteriaDefinition::createSelectionCriterion(const string& strName, const CSelectionCriterionType* pSelectionCriterionType)
{
    CSelectionCriterion* pSelectionCriterion = new CSelectionCriterion(strName, pSelectionCriterionType);

    addChild(pSelectionCriterion);

    return pSelectionCriterion;
}

// Selection Criterion access
const CSelectionCriterion* CSelectionCriteriaDefinition::getSelectionCriterion(const string& strName) const
{
    return static_cast<const CSelectionCriterion*>(findChild(strName));
}

CSelectionCriterion* CSelectionCriteriaDefinition::getSelectionCriterion(const string& strName)
{
    return static_cast<CSelectionCriterion*>(findChild(strName));
}

// List available criteria
void CSelectionCriteriaDefinition::listSelectionCriteria(string& strResult, bool bWithTypeInfo) const
{
    // Propagate
    uint32_t uiNbChildren = getNbChildren();
    uint32_t uiChild;

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CSelectionCriterion* pSelectionCriterion = static_cast<const CSelectionCriterion*>(getChild(uiChild));

        strResult += pSelectionCriterion->getFormattedDescription(bWithTypeInfo) + "\n";
    }
}

