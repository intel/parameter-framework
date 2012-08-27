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
#include "SelectionCriterion.h"
#include "AutoLog.h"

#define base CElement

CSelectionCriterion::CSelectionCriterion(const string& strName, const CSelectionCriterionType* pType) : base(strName), _iState(0), _pType(pType), _uiNbModifications(0)
{
}

string CSelectionCriterion::getKind() const
{
    return "SelectionCriterion";
}

bool CSelectionCriterion::hasBeenModified() const
{
    return _uiNbModifications != 0;
}

void CSelectionCriterion::resetModifiedStatus()
{
    _uiNbModifications = 0;
}

/// From ISelectionCriterionInterface
// State
void CSelectionCriterion::setCriterionState(int iState)
{
    // Check for a change
    if (_iState != iState) {

        _iState = iState;

        log("Selection criterion changed event: %s", getFormattedDescription(false).c_str());

        // Check if the previous criterion value has been taken into account (i.e. at least one Configuration was applied
        // since the last criterion change)
        if (_uiNbModifications > 0) {

            log("Warning: Selection criterion \"%s\" has been modified %d time(s) without any configuration application", getName().c_str(), _uiNbModifications);
        }

        // Track the number of modifications for this criterion
        _uiNbModifications++;
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
