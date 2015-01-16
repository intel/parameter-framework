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

#pragma once

#include "LinearParameterAdaptation.h"

/**
 * This class is used to perform a logarithmic adapation of type:
 * (slopeNumerator / slopeDenominator) * log(parameter) + offset
 * Since log(x) == -INFINITY , we can define FloorValue as a
 *       x -> 0
 * a lower bound limit for the adaptation
 */
class CLogarithmicParameterAdaptation : public CLinearParameterAdaptation
{
public:
    CLogarithmicParameterAdaptation();

    /**
     * Conversions must satisfy the following: f(f'(a)) = a
     * Let f=fromUserValue and f'=toUserValue
     * if y = f(log(x)/log(base)), then
     * f'(y) * log(base) = log (x)
     * exp(f'(y)*log(base)) = x
     */
    virtual int64_t fromUserValue(double dValue) const;
    virtual double toUserValue(int64_t iValue) const;

    virtual void showProperties(std::string& strResult) const;

    virtual bool fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);
private:
    /**
     * _dLogarithmBase characterizes the new logarithm logB(x) with
     * the following property: logB(x) = log(x) / log(_dLogarithmBase).
     * log being the base-e logarithm.
     */
    double _dLogarithmBase;
    /**
     * _dFloorValue reflects the lower bound for volume attenuation
     */
    double _dFloorValue;
};
