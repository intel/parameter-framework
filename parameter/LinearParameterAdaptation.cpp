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
#include "LinearParameterAdaptation.h"
#include "Utility.h"

#define base CParameterAdaptation

using std::string;

CLinearParameterAdaptation::CLinearParameterAdaptation() : base("Linear"), _dSlopeNumerator(1), _dSlopeDenominator(1)
{
}

CLinearParameterAdaptation::CLinearParameterAdaptation(const string& strType) :
    base(strType), _dSlopeNumerator(1), _dSlopeDenominator(1)
{
}

// Element properties
void CLinearParameterAdaptation::showProperties(string& strResult) const
{
    base::showProperties(strResult);

    // SlopeNumerator
    strResult += " - SlopeNumerator: ";
    strResult += CUtility::toString(_dSlopeNumerator);
    strResult += "\n";

    // SlopeDenominator
    strResult += " - SlopeDenominator: ";
    strResult += CUtility::toString(_dSlopeDenominator);
    strResult += "\n";
}

// From IXmlSink
bool CLinearParameterAdaptation::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Get SlopeNumerator
    if (xmlElement.hasAttribute("SlopeNumerator")) {

        _dSlopeNumerator = xmlElement.getAttributeDouble("SlopeNumerator");

    } else {
        // Default
        _dSlopeNumerator = 1;
    }
    // Get SlopeDenominator
    if (xmlElement.hasAttribute("SlopeDenominator")) {

        _dSlopeDenominator = xmlElement.getAttributeDouble("SlopeDenominator");

        // Avoid by 0 division errors
        if (_dSlopeDenominator == 0) {

            serializingContext.setError("SlopeDenominator attribute can't be 0 on element" + xmlElement.getPath());

            return false;
        }

    } else {
        // Default
        _dSlopeDenominator = 1;
    }

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// Conversions
int64_t CLinearParameterAdaptation::fromUserValue(double dValue) const
{
    return base::fromUserValue(dValue * _dSlopeNumerator / _dSlopeDenominator);
}

double CLinearParameterAdaptation::toUserValue(int64_t iValue) const
{
    return base::toUserValue(iValue) * _dSlopeDenominator / _dSlopeNumerator;
}
