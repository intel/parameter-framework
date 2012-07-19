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
#include "FixedPointParameterType.h"
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <assert.h>
#include <math.h>
#include "Parameter.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include <errno.h>

#define base CParameterType

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
    strResult += toString(_uiIntegral);
    strResult += ".";
    strResult += toString(_uiFractional);
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
    // Hexa
    bool bValueProvidedAsHexa = !strValue.compare(0, 2, "0x");

    // Check data integrity
    if (bValueProvidedAsHexa && !parameterAccessContext.valueSpaceIsRaw()) {

        parameterAccessContext.setError("Hexadecimal values are not supported for " + getKind() + " when selected value space is real:");

        return false;
    }

    int64_t iData;

    if (parameterAccessContext.valueSpaceIsRaw()) {
        errno = 0;
        char *pcStrEnd;

        // Get data in integer form
        iData = strtoll(strValue.c_str(), &pcStrEnd, 0);

        // Conversion error when the input string does not contain any digit or the number is out of range
        bool bConversionSucceeded = !errno && (strValue.c_str() != pcStrEnd);

        if (!bConversionSucceeded || !isEncodable((uint64_t)iData, !bValueProvidedAsHexa)) {

            // Illegal value provided
            parameterAccessContext.setError(getOutOfRangeError(strValue, parameterAccessContext.valueSpaceIsRaw(), bValueProvidedAsHexa));

            return false;
        }
        if (bValueProvidedAsHexa) {

            // Sign extend
            signExtend(iData);
        }

    } else {
        errno = 0;
        char *pcStrEnd;

        double dData = strtod(strValue.c_str(), &pcStrEnd);

        // Conversion error when the input string does not contain any digit or the number is out of range (int32_t type)
        bool bConversionSucceeded = !errno && (strValue.c_str() != pcStrEnd);

        // Check encodability
        if (!bConversionSucceeded || !checkValueAgainstRange(dData)) {

            // Illegal value provided
            parameterAccessContext.setError(getOutOfRangeError(strValue, parameterAccessContext.valueSpaceIsRaw(), bValueProvidedAsHexa));

            return false;
        }

        // Do the conversion
        iData = asInteger(dData);
    }

    // check that the data is encodable and can be safely written to the blackboard
    assert(isEncodable((unsigned long int)iData, true));

    uiValue = (uint32_t)iData;

    return true;
}

bool CFixedPointParameterType::fromBlackboard(string& strValue, const uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    int32_t iData = uiValue;

    // Check encodability
    assert(isEncodable((uint32_t)iData, false));

    // Format
    ostringstream strStream;

    // Raw formatting?
    if (parameterAccessContext.valueSpaceIsRaw()) {

        // Hexa formatting?
        if (parameterAccessContext.outputRawFormatIsHex()) {

            strStream << "0x" << hex << uppercase << setw(getSize()*2) << setfill('0') << (uint32_t)iData;
        } else {

            // Sign extend
            signExtend(iData);

            strStream << iData;
        }
    } else {

        // Sign extend
        signExtend(iData);

        // Conversion
        double dData = asDouble(iData);

        // Set up the precision of the display and notation type
        int iPrecision = (_uiFractional  * log10(2.0)) + 1;
        int iFactor = pow(10.0, iPrecision);
        strStream << fixed << ((int64_t)(dData * iFactor)) / (double)iFactor;
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
    int32_t iData = asInteger(dUserValue);

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

    dUserValue = asDouble(iData);

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

// Out of range error
string CFixedPointParameterType::getOutOfRangeError(const string& strValue, bool bRawValueSpace, bool bHexaValue) const
{


    ostringstream strStream;

    strStream << "Value " << strValue << " standing out of admitted ";

    if (!bRawValueSpace) {

        // Min/Max computation
        double dMin = 0;
        double dMax = 0;
        getRange(dMin, dMax);

        strStream << "real range [" << dMin << ", "<< dMax << "]";
    } else {

        // Min/Max computation
        int32_t iMax = (1L << (getSize() * 8 - 1)) - 1;
        int32_t iMin = -iMax - 1;

        strStream << "raw range [";

        if (bHexaValue) {

            // Format Min
            strStream << "0x" << hex << uppercase << setw(getSize()*2) << setfill('0') << makeEncodable(iMin);
            // Format Max
            strStream << ", 0x" << hex << uppercase << setw(getSize()*2) << setfill('0') << makeEncodable(iMax);

        } else {

            strStream << iMin << ", " << iMax;
        }

        strStream << "]";
    }
    strStream <<  " for " << getKind();

    return strStream.str();
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
int32_t CFixedPointParameterType::asInteger(double dValue) const
{
    // Do the conversion
    int32_t iData = (int32_t)(dValue * (1UL << _uiFractional) + 0.5F - (double)(dValue < 0));
    // Left justify
    iData <<= getSize() * 8 - getUtilSizeInBits();

    return iData;
}

double CFixedPointParameterType::asDouble(int32_t iValue) const
{
    // Unjustify
    iValue >>= getSize() * 8 - getUtilSizeInBits();
    // Convert
    return (double)iValue / (1UL << _uiFractional);
}
