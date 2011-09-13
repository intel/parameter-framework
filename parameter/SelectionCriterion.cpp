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
#include "SelectionCriterion.h"
#include "AutoLog.h"

#define base CElement

CSelectionCriterion::CSelectionCriterion(const string& strName, const CSelectionCriterionType* pType) : base(strName), _iState(0), _pType(pType)
{
}

string CSelectionCriterion::getKind() const
{
    return "SelectionCriterion";
}

/// From ISelectionCriterionInterface
// State
void CSelectionCriterion::setCriterionState(int iState)
{
    // Check for a change
    if (_iState != iState) {

        CAutoLog autoLog(this, "Selection criterion changed event: " + getFormattedDescription(false));

        _iState = iState;
    }
}

int CSelectionCriterion::getCriterionState() const
{
    return _iState;
}

// Name
string CSelectionCriterion::getCriterionName() const
{
    return getName();
}

// Type
const ISelectionCriterionTypeInterface* CSelectionCriterion::getCriterionType() const
{
    return _pType;
}

/// Match methods
bool CSelectionCriterion::is(int iState) const
{
    return _iState == iState;
}

bool CSelectionCriterion::isNot(int iState) const
{
    return _iState != iState;
}

bool CSelectionCriterion::includes(int iState) const
{
    return (_iState & iState) != 0;
}

bool CSelectionCriterion::excludes(int iState) const
{
    return (_iState & iState) == 0;
}

/// User request
string CSelectionCriterion::getFormattedDescription(bool bWithTypeInfo) const
{
    string strFormattedDescription;

    if (bWithTypeInfo) {

        // Display type info
        appendTitle(strFormattedDescription, getName() + ":");

        // States
        strFormattedDescription += "Possible states ";

        // Type Kind
        strFormattedDescription += "(";
        strFormattedDescription += _pType->isTypeInclusive() ? "Inclusive" : "Exclusive";
        strFormattedDescription += "): ";

        // States
        strFormattedDescription += _pType->listPossibleValues() + "\n";

        // Current State
        strFormattedDescription += "Current state";
    } else {
        // Name only
        strFormattedDescription = getName();
    }

    // Current State
    strFormattedDescription += " = " + _pType->getFormattedState(_iState);

    return strFormattedDescription;
}
