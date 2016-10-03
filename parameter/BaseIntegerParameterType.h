/*
 * Copyright (c) 2016, Intel Corporation
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
#pragma once

#include "ParameterType.h"

#include <string>

class CParameterAdaptation;

/** Base class for CIntegerParameterType
 *
 * CIntegerParameterType is template - this class Contains the parts that do
 * not depend on template arguments in order to make the implementation more
 * consise.
 */
class CBaseIntegerParameterType : public CParameterType
{
public:
    CBaseIntegerParameterType(const std::string &name) : CParameterType(name){};

    // CElement
    std::string getKind() const override;

    bool fromBlackboard(uint32_t &uiUserValue, uint32_t uiValue,
                        CParameterAccessContext &parameterAccessContext) const override;
    bool fromBlackboard(int32_t &iUserValue, uint32_t uiValue,
                        CParameterAccessContext &parameterAccessContext) const override;
    bool fromBlackboard(double &dUserValue, uint32_t uiValue,
                        CParameterAccessContext &parameterAccessContext) const override
    {
        return CParameterType::fromBlackboard(dUserValue, uiValue, parameterAccessContext);
    }
    bool fromBlackboard(std::string &strValue, const uint32_t &value,
                        CParameterAccessContext &parameterAccessContext) const override = 0;

protected:
    // Adaptation element retrieval
    const CParameterAdaptation *getParameterAdaptation() const;

private:
    // Returns true if children dynamic creation is to be dealt with
    bool childrenAreDynamic() const override;
};
