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
#include "BitParameterType.h"
#include "BitParameter.h"
#include <stdlib.h>
#include <sstream>
#include "ParameterAccessContext.h"
#include "BitParameterBlockType.h"
#include "Utility.h"

#define base CTypeElement

using std::string;

CBitParameterType::CBitParameterType(const string &strName) : base(strName)
{
}

// CElement
string CBitParameterType::getKind() const
{
    return "BitParameter";
}

// Element properties
void CBitParameterType::showProperties(string &strResult) const
{
    base::showProperties(strResult);

    // Bit Pos
    strResult += "Bit pos: ";
    strResult += std::to_string(_bitPos);
    strResult += "\n";

    // Bit size
    strResult += "Bit size: ";
    strResult += std::to_string(_uiBitSize);
    strResult += "\n";

    // Max
    strResult += "Max: ";
    strResult += std::to_string(_uiMax);
    strResult += "\n";
}

// From IXmlSink
bool CBitParameterType::fromXml(const CXmlElement &xmlElement,
                                CXmlSerializingContext &serializingContext)
{
    // Pos
    xmlElement.getAttribute("Pos", _bitPos);

    // Size
    xmlElement.getAttribute("Size", _uiBitSize);

    // Validate bit pos and size still fit into parent type
    const CBitParameterBlockType *pBitParameterBlockType =
        static_cast<const CBitParameterBlockType *>(getParent());

    size_t uiParentBlockBitSize = pBitParameterBlockType->getSize() * 8;

    if (_bitPos + _uiBitSize > uiParentBlockBitSize) {

        // Range exceeded
        std::ostringstream strStream;

        strStream << "Pos and Size attributes inconsistent with maximum container element size ("
                  << uiParentBlockBitSize << " bits) for " + getKind();

        serializingContext.setError(strStream.str());

        return false;
    }

    // Max
    _uiMax = getMaxEncodableValue();
    if (xmlElement.getAttribute("Max", _uiMax) && (_uiMax > getMaxEncodableValue())) {

        // Max value exceeded
        std::ostringstream strStream;

        strStream << "Max attribute inconsistent with maximum encodable size ("
                  << getMaxEncodableValue() << ") for " + getKind();

        serializingContext.setError(strStream.str());

        return false;
    }

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// Conversion
bool CBitParameterType::toBlackboard(const string &strValue, uint64_t &uiValue,
                                     CParameterAccessContext &parameterAccessContext) const
{
    // Get value
    uint64_t uiConvertedValue = strtoull(strValue.c_str(), NULL, 0);

    if (uiConvertedValue > _uiMax) {

        // Range exceeded
        std::ostringstream strStream;

        strStream << "Value " << strValue << " standing out of admitted range [";

        if (utility::isHexadecimal(strValue)) {

            strStream << "0x0, "
                      << "0x" << std::hex << std::uppercase;
        } else {

            strStream << "0, ";
        }
        strStream << _uiMax << "] for " + getKind();

        parameterAccessContext.setError(strStream.str());

        return false;
    }

    // Do bitwise RMW operation
    uiValue = (uiValue & ~getMask()) | (uiConvertedValue << _bitPos);

    return true;
}

void CBitParameterType::fromBlackboard(string &strValue, const uint64_t &uiValue,
                                       CParameterAccessContext &parameterAccessContext) const
{
    uint64_t uiConvertedValue = (uiValue & getMask()) >> _bitPos;

    // Format
    std::ostringstream strStream;

    // Take care of format
    if (parameterAccessContext.valueSpaceIsRaw() && parameterAccessContext.outputRawFormatIsHex()) {

        strStream << "0x" << std::hex << std::uppercase;
    }

    strStream << uiConvertedValue;

    strValue = strStream.str();
}

// Value access
// Integer
bool CBitParameterType::toBlackboard(uint64_t uiUserValue, uint64_t &uiValue,
                                     CParameterAccessContext &parameterAccessContext) const
{
    if (uiUserValue > _uiMax) {

        parameterAccessContext.setError("Value out of range");

        return false;
    }

    // Do bitwise RMW operation
    uiValue = (uiValue & ~getMask()) | (uiUserValue << _bitPos);

    return true;
}

void CBitParameterType::fromBlackboard(uint32_t &userValue, uint64_t value,
                                       CParameterAccessContext & /*ctx*/) const
{
    userValue = static_cast<uint32_t>((value & getMask()) >> _bitPos);
}

// Access from area configuration
uint64_t CBitParameterType::merge(uint64_t uiOriginData, uint64_t uiNewData) const
{
    return (uiOriginData & ~getMask()) | (uiNewData & getMask());
}

// Bit Size
size_t CBitParameterType::getBitSize() const
{
    return _uiBitSize;
}

CInstanceConfigurableElement *CBitParameterType::doInstantiate() const
{
    return new CBitParameter(getName(), this);
}

// Max value
uint64_t CBitParameterType::getMaxEncodableValue() const
{
    return (uint64_t)-1L >> (8 * sizeof(uint64_t) - _uiBitSize);
}

// Biwise mask
uint64_t CBitParameterType::getMask() const
{
    return getMaxEncodableValue() << _bitPos;
}

// Check data has no bit set outside available range
bool CBitParameterType::isEncodable(uint64_t uiData) const
{
    size_t uiShift = 8 * sizeof(uiData) - _uiBitSize;

    if (uiShift) {

        // Check high bits are clean
        return !(uiData >> uiShift);
    }

    return true;
}

// From IXmlSource
void CBitParameterType::toXml(CXmlElement &xmlElement,
                              CXmlSerializingContext &serializingContext) const
{
    // Position
    xmlElement.setAttribute("Pos", _bitPos);

    // Size
    xmlElement.setAttribute("Size", _uiBitSize);

    // Maximum
    xmlElement.setAttribute("Max", _uiMax);

    base::toXml(xmlElement, serializingContext);
}
