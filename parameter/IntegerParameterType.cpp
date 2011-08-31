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
#include "IntegerParameterType.h"
#include <stdlib.h>
#include <sstream>
#include "ParameterAccessContext.h"

#define base CParameterType

CIntegerParameterType::CIntegerParameterType(const string& strName) : base(strName), _uiMin(0), _uiMax(uint32_t(-1))
{
}

string CIntegerParameterType::getKind() const
{
    return "IntegerParameter";
}

bool CIntegerParameterType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Sign
    _bSigned = xmlElement.getAttributeBoolean("Signed");

    // Size
    setSize(xmlElement.getAttributeInteger("Size") / 8);

    // Min / Max
    if (_bSigned) {
        if (xmlElement.hasAttribute("Min")) {

            _uiMin = (uint32_t)xmlElement.getAttributeSignedInteger("Min");
        } else {

            _uiMin = 1UL << 31;
        }
        if (xmlElement.hasAttribute("Max")) {

            _uiMax = (uint32_t)xmlElement.getAttributeSignedInteger("Max");
        } else {

            _uiMax = (1UL << 31) - 1;
        }
    } else {
        if (xmlElement.hasAttribute("Min")) {

            _uiMin = xmlElement.getAttributeInteger("Min");
        } else {

            _uiMin = 0;
        }
        if (xmlElement.hasAttribute("Max")) {

            _uiMax = xmlElement.getAttributeInteger("Max");
        } else {

            _uiMax = (uint32_t)-1;
        }
    }

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

bool CIntegerParameterType::asInteger(const string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    uiValue = strtoul(strValue.c_str(), NULL, 0);

    // Check against Min / Max
    if (_bSigned) {

        if (!checkValueAgainstRange<int32_t>(uiValue, parameterAccessContext)) {

            return false;
        }
    } else {

        if (!checkValueAgainstRange<uint32_t>(uiValue, parameterAccessContext)) {

            return false;
        }
    }

    return true;
}

void CIntegerParameterType::asString(const uint32_t& uiValue, string& strValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)parameterAccessContext;

    // Format
    ostringstream strStream;

    if (_bSigned) {
        // Sign extend
        uint32_t uiShift = (4 - getSize()) << 3;

        int32_t iValue = (int32_t)uiValue;

        if (uiShift) {

            iValue = (iValue << uiShift) >> uiShift;
        }

        strStream << iValue;
    } else {

        strStream << uiValue;
    }

    strValue = strStream.str();
}

// Default value handling (simulation only)
uint32_t CIntegerParameterType::getDefaultValue() const
{
    return _uiMin;
}

// Range checking
template <class type> bool CIntegerParameterType::checkValueAgainstRange(type value, CParameterAccessContext& parameterAccessContext) const
{
    if ((type)value < (type)_uiMin || (type)value > (type)_uiMax) {
        ostringstream strStream;

        strStream << "Value " << value << " standing out of admitted range: [" << (type)_uiMin << ", " <<  (type)_uiMax << "] for " << getKind();

        parameterAccessContext.setError(strStream.str());

        return false;
    }
    return true;
}
