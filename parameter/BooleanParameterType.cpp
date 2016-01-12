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
#include "BooleanParameterType.h"
#include "ParameterAccessContext.h"
#include "Utility.h"
#include "convert.hpp"

#define base CParameterType

CBooleanParameterType::CBooleanParameterType(const std::string &strName) : base(strName)
{
    setSize(1);
}

std::string CBooleanParameterType::getKind() const
{
    return "BooleanParameter";
}

// Tuning interface
bool CBooleanParameterType::toBlackboard(const std::string &strValue, uint32_t &uiValue,
                                         CParameterAccessContext &parameterAccessContext) const
{
    bool value;
    if (not convertTo(strValue, value)) {
        parameterAccessContext.setError(strValue + " value is invalid for " + getKind() +
                                        ", valid values are {0, 1, 0x0, 0x1, true, false}");

        return false;
    }
    uiValue = value;
    return true;
}

bool CBooleanParameterType::fromBlackboard(std::string &strValue, const uint32_t &uiValue,
                                           CParameterAccessContext &parameterAccessContext) const
{
    strValue = uiValue ? "1" : "0";

    if (parameterAccessContext.valueSpaceIsRaw() && parameterAccessContext.outputRawFormatIsHex()) {

        strValue = "0x" + strValue;
    }

    return true;
}

// Value access
bool CBooleanParameterType::toBlackboard(bool bUserValue, uint32_t &uiValue,
                                         CParameterAccessContext & /*ctx*/) const
{
    uiValue = bUserValue;

    return true;
}

bool CBooleanParameterType::fromBlackboard(bool &bUserValue, uint32_t uiValue,
                                           CParameterAccessContext & /*ctx*/) const
{
    bUserValue = uiValue != 0;

    return true;
}

// Integer
bool CBooleanParameterType::toBlackboard(uint32_t uiUserValue, uint32_t &uiValue,
                                         CParameterAccessContext &parameterAccessContext) const
{
    if (uiUserValue > 1) {

        parameterAccessContext.setError("Value out of range");
    }

    uiValue = uiUserValue;

    return true;
}

bool CBooleanParameterType::fromBlackboard(uint32_t &uiUserValue, uint32_t uiValue,
                                           CParameterAccessContext & /*ctx*/) const
{
    uiUserValue = uiValue != 0;

    return true;
}
