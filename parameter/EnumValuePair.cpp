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
#include "EnumValuePair.h"
#include "Utility.h"

#define base CElement

using std::string;

CEnumValuePair::CEnumValuePair() : _iNumerical(0)
{
}

// CElement
string CEnumValuePair::getKind() const
{
    return "ValuePair";
}

// Numerical
int32_t CEnumValuePair::getNumerical() const
{
    return _iNumerical;
}

string CEnumValuePair::getNumericalAsString() const
{
    return CUtility::toString(_iNumerical);
}

// From IXmlSink
bool CEnumValuePair::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Literal
    setName(xmlElement.getAttributeString("Literal"));

    // Numerical
    _iNumerical = xmlElement.getAttributeSignedInteger("Numerical");

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// Content dumping
void CEnumValuePair::logValue(string& strValue, CErrorContext& errorContext) const
{
    (void)errorContext;
    // Convert value
    strValue = getNumericalAsString();
}

// From IXmlSource
void CEnumValuePair::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Literal
    xmlElement.setAttributeString("Literal", this->getName());

    // Numerical
    xmlElement.setAttributeString("Numerical", getNumericalAsString());

    base::toXml(xmlElement, serializingContext);
}
