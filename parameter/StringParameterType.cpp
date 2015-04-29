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
#include "StringParameterType.h"
#include "StringParameter.h"
#include "Utility.h"

#define base CTypeElement

using std::string;

CStringParameterType::CStringParameterType(const string& strName) : base(strName), _uiMaxLength(0)
{
}

// CElement
string CStringParameterType::getKind() const
{
    return "StringParameter";
}

// Element properties
void CStringParameterType::showProperties(string& strResult) const
{
    base::showProperties(strResult);

    // Max length
    strResult += "Max length: ";
    strResult += CUtility::toString(_uiMaxLength);
    strResult += "\n";
}

// From IXmlSink
bool CStringParameterType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // MaxLength
    _uiMaxLength = xmlElement.getAttributeInteger("MaxLength");

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

CInstanceConfigurableElement* CStringParameterType::doInstantiate() const
{
    return new CStringParameter(getName(), this);
}

// Max length
uint32_t CStringParameterType::getMaxLength() const
{
    return _uiMaxLength;
}

// From IXmlSource
void CStringParameterType::toXml(CXmlElement &xmlElement, CXmlSerializingContext &serializingContext) const
{
    // MaxLength
    xmlElement.setAttributeInteger("MaxLength", _uiMaxLength);

    base::toXml(xmlElement, serializingContext);
}
