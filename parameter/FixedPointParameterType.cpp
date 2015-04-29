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
#include "FixedPointParameterType.h"
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <assert.h>
#include <math.h>
#include "Parameter.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "Utility.h"
#include <errno.h>
#include <convert.hpp>

#define base CParameterType

using std::string;

CFixedPointParameterType::CFixedPointParameterType(const string& strName) : base(strName), _uiIntegral(0), _uiFractional(0)
{
}

string CFixedPointParameterType::getKind() const
{
    return "FixedPointParameter";
}

// Element properties
void CFixedPointParameterType::showProperties(string& strResult) const
{
    base::showProperties(strResult);

    // Notation
    strResult += "Notation: Q";
    strResult += CUtility::toString(_uiIntegral);
    strResult += ".";
    strResult += CUtility::toString(_uiFractional);
    strResult += "\n";
}

// XML Serialization value space handling
// Value space handling for configuration import
void CFixedPointParameterType::handleValueSpaceAttribute(CXmlElement& xmlConfigurableElementSettingsElement, CConfigurationAccessContext& configurationAccessContext) const
{
    // Direction?
    if (!configurationAccessContext.serializeOut()) {

        // Get Value space from XML
        if (xmlConfigurableElementSettingsElement.hasAttribute("ValueSpace")) {

            configurationAccessContext.setValueSpaceRaw(xmlConfigurableElementSettingsElement.getAttributeBoolean("ValueSpace", "Raw"));
        } else {

            configurationAccessContext.setValueSpaceRaw(false);
        }
    } else {
        // Provide value space only if not the default one
        if (configurationAccessContext.valueSpaceIsRaw()) {

            xmlConfigurableElementSettingsElement.setAttributeString("ValueSpace", "Raw");
        }
    }
}

bool CFixedPointParameterType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Size
    uint32_t uiSizeInBits = xmlElement.getAttributeInteger("Size");

    // Q notation
    _uiIntegral = xmlElement.getAttributeInteger("Integral");
    _uiFractional = xmlElement.getAttributeInteger("Fractional");

    // Size vs. Q notation integrity check
    if (uiSizeInBits < getUtilSizeInBits()) {

        serializingContext.setError("Inconsistent Size vs. Q notation for " + getKind() + " " + xmlElement.getPath() + ": Summing (Integral + _uiFractional + 1) should not exceed given Size (" + xmlElement.getAttributeString("Size") + ")");

        return false;
    }

    // Set the size
    setSize(uiSizeInBits / 8);

    return base::fromXml(xmlElement, serializingContext);
}

bool CFixedPointParameterType::toBlackboard(const string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    bool bValueProvidedAsHexa = isHexadecimal(strValue);

    // Check data integrity
    if (bValueProvidedAsHexa && !parameterAccessContext.valueSpaceIsRaw()) {

        parameterAccessContext.setError("Hexadecimal values are not supported for " + getKind() + " when selected value space is real:");

        return false;
    }

    if (parameterAccessContext.valueSpaceIsRaw()) {

        if (bValueProvidedAsHexa) {

            return convertFromHexadecimal(strValue, uiValue, parameterAccessContext);

        }
        return convertFromDecimal(strValue, uiValue, parameterAccessContext);
    }
    return convertFromQnm(strValue, uiValue, parameterAccessContext);
}

void CFixedPointParameterType::setOutOfRangeError(const string& strValue, CParameterAccessContext& parameterAccessContext) const
{
    std::ostringstream strStream;

    strStream << "Value " << strValue << " standing out of admitted ";

    if (!parameterAccessContext.valueSpaceIsRaw()) {

        // Min/Max computation
        double dMin = 0;
        double dMax = 0;
        getRange(dMin, dMax);

        strStream << std::fixed << std::setprecision(_uiFractional)
                  << "real range [" << dMin << ", " << dMax << "]";
    } else {

        // Min/Max computation
        int32_t iMax = getMaxValue<uint32_t>();
        int32_t iMin = -iMax - 1;

        strStream << "raw range [";

        if (isHexadecimal(strValue)) {

            // Format Min
            strStream << "0x" << std::hex << std::uppercase <<
                std::setw(getSize() * 2) << std::setfill('0') << makeEncodable(iMin);
            // Format Max
            strStream << ", 0x" << std::hex << std::uppercase <<
                std::setw(getSize() * 2) << std::setfill('0') << makeEncodable(iMax);

        } else {

            strStream << iMin << ", " << iMax;
        }

        strStream << "]";
    }
    strStream << " for " << getKind();

    parameterAccessContext.setError(strStream.str());
}

bool CFixedPointParameterType::fromBlackboard(string& strValue, const uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    int32_t iData = uiValue;

    // Check encodability
    assert(isEncodable((uint32_t)iData, false));

    // Format
    std::ostringstream strStream;

    // Raw formatting?
    if (parameterAccessContext.valueSpaceIsRaw()) {

        // Hexa formatting?
        if (parameterAccessContext.outputRawFormatIsHex()) {

            strStream << "0x" << std::hex << std::uppercase << std::setw(getSize()*2) << std::setfill('0') << (uint32_t)iData;
        } else {

            // Sign extend
            signExtend(iData);

            strStream << iData;
        }
    } else {

        // Sign extend
        signExtend(iData);

        // Conversion
        double dData = binaryQnmToDouble(iData);

        strStream << std::fixed << std::setprecision(_uiFractional) << dData;
    }

    strValue = strStream.str();

    return true;
}

// Value access
bool CFixedPointParameterType::toBlackboard(double dUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    // Check that the value is within the allowed range for this type
    if (!checkValueAgainstRange(dUserValue)) {

        // Illegal value provided
        parameterAccessContext.setError("Value out of range");

        return false;
    }

    // Do the conversion
    int32_t iData = doubleToBinaryQnm(dUserValue);

    // Check integrity
    assert(isEncodable((uint32_t)iData, true));

    uiValue = iData;

    return true;
}

bool CFixedPointParameterType::fromBlackboard(double& dUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)parameterAccessContext;

    int32_t iData = uiValue;

    // Check unsigned value is encodable
    assert(isEncodable(uiValue, false));

    // Sign extend
    signExtend(iData);

    dUserValue = binaryQnmToDouble(iData);

    return true;
}

// Util size
uint32_t CFixedPointParameterType::getUtilSizeInBits() const
{
    return _uiIntegral + _uiFractional + 1;
}

// Compute the range for the type (minimum and maximum values)
void CFixedPointParameterType::getRange(double& dMin, double& dMax) const
{
    dMax = (double)((1UL << (_uiIntegral + _uiFractional)) - 1) / (1UL << _uiFractional);
    dMin = -(double)(1UL << (_uiIntegral + _uiFractional)) / (1UL << _uiFractional);
}

bool CFixedPointParameterType::isHexadecimal(const string& strValue) const
{
    return !strValue.compare(0, 2, "0x");
}

bool CFixedPointParameterType::convertFromHexadecimal(const string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    // For hexadecimal representation, we need full 32 bit range conversion.
    uint32_t uiData;
    if (!convertTo(strValue, uiData) || !isEncodable(uiData, false)) {

        setOutOfRangeError(strValue, parameterAccessContext);
        return false;
    }
    signExtend((int32_t&)uiData);

    // check that the data is encodable and can been safely written to the blackboard
    assert(isEncodable(uiData, true));
    uiValue = uiData;

    return true;
}

bool CFixedPointParameterType::convertFromDecimal(const string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    int32_t iData;

    if (!convertTo(strValue, iData) || !isEncodable((uint32_t)iData, true)) {

        setOutOfRangeError(strValue, parameterAccessContext);
        return false;
    }
    uiValue = static_cast<uint32_t>(iData);

    return true;
}

bool CFixedPointParameterType::convertFromQnm(const string& strValue, uint32_t& uiValue,
                                              CParameterAccessContext& parameterAccessContext) const
{
    double dData;

    if (!convertTo(strValue, dData) || !checkValueAgainstRange(dData)) {

        setOutOfRangeError(strValue, parameterAccessContext);
        return false;
    }
    uiValue = static_cast<uint32_t>(doubleToBinaryQnm(dData));

    // check that the data is encodable and has been safely written to the blackboard
    assert(isEncodable(uiValue, true));

    return true;
}

// Check that the value is within available range for this type
bool CFixedPointParameterType::checkValueAgainstRange(double dValue) const
{
    double dMin = 0;
    double dMax = 0;
    getRange(dMin, dMax);

    return (dValue <= dMax) && (dValue >= dMin);
}

// Data conversion
int32_t CFixedPointParameterType::doubleToBinaryQnm(double dValue) const
{
    // For Qn.m number, multiply by 2^n and round to the nearest integer
    int32_t iData = static_cast<int32_t>(round(dValue * (1UL << _uiFractional)));
    // Left justify
    // For a Qn.m number, shift 32 - (n + m + 1) bits to the left (the rest of
    // the bits aren't used)
    iData <<= getSize() * 8 - getUtilSizeInBits();

    return iData;
}


double CFixedPointParameterType::binaryQnmToDouble(int32_t iValue) const
{
    // Unjustify
    iValue >>= getSize() * 8 - getUtilSizeInBits();
    return static_cast<double>(iValue) / (1UL << _uiFractional);
}

// From IXmlSource
void CFixedPointParameterType::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Size
    xmlElement.setAttributeString("Size", CUtility::toString(getSize() * 8));

    // Integral
    xmlElement.setAttributeString("Integral", CUtility::toString(_uiIntegral));

    // Fractional
    xmlElement.setAttributeString("Fractional", CUtility::toString(_uiFractional));

    base::toXml(xmlElement, serializingContext);
}
