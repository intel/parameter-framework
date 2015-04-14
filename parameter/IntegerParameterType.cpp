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
#include "IntegerParameterType.h"
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include "ParameterAccessContext.h"
#include <assert.h>
#include "ParameterAdaptation.h"
#include "Utility.h"
#include <errno.h>

#define base CParameterType

using std::string;
using std::ostringstream;

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
    strResult += _bSigned ? CUtility::toString((int32_t)_uiMin) : CUtility::toString(_uiMin);
    strResult += "\n";

    // Max
    strResult += "Max: ";
    strResult += _bSigned ? CUtility::toString((int32_t)_uiMax) : CUtility::toString(_uiMax);
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

        }
        signExtend((int32_t&)_uiMin);

        if (xmlElement.hasAttribute("Max")) {

            _uiMax = (uint32_t)xmlElement.getAttributeSignedInteger("Max");

            signExtend((int32_t&)_uiMax);
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

    // Get integer value from the string provided
    int64_t iData;

    if (!convertValueFromString(strValue, iData, parameterAccessContext)) {

        return false;
    }

    // Check against Min / Max
    if (_bSigned) {

        if (bValueProvidedAsHexa && isEncodable((uint64_t)iData, !bValueProvidedAsHexa)) {

            // Sign extend
            signExtend(iData);
        }

        if (!checkValueAgainstRange<int64_t>(strValue, iData, (int32_t)_uiMin, (int32_t)_uiMax, parameterAccessContext, bValueProvidedAsHexa)) {

            return false;
        }
    } else {

        if (!checkValueAgainstRange<uint64_t>(strValue, iData, _uiMin, _uiMax, parameterAccessContext, bValueProvidedAsHexa)) {

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
        strStream << "0x" << std::hex << std::uppercase << std::setw(getSize()*2) << std::setfill('0') << uiValue;
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
    (void)parameterAccessContext;

    // Do assign
    uiUserValue = uiValue;

    return true;
}

// Signed Integer
bool CIntegerParameterType::toBlackboard(int32_t iUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
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
    (void)parameterAccessContext;

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

int CIntegerParameterType::toPlainInteger(int iSizeOptimizedData) const
{
    if (_bSigned) {

        signExtend(iSizeOptimizedData);
    }

    return base::toPlainInteger(iSizeOptimizedData);
}

// Convert value provided by the user as a string into an int64
bool CIntegerParameterType::convertValueFromString(const string& strValue, int64_t& iData, CParameterAccessContext& parameterAccessContext) const {

    // Reset errno to check if it is updated during the conversion (strtol/strtoul)
    errno = 0;
    char *pcStrEnd;

    // Convert the input string
    if (_bSigned) {

        iData = strtoll(strValue.c_str(), &pcStrEnd, 0);
    } else {

        iData = strtoull(strValue.c_str(), &pcStrEnd, 0);
    }

    // Conversion error when the input string does not contain only digits or the number is out of range (int32_t type)
    if (errno || (*pcStrEnd != '\0')) {

        string strError;
        strError =  "Impossible to convert value " + strValue + " for " + getKind();

        parameterAccessContext.setError(strError);

        return false;
    }

    return true;
}

// Range checking
template <typename type> bool CIntegerParameterType::checkValueAgainstRange(const string& strValue, type value, type minValue, type maxValue, CParameterAccessContext& parameterAccessContext, bool bHexaValue) const
{
    if (value < minValue || value > maxValue) {

        ostringstream strStream;

        strStream << "Value " << strValue << " standing out of admitted range [";

        if (bHexaValue) {

            // Format Min
            strStream << "0x" << std::hex << std::uppercase << std::setw(getSize()*2) << std::setfill('0') << makeEncodable(minValue);
            // Format Max
            strStream << ", 0x" << std::hex << std::uppercase << std::setw(getSize()*2) << std::setfill('0') << makeEncodable(maxValue);

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

// From IXmlSource
void CIntegerParameterType::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Sign
    xmlElement.setAttributeBoolean("Signed", _bSigned);

    if (_bSigned) {

        // Mininmum
        xmlElement.setAttributeString("Min", CUtility::toString((int32_t)_uiMin));

        // Maximum
        xmlElement.setAttributeString("Max", CUtility::toString((int32_t)_uiMax));

    } else {

        // Minimum
        xmlElement.setAttributeString("Min", CUtility::toString(_uiMin));

        // Maximum
        xmlElement.setAttributeString("Max", CUtility::toString(_uiMax));
    }

    // Size
    xmlElement.setAttributeString("Size", CUtility::toString(getSize() * 8));

    base::toXml(xmlElement, serializingContext);

}
