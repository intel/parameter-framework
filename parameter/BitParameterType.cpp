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

// From IXmlSink
bool CBitParameterType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Pos
    _uiBitPos = xmlElement.getAttributeInteger("Pos");

    // Size
    _uiBitSize = xmlElement.getAttributeInteger("Size");

    // TODO: Validate bit pos and size still fit into parent type
    const CBitParameterBlockType* pBitParameterBlockType = static_cast<const CBitParameterBlockType*>(getParent());

    uint32_t uiParentBlockBitSize = pBitParameterBlockType->getSize() << 3;

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
bool CBitParameterType::asInteger(const string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiConvertedValue = strtoul(strValue.c_str(), NULL, 0);

    if (uiConvertedValue > getMaxValue()) {

        // Range exceeded
        ostringstream strStream;

        strStream << strValue << " value out of range [0, " << getMaxValue() << "] for " + getKind();

        parameterAccessContext.setError(strStream.str());

        return false;
    }

    // Do bitwise operation
    uiValue = (uiValue & ~getMask()) | (uiConvertedValue << _uiBitPos);

    return true;
}

void CBitParameterType::asString(const uint32_t& uiValue, string& strValue) const
{
    uint32_t uiConvertedValue = (uiValue & getMask()) >> _uiBitPos;

    // Format
    ostringstream strStream;

    strStream << uiConvertedValue;

    strValue = strStream.str();
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
