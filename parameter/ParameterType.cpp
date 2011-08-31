/* <auto_header>
 * <FILENAME>
 * 
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
 *  AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 * 
 * 
 * </auto_header>
 */
#include "ParameterType.h"
#include "Parameter.h"
#include "ArrayParameter.h"

#define base CTypeElement

CParameterType::CParameterType(const string& strName) : base(strName), _uiSize(0)
{
}

CParameterType::~CParameterType()
{
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
    strResult += "Scalar size: " + toString(_uiSize) + " byte(s) \n";
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

// Sign extension
void CParameterType::signExtend(int32_t& iData) const
{
    uint32_t uiSizeInBits = _uiSize << 3;
    uint32_t uiShift = 32 - uiSizeInBits;

    if (uiShift) {

        iData = (iData << uiShift) >> uiShift;
    }
}

// Check data has no bit set outside available range
bool CParameterType::isEncodable(uint32_t uiData) const
{
    uint32_t uiSizeInBits = _uiSize << 3;
    uint32_t uiShift = 32 - uiSizeInBits;

    if (uiShift) {

        // Check high bits are clean
        return !(uiData >> uiShift);
    }

    return true;
}

// Remove all bits set outside available range
uint32_t CParameterType::makeEncodable(uint32_t uiData) const
{
    uint32_t uiSizeInBits = _uiSize << 3;

    uint32_t uiMask = (1 << uiSizeInBits) - 1;

    return uiData & uiMask;
}

// Check data is consistent with available range, with respect to its sign
bool CParameterType::isConsistent(uint32_t uiData, bool bSigned) const
{
    uint32_t uiSizeInBits = _uiSize << 3;
    uint32_t uiShift = 32 - uiSizeInBits;

    if (uiShift) {

        // Negative value?
        bool bIsValueExpectedNegative = bSigned && (uiData & (1 << (uiShift - 1))) != 0;

        // Check high bits are clean
        return bIsValueExpectedNegative ? !(~uiData >> uiShift) : !(uiData >> uiShift);
    }

    return true;
}

