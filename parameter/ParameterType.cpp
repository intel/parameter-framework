/* 
 * INTEL CONFIDENTIAL
 * Copyright © 2011 Intel 
 * Corporation All Rights Reserved.
 * 
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel’s prior express written permission.
 * 
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 * 
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 */
#include "ParameterType.h"
#include "Parameter.h"
#include "ArrayParameter.h"
#include "ParameterAccessContext.h"

#define base CTypeElement

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

// From IXmlSink
bool CParameterType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Unit
    _strUnit = xmlElement.getAttributeString("Unit");

    return base::fromXml(xmlElement, serializingContext);
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

    // Unit
    if (!_strUnit.empty()) {

        strResult += "Unit: " + _strUnit + "\n";
    }

    // Scalar size
    strResult += "Scalar size: " + toString(getSize()) + " byte(s) \n";
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

// Sign extension (32 bits)
void CParameterType::signExtend(int32_t& iData) const
{
    doSignExtend(iData);
}

// Sign extension (64 bits)
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


