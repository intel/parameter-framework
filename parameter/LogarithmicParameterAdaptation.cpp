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

#include "LogarithmicParameterAdaptation.h"
#include <cmath>
#include <limits>
#include <algorithm>

#define base CLinearParameterAdaptation

CLogarithmicParameterAdaptation::CLogarithmicParameterAdaptation() : base("Logarithmic")
{
    static_assert(std::numeric_limits<double>::is_iec559,
                  "Only double-precision floating points that are compliant with"
                  " IEC 559 (aka IEEE 754) are supported");
}

// Element properties
void CLogarithmicParameterAdaptation::showProperties(std::string &strResult) const
{
    base::showProperties(strResult);

    strResult += " - LogarithmBase: ";
    strResult += std::to_string(_dLogarithmBase);
    strResult += "\n";
    strResult += " - FloorValue: ";
    strResult += std::to_string(_dFloorValue);
    strResult += "\n";
}

bool CLogarithmicParameterAdaptation::fromXml(const CXmlElement &xmlElement,
                                              CXmlSerializingContext &serializingContext)
{
    if (xmlElement.getAttribute("LogarithmBase", _dLogarithmBase) &&
        (_dLogarithmBase <= 0 || _dLogarithmBase == 1)) {
        // Avoid negative and 1 values
        serializingContext.setError("LogarithmBase attribute cannot be negative or 1 on element" +
                                    xmlElement.getPath());

        return false;
    }

    xmlElement.getAttribute("FloorValue", _dFloorValue);

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

int64_t CLogarithmicParameterAdaptation::fromUserValue(double value) const
{
    return std::max(base::fromUserValue(log(value) / log(_dLogarithmBase)),
                    static_cast<int64_t>(_dFloorValue));
}

double CLogarithmicParameterAdaptation::toUserValue(int64_t iValue) const
{
    return exp(base::toUserValue(iValue) * log(_dLogarithmBase));
}
