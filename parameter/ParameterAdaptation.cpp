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
#include "ParameterAdaptation.h"
#include "Utility.h"

#define base CElement

using std::string;

CParameterAdaptation::CParameterAdaptation(const string& strType) : base(strType), _iOffset(0)
{
}
// CElement
string CParameterAdaptation::getKind() const
{
    return "Adaptation";
}

// Attributes
int32_t CParameterAdaptation::getOffset() const
{
    return _iOffset;
}

// Element properties
void CParameterAdaptation::showProperties(string& strResult) const
{
    // Adaptation type
    strResult += " - Type: ";
    strResult += getName();
    strResult += "\n";

    // Offset
    strResult += " - Offset: ";
    strResult += CUtility::toString(_iOffset);
    strResult += "\n";
}

// From IXmlSink
bool CParameterAdaptation::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Get offset
    if (xmlElement.hasAttribute("Offset")) {

        _iOffset = xmlElement.getAttributeSignedInteger("Offset");

    } else {
        // Default
        _iOffset = 0;
    }

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// Conversions
int64_t CParameterAdaptation::fromUserValue(double dValue) const
{
    return (int64_t)dValue + _iOffset;
}

double CParameterAdaptation::toUserValue(int64_t iValue) const
{
    return (double)(iValue - _iOffset);
}
