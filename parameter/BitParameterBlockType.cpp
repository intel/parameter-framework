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
#include "BitParameterBlockType.h"
#include "BitParameterBlock.h"
#include "Utility.h"

#define base CTypeElement

using std::string;

CBitParameterBlockType::CBitParameterBlockType(const string& strName) : base(strName), _uiSize(0)
{
}

string CBitParameterBlockType::getKind() const
{
    return "BitParameterBlock";
}

bool CBitParameterBlockType::childrenAreDynamic() const
{
    return true;
}

// Size
uint32_t CBitParameterBlockType::getSize() const
{
    return _uiSize;
}

// From IXmlSink
bool CBitParameterBlockType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Size
    _uiSize = xmlElement.getAttributeInteger("Size") / 8;

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// Instantiation
CInstanceConfigurableElement* CBitParameterBlockType::doInstantiate() const
{
    return new CBitParameterBlock(getName(), this);
}

// From IXmlSource
void CBitParameterBlockType::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Size
    xmlElement.setAttributeString("Size", CUtility::toString(_uiSize * 8));

    base::toXml(xmlElement, serializingContext);
}
