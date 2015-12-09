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

CIntegerParameterType::CIntegerParameterType(const string &strName) : base(strName)
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
void CIntegerParameterType::showProperties(string &strResult) const
{
    base::showProperties(strResult);

    // Sign
    strResult += "Signed: ";
    strResult += _bSigned ? "yes" : "no";
    strResult += "\n";

    // Min
    strResult += "Min: ";
    strResult += _bSigned ? std::to_string((int32_t)_uiMin) : std::to_string(_uiMin);
    strResult += "\n";

    // Max
    strResult += "Max: ";
    strResult += _bSigned ? std::to_string((int32_t)_uiMax) : std::to_string(_uiMax);
    strResult += "\n";

    // Check if there's an adaptation object available
    const CParameterAdaptation *pParameterAdaption = getParameterAdaptation();

    if (pParameterAdaption) {

        // Display adaptation properties
        strResult += "Adaptation:\n";

        pParameterAdaption->showProperties(strResult);
    }
}

bool CIntegerParameterType::fromXml(const CXmlElement &xmlElement,
                                    CXmlSerializingContext &serializingContext)
{
    // Sign
    xmlElement.getAttribute("Signed", _bSigned);

    // Size in bits
    size_t sizeInBits = 0;
    xmlElement.getAttribute("Size", sizeInBits);

    // Size
    setSize(sizeInBits / 8);

    // Min / Max
    // TODO: Make IntegerParameter template
    if (_bSigned) {

        // Signed means we have one less util bit
        sizeInBits--;

        if (!xmlElement.getAttribute("Min", (int32_t &)_uiMin)) {

            _uiMin = 1U << sizeInBits;
        }

        if (!xmlElement.getAttribute("Max", (int32_t &)_uiMax)) {

            _uiMax = (1U << sizeInBits) - 1;
        }
        signExtend((int32_t &)_uiMin);
        signExtend((int32_t &)_uiMax);
    } else {
        if (!xmlElement.getAttribute("Min", _uiMin)) {

            _uiMin = 0;
        }

        if (!xmlElement.getAttribute("Max", _uiMax)) {

            _uiMax = ~0U >> (8 * sizeof(size_t) - sizeInBits);
        }
    }

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// Conversion (tuning)
bool CIntegerParameterType::toBlackboard(const string &strValue, uint32_t &uiValue,
                                         CParameterAccessContext &parameterAccessContext) const
{
    // Hexa
    bool bValueProvidedAsHexa = utility::isHexadecimal(strValue);

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

        if (!checkValueAgainstRange<int64_t>(strValue, iData, (int32_t)_uiMin, (int32_t)_uiMax,
                                             parameterAccessContext, bValueProvidedAsHexa)) {

            return false;
        }
    } else {

        if (!checkValueAgainstRange<uint64_t>(strValue, iData, _uiMin, _uiMax,
                                              parameterAccessContext, bValueProvidedAsHexa)) {

            return false;
        }
    }

    uiValue = (uint32_t)iData;

    return true;
}

bool CIntegerParameterType::fromBlackboard(string &strValue, const uint32_t &value,
                                           CParameterAccessContext &parameterAccessContext) const
{
    // Check unsigned value is encodable
    assert(isEncodable(value, false));

    // Format
    ostringstream stream;

    // Take care of format
    if (parameterAccessContext.valueSpaceIsRaw() && parameterAccessContext.outputRawFormatIsHex()) {

        // Hexa display with unecessary bits cleared out
        stream << "0x" << std::hex << std::uppercase << std::setw(static_cast<int>(getSize() * 2))
               << std::setfill('0') << value;
    } else {

        if (_bSigned) {

            int32_t iValue = value;

            // Sign extend
            signExtend(iValue);

            stream << iValue;
        } else {

            stream << value;
        }
    }

    strValue = stream.str();

    return true;
}

// Value access
// Integer
bool CIntegerParameterType::toBlackboard(uint32_t uiUserValue, uint32_t &uiValue,
                                         CParameterAccessContext &parameterAccessContext) const
{
    if (uiUserValue < _uiMin || uiUserValue > _uiMax) {

        parameterAccessContext.setError("Value out of range");

        return false;
    }
    // Do assign
    uiValue = uiUserValue;

    return true;
}

bool CIntegerParameterType::fromBlackboard(uint32_t &uiUserValue, uint32_t uiValue,
                                           CParameterAccessContext & /*ctx*/) const
{
    // Do assign
    uiUserValue = uiValue;

    return true;
}

// Signed Integer
bool CIntegerParameterType::toBlackboard(int32_t iUserValue, uint32_t &uiValue,
                                         CParameterAccessContext &parameterAccessContext) const
{
    if (iUserValue < (int32_t)_uiMin || iUserValue > (int32_t)_uiMax) {

        parameterAccessContext.setError("Value out of range");

        return false;
    }
    // Do assign
    uiValue = iUserValue;

    return true;
}

bool CIntegerParameterType::fromBlackboard(int32_t &iUserValue, uint32_t uiValue,
                                           CParameterAccessContext & /*ctx*/) const
{
    int32_t iValue = uiValue;

    // Sign extend
    signExtend(iValue);

    // Do assign
    iUserValue = iValue;

    return true;
}

// Double
bool CIntegerParameterType::toBlackboard(double dUserValue, uint32_t &uiValue,
                                         CParameterAccessContext &parameterAccessContext) const
{
    // Check if there's an adaptation object available
    const CParameterAdaptation *pParameterAdaption = getParameterAdaptation();

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

bool CIntegerParameterType::fromBlackboard(double &dUserValue, uint32_t uiValue,
                                           CParameterAccessContext &parameterAccessContext) const
{
    // Check if there's an adaptation object available
    const CParameterAdaptation *pParameterAdaption = getParameterAdaptation();

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
bool CIntegerParameterType::convertValueFromString(
    const string &strValue, int64_t &iData, CParameterAccessContext &parameterAccessContext) const
{

    // Reset errno to check if it is updated during the conversion (strtol/strtoul)
    errno = 0;
    char *pcStrEnd;

    // Convert the input string
    if (_bSigned) {

        iData = strtoll(strValue.c_str(), &pcStrEnd, 0);
    } else {

        iData = strtoull(strValue.c_str(), &pcStrEnd, 0);
    }

    // Conversion error when the input string does not contain only digits or the number is out of
    // range (int32_t type)
    if (errno || (*pcStrEnd != '\0')) {

        string strError;
        strError = "Impossible to convert value " + strValue + " for " + getKind();

        parameterAccessContext.setError(strError);

        return false;
    }

    return true;
}

// Range checking
template <typename type>
bool CIntegerParameterType::checkValueAgainstRange(const string &strValue, type value,
                                                   type minValue, type maxValue,
                                                   CParameterAccessContext &parameterAccessContext,
                                                   bool bHexaValue) const
{
    if (value < minValue || value > maxValue) {

        ostringstream stream;

        stream << "Value " << strValue << " standing out of admitted range [";

        if (bHexaValue) {

            stream << "0x" << std::hex << std::uppercase
                   << std::setw(static_cast<int>(getSize() * 2)) << std::setfill('0');
            // Format Min
            stream << minValue;
            // Format Max
            stream << maxValue;

        } else {

            stream << minValue << ", " << maxValue;
        }

        stream << "] for " << getKind();

        parameterAccessContext.setError(stream.str());

        return false;
    }
    return true;
}

// Adaptation element retrieval
const CParameterAdaptation *CIntegerParameterType::getParameterAdaptation() const
{
    return static_cast<const CParameterAdaptation *>(findChildOfKind("Adaptation"));
}

// From IXmlSource
void CIntegerParameterType::toXml(CXmlElement &xmlElement,
                                  CXmlSerializingContext &serializingContext) const
{
    // Sign
    xmlElement.setAttribute("Signed", _bSigned);

    if (_bSigned) {

        // Mininmum
        xmlElement.setAttribute("Min", (int32_t)_uiMin);

        // Maximum
        xmlElement.setAttribute("Max", (int32_t)_uiMax);

    } else {

        // Minimum
        xmlElement.setAttribute("Min", _uiMin);

        // Maximum
        xmlElement.setAttribute("Max", _uiMax);
    }

    // Size
    xmlElement.setAttribute("Size", getSize() * 8);

    base::toXml(xmlElement, serializingContext);
}
