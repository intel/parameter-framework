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
#include "ParameterType.h"
#include "Parameter.h"
#include "ArrayParameter.h"
#include "ParameterAccessContext.h"

#include <climits>

#define base CTypeElement

using std::string;

const std::string CParameterType::gUnitPropertyName = "Unit";

CParameterType::CParameterType(const string &strName) : base(strName)
{
}

// Object creation
void CParameterType::populate(CElement * /*elem*/) const
{
    // Prevent further digging for instantiaton since we're leaf on the strcture tree
}

// Size
void CParameterType::setSize(size_t size)
{
    _size = size;
}

size_t CParameterType::getSize() const
{
    return _size;
}

// Unit
string CParameterType::getUnit() const
{
    return _strUnit;
}

void CParameterType::setUnit(const std::string &strUnit)
{
    _strUnit = strUnit;
}

// From IXmlSink
bool CParameterType::fromXml(const CXmlElement &xmlElement,
                             CXmlSerializingContext &serializingContext)
{
    xmlElement.getAttribute(gUnitPropertyName, _strUnit);
    return base::fromXml(xmlElement, serializingContext);
}

// From IXmlSource
void CParameterType::toXml(CXmlElement &xmlElement,
                           CXmlSerializingContext &serializingContext) const
{
    base::toXml(xmlElement, serializingContext);
    setXmlUnitAttribute(xmlElement);
}

void CParameterType::setXmlUnitAttribute(CXmlElement &xmlElement) const
{
    const string &unit = getUnit();
    if (!unit.empty()) {
        xmlElement.setAttribute(gUnitPropertyName, unit);
    }
}

// XML Serialization value space handling
// Value space handling for configuration import/export
void CParameterType::handleValueSpaceAttribute(
    CXmlElement & /*xmlConfigurableElementSettingsElement*/,
    CConfigurationAccessContext & /*ctx*/) const
{
    // Do nothing by default
}

// Element properties
void CParameterType::showProperties(string &strResult) const
{
    base::showProperties(strResult);

    // Add Unit property if found
    if (!getUnit().empty()) {
        strResult += gUnitPropertyName + ": " + getUnit() + "\n";
    }

    // Scalar size
    strResult += "Scalar size: " + std::to_string(getSize()) + " byte(s) \n";
}

// Default value handling (simulation only)
uint32_t CParameterType::getDefaultValue() const
{
    return 0;
}

// Parameter instantiation
CInstanceConfigurableElement *CParameterType::doInstantiate() const
{
    if (isScalar()) {
        // Scalar parameter
        return new CParameter(getName(), this);
    } else {
        // Array Parameter
        return new CArrayParameter(getName(), this);
    }
}

void CParameterType::signExtend(int32_t &iData) const
{
    doSignExtend(iData);
}

void CParameterType::signExtend(int64_t &iData) const
{
    doSignExtend(iData);
}

// Generic sign extension
template <typename type>
void CParameterType::doSignExtend(type &data) const
{
    size_t shift = CHAR_BIT * (sizeof(data) - getSize());
    // FIXME: If `data` has a signed type and nonnegative value,
    //        and `data × 2^shift` is representable in the result type,
    //        then that is the resulting value;
    //        otherwise, **the behavior is undeﬁned**.
    //        ISO C99 (6.5.7/4) & ISO C++11 [expr.shift]
    data = (data << shift) >> shift;
}

// Check data has no bit set outside available range (32 bits)
bool CParameterType::isEncodable(uint32_t uiData, bool bIsSigned) const
{
    return doIsEncodable(uiData, bIsSigned);
}

// Check data has no bit set outside available range (64 bits)
bool CParameterType::isEncodable(uint64_t uiData, bool bIsSigned) const
{
    return doIsEncodable(uiData, bIsSigned);
}

// Generic encodability check
template <typename type>
bool CParameterType::doIsEncodable(type data, bool bIsSigned) const
{
    if (getSize() == sizeof(data)) {
        // Prevent inappropriate shifts
        return true;
    }

    size_t shift = getSize() * 8;

    if (!bIsSigned) {

        // Check high bits are clean
        return !(data >> shift);

    } else {

        // Negative value?
        bool bIsValueExpectedNegative = (data & (type(1) << (shift - 1))) != 0;

        // Check high bits are clean
        return bIsValueExpectedNegative ? !(~data >> shift) : !(data >> shift);
    }
}

// Remove all bits set outside available range
uint32_t CParameterType::makeEncodable(uint32_t uiData) const
{
    size_t sizeInBits = getSize() * 8;

    uint32_t uiMask = (1 << sizeInBits) - 1;

    return uiData & uiMask;
}

// Conversions (dynamic access)
// Value access
// Boolean
bool CParameterType::toBlackboard(bool /*bUserValue*/, uint32_t & /*uiValue*/,
                                  CParameterAccessContext &parameterAccessContext) const
{
    parameterAccessContext.setError("Unsupported conversion");
    return false;
}

bool CParameterType::fromBlackboard(bool & /*bUserValue*/, uint32_t /*uiValue*/,
                                    CParameterAccessContext &parameterAccessContext) const
{
    parameterAccessContext.setError("Unsupported conversion");
    return false;
}

// Integer
bool CParameterType::toBlackboard(uint32_t /*uiUserValue*/, uint32_t & /*uiValue*/,
                                  CParameterAccessContext &parameterAccessContext) const
{
    parameterAccessContext.setError("Unsupported conversion");
    return false;
}

bool CParameterType::fromBlackboard(uint32_t & /*uiUserValue*/, uint32_t /*uiValue*/,
                                    CParameterAccessContext &parameterAccessContext) const
{
    parameterAccessContext.setError("Unsupported conversion");
    return false;
}

// Signed Integer
bool CParameterType::toBlackboard(int32_t /*iUserValue*/, uint32_t & /*uiValue*/,
                                  CParameterAccessContext &parameterAccessContext) const
{
    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

bool CParameterType::fromBlackboard(int32_t & /*iUserValue*/, uint32_t /*uiValue*/,
                                    CParameterAccessContext &parameterAccessContext) const
{
    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

// Double
bool CParameterType::toBlackboard(double /*dUserValue*/, uint32_t & /*uiValue*/,
                                  CParameterAccessContext &parameterAccessContext) const
{
    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

bool CParameterType::fromBlackboard(double & /*dUserValue*/, uint32_t /*uiValue*/,
                                    CParameterAccessContext &parameterAccessContext) const
{
    parameterAccessContext.setError("Unsupported conversion");

    return false;
}
