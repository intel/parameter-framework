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
#include "IntegerParameterType.h"
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include "ParameterAccessContext.h"
#include <assert.h>
#include "ParameterAdaptation.h"
#include <errno.h>

#define base CParameterType

CIntegerParameterType::CIntegerParameterType(const string& strName) : base(strName), _uiMin(0), _uiMax(uint32_t(-1))
{
}

// Kind
string CIntegerParameterType::getKind() const
{
    return "IntegerParameter";
}

// Deal with adaption node
bool CIntegerParameterType::childrenAreDynamic() const
{
    return true;
}

// Element properties
void CIntegerParameterType::showProperties(string& strResult) const
{
    base::showProperties(strResult);

    // Sign
    strResult += "Signed: ";
    strResult += _bSigned ? "yes" : "no";
    strResult += "\n";

    // Min
    strResult += "Min: ";
    strResult += _bSigned ? toString((int32_t)_uiMin) : toString(_uiMin);
    strResult += "\n";

    // Max
    strResult += "Max: ";
    strResult += _bSigned ? toString((int32_t)_uiMax) : toString(_uiMax);
    strResult += "\n";

    // Check if there's an adaptation object available
    const CParameterAdaptation* pParameterAdaption = getParameterAdaptation();

    if (pParameterAdaption) {

        // Display adaptation properties
        strResult += "Adaptation:\n";

        pParameterAdaption->showProperties(strResult);
    }
}

bool CIntegerParameterType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Sign
    _bSigned = xmlElement.getAttributeBoolean("Signed");

    // Size in bits
    uint32_t uiSizeInBits = xmlElement.getAttributeInteger("Size");

    // Size
    setSize(uiSizeInBits / 8);

    // Min / Max
    if (_bSigned) {

        // Signed means we have one less util bit
        uiSizeInBits--;

        if (xmlElement.hasAttribute("Min")) {

            _uiMin = (uint32_t)xmlElement.getAttributeSignedInteger("Min");
        } else {

            _uiMin = 1UL << uiSizeInBits;

            signExtend((int32_t&)_uiMin);
        }
        if (xmlElement.hasAttribute("Max")) {

            _uiMax = (uint32_t)xmlElement.getAttributeSignedInteger("Max");
        } else {

            _uiMax = (1UL << uiSizeInBits) - 1;
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

            _uiMax = (uint32_t)-1L >> (8 * sizeof(uint32_t) - uiSizeInBits);
        }
    }

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// Conversion (tuning)
bool CIntegerParameterType::toBlackboard(const string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    // Hexa
    bool bValueProvidedAsHexa = !strValue.compare(0, 2, "0x");

    // Get value
    int64_t iData;

    // Reset errno to check if it is updated during the conversion (strtol/strtoul)
    errno = 0;
    char *pcStrEnd;

    // Convert the input string
    if (_bSigned) {

        iData = strtoll(strValue.c_str(), &pcStrEnd, 0);
    } else {

        iData = strtoull(strValue.c_str(), &pcStrEnd, 0);
    }


    // Conversion error when the input string does not contain any digit or the number is out of range (int32_t type)
    bool bConversionSucceeded = !errno && (strValue.c_str() != pcStrEnd);

    // Check against Min / Max
    if (_bSigned) {

        if (bConversionSucceeded && bValueProvidedAsHexa && isEncodable((uint64_t)iData, !bValueProvidedAsHexa)) {

            // Sign extend
            signExtend(iData);
        }

        if (!checkValueAgainstRange<int64_t>(strValue, iData, (int32_t)_uiMin, (int32_t)_uiMax, parameterAccessContext, bValueProvidedAsHexa, bConversionSucceeded)) {

            return false;
        }
    } else {

        if (!checkValueAgainstRange<uint64_t>(strValue, iData, _uiMin, _uiMax, parameterAccessContext, bValueProvidedAsHexa, bConversionSucceeded)) {

            return false;
        }
    }

    uiValue = (uint32_t)iData;

    return true;
}

bool CIntegerParameterType::fromBlackboard(string& strValue, const uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    // Check unsigned value is encodable
    assert(isEncodable(uiValue, false));

    // Format
    ostringstream strStream;

    // Take care of format
    if (parameterAccessContext.valueSpaceIsRaw() && parameterAccessContext.outputRawFormatIsHex()) {

        // Hexa display with unecessary bits cleared out
        strStream << "0x" << hex << uppercase << setw(getSize()*2) << setfill('0') << uiValue;
    } else {

        if (_bSigned) {

            int32_t iValue = uiValue;

            // Sign extend
            signExtend(iValue);

            strStream << iValue;
        } else {

            strStream << uiValue;
        }
    }

    strValue = strStream.str();

    return true;
}

// Value access
// Integer
bool CIntegerParameterType::toBlackboard(uint32_t uiUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    if (_bSigned) {

        parameterAccessContext.setError("Parameter is signed");

        return false;
    }
    if (uiUserValue < _uiMin || uiUserValue > _uiMax) {

        parameterAccessContext.setError("Value out of range");

        return false;
    }
    // Do assign
    uiValue = uiUserValue;

    return true;
}

bool CIntegerParameterType::fromBlackboard(uint32_t& uiUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const
{
    if (_bSigned) {

        parameterAccessContext.setError("Parameter is signed");

        return false;
    }
    // Do assign
    uiUserValue = uiValue;

    return true;
}

// Signed Integer
bool CIntegerParameterType::toBlackboard(int32_t iUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    if (!_bSigned) {

        parameterAccessContext.setError("Parameter is unsigned");

        return false;
    }
    if (iUserValue < (int32_t)_uiMin || iUserValue > (int32_t)_uiMax) {

        parameterAccessContext.setError("Value out of range");

        return false;
    }
    // Do assign
    uiValue = iUserValue;

    return true;
}

bool CIntegerParameterType::fromBlackboard(int32_t& iUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const
{
    if (!_bSigned) {

        parameterAccessContext.setError("Parameter is unsigned");

        return false;
    }
    int32_t iValue = uiValue;

    // Sign extend
    signExtend(iValue);

    // Do assign
    iUserValue = iValue;

    return true;
}

// Double
bool CIntegerParameterType::toBlackboard(double dUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    // Check if there's an adaptation object available
    const CParameterAdaptation* pParameterAdaption = getParameterAdaptation();

    if (!pParameterAdaption) {

        // Reject request and let upper class handle the error
        return base::toBlackboard(dUserValue, uiValue, parameterAccessContext);
    }

    // Do the conversion
    int64_t iConvertedValue = pParameterAdaption->fromUserValue(dUserValue);

    // Check against range
    if (_bSigned) {

        if (iConvertedValue < (int32_t)_uiMin || iConvertedValue > (int32_t)_uiMax) {

            parameterAccessContext.setError("Value out of range");

            return false;
        }
    } else {

        if (iConvertedValue < _uiMin || iConvertedValue > _uiMax) {

            parameterAccessContext.setError("Value out of range");

            return false;
        }
    }

    // Do assign
    uiValue = (uint32_t)iConvertedValue;

    return true;
}

bool CIntegerParameterType::fromBlackboard(double& dUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const
{
    // Check if there's an adaptation object available
    const CParameterAdaptation* pParameterAdaption = getParameterAdaptation();

    if (!pParameterAdaption) {

        // Reject request and let upper class handle the error
        return base::fromBlackboard(dUserValue, uiValue, parameterAccessContext);
    }

    int64_t iValueToConvert;

    // Deal with signed data
    if (_bSigned) {

        int32_t iValue = uiValue;

        signExtend(iValue);

        iValueToConvert = iValue;
    } else {

        iValueToConvert = uiValue;
    }

    // Do the conversion
    dUserValue = pParameterAdaption->toUserValue(iValueToConvert);

    return true;
}

// Default value handling (simulation only)
uint32_t CIntegerParameterType::getDefaultValue() const
{
    return _uiMin;
}

// Range checking
template <typename type> bool CIntegerParameterType::checkValueAgainstRange(const string& strValue, type value, type minValue, type maxValue, CParameterAccessContext& parameterAccessContext, bool bHexaValue, bool bConversionSucceeded) const
{
    if (!bConversionSucceeded || value < minValue || value > maxValue) {

        ostringstream strStream;

        strStream << "Value " << strValue << " standing out of admitted range [";

        if (bHexaValue) {

            // Format Min
            strStream << "0x" << hex << uppercase << setw(getSize()*2) << setfill('0') << makeEncodable(minValue);
            // Format Max
            strStream << ", 0x" << hex << uppercase << setw(getSize()*2) << setfill('0') << makeEncodable(maxValue);

        } else {

            strStream << minValue << ", " <<  maxValue;
        }

        strStream << "] for " << getKind();

        parameterAccessContext.setError(strStream.str());

        return false;
    }
    return true;
}

// Adaptation element retrieval
const CParameterAdaptation* CIntegerParameterType::getParameterAdaptation() const
{
    return static_cast<const CParameterAdaptation*>(findChildOfKind("Adaptation"));
}
