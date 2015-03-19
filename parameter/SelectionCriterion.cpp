/*
 * Copyright (c) 2011-2015, Intel Corporation
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
#include <log/Logger.h>
#include "Utility.h"

#include <stdexcept>

CSelectionCriterion::CSelectionCriterion(const std::string& name, core::log::Logger& logger) :
    CSelectionCriterion(name, logger, {})
{
}

CSelectionCriterion::CSelectionCriterion(const std::string& name,
                                         core::log::Logger& logger,
                                         const MatchMethods& derivedMatchMethods) :
    mMatchMethods(CUtility::merge(MatchMethods{
                                    {"Is", [&](int state){ return mState == state; }},
                                    {"IsNot", [&](int state){ return mState != state; }}},
                                  derivedMatchMethods)),
    mState(0), _uiNbModifications(0), _logger(logger), mName(name)
{
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
    if (mState != iState) {

        mState = iState;

        _logger.info() << "Selection criterion changed event: "
                       << getFormattedDescription(false, false);

        // Check if the previous criterion value has been taken into account (i.e. at least one Configuration was applied
        // since the last criterion change)
        if (_uiNbModifications != 0) {

            _logger.warning() << "Selection criterion '" << mName
                              << "' has been modified " << _uiNbModifications
                              << " time(s) without any configuration application";
        }

        // Track the number of modifications for this criterion
        _uiNbModifications++;
    }
}

int CSelectionCriterion::getCriterionState() const
{
    return mState;
}

// Name
std::string CSelectionCriterion::getCriterionName() const
{
    return mName;
}

/// User request
std::string CSelectionCriterion::getFormattedDescription(bool bWithTypeInfo, bool bHumanReadable) const
{
    std::string strFormattedDescription;

    if (bHumanReadable) {

        if (bWithTypeInfo) {

            // Display type info
            CUtility::appendTitle(strFormattedDescription, mName + ":");

            // States
            strFormattedDescription += "Possible states ";

            // Type Kind
            strFormattedDescription += "(";
            strFormattedDescription += isInclusive() ? "Inclusive" : "Exclusive";
            strFormattedDescription += "): ";

            // States
            strFormattedDescription += listPossibleValues() + "\n";

            // Current State
            strFormattedDescription += "Current state";
        } else {
            // Name only
            strFormattedDescription = mName;
        }

        // Current State
        strFormattedDescription += " = " + getFormattedState();
    } else {
        // Name
        strFormattedDescription = "Criterion name: " + mName;

        if (bWithTypeInfo) {
            // Type Kind
            strFormattedDescription += ", type kind: ";
            strFormattedDescription +=  isInclusive() ? "inclusive" : "exclusive";
        }

        // Current State
        strFormattedDescription += ", current state: " + getFormattedState();

         if (bWithTypeInfo) {
            // States
            strFormattedDescription += ", states: " + listPossibleValues();
        }
    }
    return strFormattedDescription;
}

// XML export
void CSelectionCriterion::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    (void) serializingContext;
    xmlElement.setAttributeString("Value", getFormattedState());
    xmlElement.setAttributeString("Name", mName);
    xmlElement.setAttributeString("Kind", isInclusive() ? "Inclusive" : "Exclusive");

    for (auto& valuePair: mValuePairs) {
        CXmlElement childValuePairElement;

        xmlElement.createChild(childValuePairElement, "ValuePair");
        childValuePairElement.setAttributeString("Literal", valuePair.first);
        childValuePairElement.setAttributeSignedInteger("Numerical", valuePair.second);
    }
}

bool CSelectionCriterion::isInclusive() const
{
    return false;
}

bool CSelectionCriterion::addValuePair(int numericalValue,
                                       const std::string& literalValue,
                                       std::string& error)
{
    // Check already inserted
    if (mValuePairs.count(literalValue) == 1) {

        std::ostringstream errorBuf;
        errorBuf << "Rejecting value pair association (literal already present): 0x"
                 << std::hex << numericalValue << " - " << literalValue
                 << " for criterion '" << getCriterionName() << "'";
        error = errorBuf.str();

        return false;
    }
    mValuePairs[literalValue] = numericalValue;

    return true;
}

bool CSelectionCriterion::getNumericalValue(const std::string& literalValue,
                                            int& numericalValue) const
{
    try {
        numericalValue = mValuePairs.at(literalValue);
        return true;
    }
    catch (std::out_of_range&) {
        return false;
    }
}

bool CSelectionCriterion::getLiteralValue(int numericalValue, std::string& literalValue) const
{
    for (auto& value: mValuePairs) {
        if (value.second == numericalValue) {
            literalValue = value.first;
            return true;
        }
    }
    return false;
}

std::string CSelectionCriterion::getFormattedState() const
{
    std::string formattedState;
    getLiteralValue(mState, formattedState);

    return CSelectionCriterion::checkFormattedStateEmptyness(formattedState);
}

std::string CSelectionCriterion::listPossibleValues() const
{
    std::string possibleValues = "{";

    // Get comma separated list of values
    bool first = true;
    for (auto& value: mValuePairs) {

        if (first) {
            first = false;
        } else {
            possibleValues += ", ";
        }
        possibleValues += value.first;
    }
    possibleValues += "}";

    return possibleValues;
}

std::string& CSelectionCriterion::checkFormattedStateEmptyness(std::string& formattedState) const
{
    if (formattedState.empty()) {
        formattedState = "<none>";
    }
    return formattedState;
}

bool CSelectionCriterion::match(const std::string& method, int32_t state) const
{
    return mMatchMethods.at(method)(state);
}

bool CSelectionCriterion::isMatchMethodAvailable(const std::string& method) const
{
    return mMatchMethods.count(method) == 1;
}
