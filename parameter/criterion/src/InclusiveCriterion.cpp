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
#include <Utility.h>

#include <sstream>
#include <cassert>

namespace core
{
namespace criterion
{

const std::string InclusiveCriterion::gDelimiter = "|";

InclusiveCriterion::InclusiveCriterion(const std::string& name,
                                       const Values& values,
                                       core::log::Logger& logger)
    : Criterion(name, logger,
                CUtility::merge(Values{{"none", 0}}, values),
                {{"Includes", [&](int state){ return (mState & state) == state; }},
                 {"Excludes", [&](int state){ return (mState & state) == 0; }}})
{
    // Checking that no values match the 0 numerical state as this is not a valid inclusive value
    for (auto& value : values) {
        if (value.second == 0) {
            throw InvalidCriterionError("Invalid numerical value '0' provided for inclusive "
                                        "criterion '" + name + "' in association with '" +
                                        value.first + "' litteral value");
        }
    }
    if (values.size() > 31) {
        // mState is an integer, so it as 31 bits + 1 for the sign which can't be used
        throw InvalidCriterionError("Too much values provided for inclusive criterion '" +
                                    name  + "', limit is 31");
    }

    // Set Inclusive default state
    mState = 0;
}

bool InclusiveCriterion::isInclusive() const
{
    return true;
}

std::string InclusiveCriterion::getFormattedState() const
{
    std::string formattedState;
    if (mState == 0) {
        // Default inclusive criterion state is always present
        getLiteralValue(0, formattedState);
        return formattedState;
    }

    uint32_t bit;
    bool first = true;

    // Need to go through all set bit
    for (bit = 0; bit < sizeof(mState) * 8; bit++) {
        // Check if current bit is set
        if ((mState & (1 << bit)) == 0) {
            continue;
        }
        // Simple translation
        std::string atomicState;
        // Get literal value with an offset of one as numerical values start from one
        if (!getLiteralValue(bit + 1, atomicState)) {
            // Numeric value not part supported values for this criterion type.
            continue;
        }

        if (first) {
            first = false;
        } else {
            formattedState += gDelimiter;
        }

        formattedState += atomicState;
    }
    return formattedState;
}

} /** criterion namespace */
} /** core namespace */
