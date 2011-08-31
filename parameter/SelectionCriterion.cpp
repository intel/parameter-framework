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

#define base CElement

CSelectionCriterion::CSelectionCriterion(const string& strName, const CSelectionCriterionType* pType) : base(strName), _iState(0), _pType(pType), _pObserver(NULL)
{
}

string CSelectionCriterion::getKind() const
{
    return "SelectionCriterion";
}

/// From ISelectionCriterionInterface
// State
void CSelectionCriterion::setCriterionState(int iState, bool bUpdate)
{
    // Check for a change
    if (_iState != iState) {

        _iState = iState;

        // Update if required
        if (bUpdate && _pObserver) {

            _pObserver->selectionCriterionChanged(this);
        }
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

/// Observer
void CSelectionCriterion::setObserver(ISelectionCriterionObserver* pSelectionCriterionObserver)
{
    _pObserver = pSelectionCriterionObserver;
}

/// Match methods
bool CSelectionCriterion::equals(int iState) const
{
    return _iState == iState;
}

bool CSelectionCriterion::contains(int iState) const
{
    return (_iState & iState) != 0;
}
