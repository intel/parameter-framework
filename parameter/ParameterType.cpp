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
#include "Utility.h"

#define base CTypeElement

using std::string;

const std::string CParameterType::gUnitPropertyName = "Unit";

CParameterType::CParameterType(const string& strName) : base(strName), _uiSize(0)
{
}

CParameterType::~CParameterType()
{
}

// Object creation
void CParameterType::populate(CElement* pElement) const
{
    (void)pElement;
    // Prevent further digging for instantiaton since we're leaf on the strcture tree
}

// Size
void CParameterType::setSize(uint32_t uiSize)
{
    _uiSize = uiSize;
}

uint32_t CParameterType::getSize() const
{
    return _uiSize;
}

// Unit
string CParameterType::getUnit() const
{
    return _strUnit;
}

void CParameterType::setUnit(const std::string& strUnit)
{
    _strUnit = strUnit;
}

// From IXmlSink
bool CParameterType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    setUnit(xmlElement.getAttributeString(gUnitPropertyName));
    return base::fromXml(xmlElement, serializingContext);
}

// From IXmlSource
void CParameterType::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    base::toXml(xmlElement, serializingContext);
    setXmlUnitAttribute(xmlElement);
}

void CParameterType::setXmlUnitAttribute(CXmlElement& xmlElement) const
{
    const string& unit = getUnit();
    if (!unit.empty()) {
        xmlElement.setAttributeString(gUnitPropertyName, unit);
    }
}

// XML Serialization value space handling
// Value space handling for configuration import/export
void CParameterType::handleValueSpaceAttribute(CXmlElement& xmlConfigurableElementSettingsElement, CConfigurationAccessContext& configurationAccessContext) const
{
    (void)xmlConfigurableElementSettingsElement;
    (void)configurationAccessContext;
    // Do nothing by default
}

// Element properties
void CParameterType::showProperties(string& strResult) const
{
    base::showProperties(strResult);

    // Add Unit property if found
    if (!getUnit().empty()) {
        strResult += gUnitPropertyName + ": " + getUnit() + "\n";
    }

    // Scalar size
    strResult += "Scalar size: " + CUtility::toString(getSize()) + " byte(s) \n";
}

// Default value handling (simulation only)
uint32_t CParameterType::getDefaultValue() const
{
    return 0;
}

// Parameter instantiation
CInstanceConfigurableElement* CParameterType::doInstantiate() const
{
    if (isScalar()) {
        // Scalar parameter
        return new CParameter(getName(), this);
    } else {
        // Array Parameter
        return new CArrayParameter(getName(), this);
    }
}

void CParameterType::signExtend(int32_t& iData) const
{
    doSignExtend(iData);
}

void CParameterType::signExtend(int64_t& iData) const
{
    doSignExtend(iData);
}

// Generic sign extension
template <typename type>
void CParameterType::doSignExtend(type& data) const
{
    uint32_t uiSizeInBits = getSize() * 8;
    uint32_t uiShift = 8 * sizeof(data) - uiSizeInBits;

    if (uiShift) {

        data = (data << uiShift) >> uiShift;
    }
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

    uint32_t uiShift = getSize() * 8;

    if (!bIsSigned) {

        // Check high bits are clean
        return !(data >> uiShift);

    } else {

        // Negative value?
        bool bIsValueExpectedNegative = (data & (1 << (uiShift - 1))) != 0;

        // Check high bits are clean
        return bIsValueExpectedNegative ? !(~data >> uiShift) : !(data >> uiShift);
    }
}

// Remove all bits set outside available range
uint32_t CParameterType::makeEncodable(uint32_t uiData) const
{
    if (getSize() == sizeof(uint32_t)) {

        return uiData;
    }
    uint32_t uiSizeInBits = getSize() * 8;

    uint32_t uiMask = (1 << uiSizeInBits) - 1;

    return uiData & uiMask;
}

// Conversions (dynamic access)
// Value access
// Boolean
bool CParameterType::toBlackboard(bool bUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)bUserValue;
    (void)uiValue;
    (void)parameterAccessContext;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

bool CParameterType::fromBlackboard(bool& bUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)bUserValue;
    (void)uiValue;
    (void)parameterAccessContext;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

// Integer
bool CParameterType::toBlackboard(uint32_t uiUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)uiUserValue;
    (void)uiValue;
    (void)parameterAccessContext;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

bool CParameterType::fromBlackboard(uint32_t& uiUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)uiUserValue;
    (void)uiValue;
    (void)parameterAccessContext;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

// Signed Integer
bool CParameterType::toBlackboard(int32_t iUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)iUserValue;
    (void)uiValue;
    (void)parameterAccessContext;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

bool CParameterType::fromBlackboard(int32_t& iUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)iUserValue;
    (void)uiValue;
    (void)parameterAccessContext;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

// Double
bool CParameterType::toBlackboard(double dUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)dUserValue;
    (void)uiValue;
    (void)parameterAccessContext;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

bool CParameterType::fromBlackboard(double& dUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)dUserValue;
    (void)uiValue;
    (void)parameterAccessContext;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}


