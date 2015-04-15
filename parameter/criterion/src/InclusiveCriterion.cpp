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
#include "Tokenizer.h"

#include <sstream>
#include <cassert>

namespace core
{
namespace criterion
{

const std::string InclusiveCriterion::gDelimiter = "|";

InclusiveCriterion::InclusiveCriterion(const std::string& name, core::log::Logger& logger)
    : Criterion(name, logger,
                {{"none", 0}},
                {{"Includes", [&](int state){ return (mState & state) == state; }},
                 {"Excludes", [&](int state){ return (mState & state) == 0; }}})
{
}

bool InclusiveCriterion::isInclusive() const
{
    return true;
}

bool InclusiveCriterion::addValuePair(int numericalValue,
                                      const std::string& literalValue,
                                      std::string& error)
{
    if (numericalValue == 0) {
        error = "Rejecting value pair association: 0x0 - " + literalValue +
                " for criterion '" + getCriterionName() + "'";
        return false;
    }

    return Criterion::addValuePair(numericalValue, literalValue, error);
}

bool InclusiveCriterion::getNumericalValue(const std::string& literalValue,
                                           int& numericalValue) const
{
    Tokenizer tok(literalValue, gDelimiter);
    std::vector<std::string> literalValues = tok.split();
    numericalValue = 0;

    // Looping on each std::string delimited by "|" token and adding the associated value
    for (std::string atomicLiteral : literalValues) {

        int atomicNumerical = 0;
        if (!Criterion::getNumericalValue(atomicLiteral, atomicNumerical)) {
            return false;
        }
        numericalValue |= atomicNumerical;
    }
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
