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

#include "SelectionCriterion.h"
#include "AutoLog.h"
#include "Utility.h"

#define base CElement

CSelectionCriterion::CSelectionCriterion(const std::string& strName, const CSelectionCriterionType* pType) : base(strName), _iState(0), _pType(pType), _uiNbModifications(0)
{
}

std::string CSelectionCriterion::getKind() const
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

        log_info("Selection criterion changed event: %s", getFormattedDescription(false, false).c_str());

        // Check if the previous criterion value has been taken into account (i.e. at least one Configuration was applied
        // since the last criterion change)
        if (_uiNbModifications != 0) {

            log_warning("Selection criterion \"%s\" has been modified %d time(s) without any configuration application", getName().c_str(), _uiNbModifications);
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
std::string CSelectionCriterion::getCriterionName() const
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
    // For inclusive criterion, Includes checks if ALL the bit sets in iState are set in the
    // current _iState.
    return (_iState & iState) == iState;
}

bool CSelectionCriterion::excludes(int iState) const
{
    return (_iState & iState) == 0;
}

/// User request
std::string CSelectionCriterion::getFormattedDescription(bool bWithTypeInfo, bool bHumanReadable) const
{
    std::string strFormattedDescription;

    if (bHumanReadable) {

        if (bWithTypeInfo) {

            // Display type info
            CUtility::appendTitle(strFormattedDescription, getName() + ":");

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
    } else {
        // Name
        strFormattedDescription = "Criterion name: " + getName();

        if (bWithTypeInfo) {
            // Type Kind
            strFormattedDescription += ", type kind: ";
            strFormattedDescription +=  _pType->isTypeInclusive() ? "inclusive" : "exclusive";
        }

        // Current State
        strFormattedDescription += ", current state: " +
                                   _pType->getFormattedState(_iState);

         if (bWithTypeInfo) {
            // States
            strFormattedDescription += ", states: " +
                                       _pType->listPossibleValues();
        }
    }
    return strFormattedDescription;
}

// XML export
void CSelectionCriterion::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Current Value
    xmlElement.setAttributeString("Value", _pType->getFormattedState(_iState));

    // Serialize Type node
    _pType->toXml(xmlElement, serializingContext);

    base::toXml(xmlElement, serializingContext);
}
