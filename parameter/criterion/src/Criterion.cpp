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
#include "criterion/Criterion.h"
#include <Utility.h>
#include <log/Logger.h>

#include <stdexcept>
#include <sstream>
#include <cassert>

namespace core
{
namespace criterion
{

Criterion::Criterion(const std::string& name,
                     const criterion::Values& values,
                     core::log::Logger& logger)
    : Criterion(name, logger, values, (values.empty() ? State{""} : State{*values.begin()}), {})
{
    if (mValues.size() < 2) {
        throw InvalidCriterionError("Not enough values were provided for exclusive criterion '" +
                                    mName  + "' which needs at least 2 values");
    }
}

Criterion::Criterion(const std::string& name,
                     core::log::Logger& logger,
                     const criterion::Values& values,
                     const State& defaultState,
                     const MatchMethods& derivedMatchMethods)
    : mValues(values.begin(), values.end()),
      mMatchMethods(CUtility::merge(MatchMethods{
                                      {"Is", [&](const State& state){ return mState == state; }},
                                      {"IsNot", [&](const State& state){ return mState != state; }}},
                                    derivedMatchMethods)),
      mState(defaultState), _uiNbModifications(0), _logger(logger), mName(name),
      mDefaultState(defaultState)
{
}

bool Criterion::hasBeenModified() const
{
    return _uiNbModifications != 0;
}

void Criterion::resetModifiedStatus()
{
    _uiNbModifications = 0;
}

bool Criterion::setState(const State& state, std::string& error)
{
    if (state.size() > 1) {
        error = "Exclusive criterion '" + mName + "' can't be set with more than one value";
        return false;
    }

    State oldState = mState;
    if (state.empty()) {
        mState = mDefaultState;
    } else {
        // Check that the state contains a registered value
        if (mValues.count(*state.begin()) != 1) {
            error = "Exclusive criterion '" + mName + "' can't be set with '" +
                    *state.begin() + "' value which is not registered";
            return false;
        }
        mState = state;
    }

    if (mState != oldState) {
        stateModificationsEvent();
    }

    return true;
}

void Criterion::stateModificationsEvent()
{
    _logger.info() << "Selection criterion changed event: "
                   << getFormattedDescription(false, false);
    // Check if the previous criterion value has been taken into account
    // (i.e. at least one Configuration was applied
    // since the last criterion change)
    if (_uiNbModifications != 0) {
        _logger.warning() << "Selection criterion '" << mName
                          << "' has been modified " << _uiNbModifications
                          << " time(s) without any configuration application";
    }
    // Track the number of modifications for this criterion
    _uiNbModifications++;
}

core::criterion::State Criterion::getState() const
{
    return mState;
}

std::string Criterion::getName() const
{
    return mName;
}

std::string Criterion::getFormattedDescription(bool bWithTypeInfo, bool bHumanReadable) const
{
    std::string strFormattedDescription;
    std::string typeName = isInclusive() ? "Inclusive" : "Exclusive";

    if (bHumanReadable) {

        if (bWithTypeInfo) {

            // Display type info
            CUtility::appendTitle(strFormattedDescription, mName + ":");

            // States
            strFormattedDescription += "Possible states ";

            // Type Kind
            strFormattedDescription += "(";
            strFormattedDescription += typeName;
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
            strFormattedDescription +=  typeName;
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

void Criterion::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    (void)serializingContext;
    xmlElement.setAttributeString("Value", getFormattedState());
    xmlElement.setAttributeString("Name", mName);
    xmlElement.setAttributeString("Kind", isInclusive() ? "Inclusive" : "Exclusive");

    for (auto& value : mValues) {
        CXmlElement childValueElement;

        xmlElement.createChild(childValueElement, "Value");
        childValueElement.setTextContent(value);
    }
}

bool Criterion::isInclusive() const
{
    return false;
}

std::string Criterion::getFormattedState() const
{
    assert(!mState.empty());
    return *mState.begin();
}

std::string Criterion::listPossibleValues() const
{
    std::string possibleValues = "{";

    // Get comma separated list of values
    bool first = true;
    for (auto& value : mValues) {

        if (first) {
            first = false;
        } else {
            possibleValues += ", ";
        }
        possibleValues += value;
    }
    possibleValues += "}";

    return possibleValues;
}

bool Criterion::isValueAvailable(const Value &value) const
{
    return mValues.count(value) == 1;
}

bool Criterion::match(const std::string& method, const State& state) const
{
    return mMatchMethods.at(method)(state);
}

bool Criterion::isMatchMethodAvailable(const std::string& method) const
{
    return mMatchMethods.count(method) == 1;
}

} /** criterion namespace */
} /** core namespace */
