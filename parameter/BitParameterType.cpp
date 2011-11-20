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
#include "BitParameterType.h"
#include "BitParameter.h"
#include <stdlib.h>
#include <sstream>
#include "ParameterAccessContext.h"
#include "BitParameterBlockType.h"

#define base CTypeElement

CBitParameterType::CBitParameterType(const string& strName) : base(strName), _uiBitPos(0), _uiBitSize(0)
{
}

// CElement
string CBitParameterType::getKind() const
{
    return "BitParameter";
}

// Element properties
void CBitParameterType::showProperties(string& strResult) const
{
    base::showProperties(strResult);

    // Bit Pos
    strResult += "Bit pos: ";
    strResult += toString(_uiBitPos);
    strResult += "\n";

    // Bit size
    strResult += "Bit size: ";
    strResult += toString(_uiBitSize);
    strResult += "\n";
}

// From IXmlSink
bool CBitParameterType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Pos
    _uiBitPos = xmlElement.getAttributeInteger("Pos");

    // Size
    _uiBitSize = xmlElement.getAttributeInteger("Size");

    // Validate bit pos and size still fit into parent type
    const CBitParameterBlockType* pBitParameterBlockType = static_cast<const CBitParameterBlockType*>(getParent());

    uint32_t uiParentBlockBitSize = pBitParameterBlockType->getSize() * 8;

    if (_uiBitPos + _uiBitSize > uiParentBlockBitSize) {

        // Range exceeded
        ostringstream strStream;

        strStream << "Pos and Size attributes inconsistent with maximum container element size (" << uiParentBlockBitSize << " bits) for " + getKind();

        serializingContext.setError(strStream.str());

        return false;
    }

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// Conversion
bool CBitParameterType::toBlackboard(const string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    // Hexa
    bool bValueProvidedAsHexa = !strValue.compare(0, 2, "0x");

    // Get value
    uint32_t uiConvertedValue = strtoul(strValue.c_str(), NULL, 0);

    if (uiConvertedValue > getMaxValue()) {

        // Range exceeded
        ostringstream strStream;

        strStream << "Value " << strValue << " standing out of admitted range [";

        if (bValueProvidedAsHexa) {

            strStream << "0x0, " << "0x" << hex << uppercase;
        } else {

            strStream << "0, ";
        }
        strStream << getMaxValue() << "] for " + getKind();

        parameterAccessContext.setError(strStream.str());

        return false;
    }

    // Do bitwise RMW operation
    uiValue = (uiValue & ~getMask()) | (uiConvertedValue << _uiBitPos);

    return true;
}

void CBitParameterType::fromBlackboard(string& strValue, const uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiConvertedValue = (uiValue & getMask()) >> _uiBitPos;

    // Format
    ostringstream strStream;

    // Take care of format
    if (parameterAccessContext.valueSpaceIsRaw() && parameterAccessContext.outputRawFormatIsHex()) {

        strStream << "0x" << hex << uppercase;
    }

    strStream << uiConvertedValue;

    strValue = strStream.str();
}

// Value access
// Integer
bool CBitParameterType::toBlackboard(uint32_t uiUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    if (uiUserValue > getMaxValue()) {

        parameterAccessContext.setError("Value out of range");

        return false;
    }

    // Do bitwise RMW operation
    uiValue = (uiValue & ~getMask()) | (uiUserValue << _uiBitPos);

    return true;
}

void CBitParameterType::fromBlackboard(uint32_t& uiUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)parameterAccessContext;

    uiUserValue = (uiValue & getMask()) >> _uiBitPos;
}

// Bit Size
uint32_t CBitParameterType::getBitSize() const
{
    return _uiBitSize;
}

CInstanceConfigurableElement* CBitParameterType::doInstantiate() const
{
    return new CBitParameter(getName(), this);
}

// Max value
uint32_t CBitParameterType::getMaxValue() const
{
    return (1 << _uiBitSize) - 1;
}

// Biwise mask
uint32_t CBitParameterType::getMask() const
{
    return getMaxValue() << _uiBitPos;
}

// Check data has no bit set outside available range
bool CBitParameterType::isEncodable(uint32_t uiData) const
{
    uint32_t uiShift = 8 * sizeof(uiData) - _uiBitSize;

    if (uiShift) {

        // Check high bits are clean
        return !(uiData >> uiShift);
    }

    return true;
}
