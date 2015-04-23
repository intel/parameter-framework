/*
 * Copyright (c) 2015, Intel Corporation
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
#include "criterion/InclusiveCriterion.h"

#include <sstream>
#include <cassert>
#include <algorithm>

namespace core
{
namespace criterion
{

const std::string InclusiveCriterion::gDelimiter = "|";

InclusiveCriterion::InclusiveCriterion(const std::string& name,
                                       const Values& values,
                                       core::log::Logger& logger)
    : Criterion(name, logger, values, {},
                {{"Includes", [&](const State& state) {
                    return std::includes(mState.begin(), mState.end(),
                                         state.begin(), state.end()); }},
                 {"Excludes", [&](const State& state) {
                    State inter;
                    std::set_intersection(mState.begin(), mState.end(),
                                          state.begin(), state.end(),
                                          std::inserter(inter, inter.begin()));
                    return inter.empty(); }}})
{
    if (mValues.size() < 1) {
        throw InvalidCriterionError("Not enough values were provided for inclusive criterion '" +
                                    getCriterionName() + "' which needs at least 1 values");
    }
}

bool InclusiveCriterion::isInclusive() const
{
    return true;
}

std::string InclusiveCriterion::getFormattedState() const
{
    if (mState.empty()) {
        return "none";
    }

    std::string formattedState;
    for (auto& numericalValue : mState) {
        std::string literalValue;
        bool succeed = getLiteralValue(numericalValue, literalValue);
        assert(succeed);
        if (*mState.begin() != numericalValue) {
            formattedState += gDelimiter;
        }
        formattedState += literalValue;
    }
    return formattedState;
}

bool InclusiveCriterion::setState(const State& state, std::string& error)
{
    // Check for a change
    if (mState != state) {
        // Check that the state contains only registered values
        if (!std::all_of(state.begin(), state.end(),
                [&](int value) { return getLiteralValue(value, error); })) {
            error = "Inclusive criterion '" + getCriterionName() +
                    "' can't be set because some values are not registered";
            return false;
        }
        mState = state;
        stateModificationsEvent();
    }
    return true;
}

} /** criterion namespace */
} /** core namespace */
