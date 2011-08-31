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
#include "FixedPointParameterType.h"
#include <stdlib.h>
#include <sstream>
#include "Parameter.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"

#define base CParameterType

CFixedPointParameterType::CFixedPointParameterType(const string& strName) : base(strName), _uiIntegral(0), _uiFractional(0)
{
}

string CFixedPointParameterType::getKind() const
{
    return "FixedPointParameter";
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

        serializingContext.setError("Inconsistent Size vs. Q notation for " + getKind() + " " + xmlElement.getPath() + ": Summing (Integral + _uiFractional + 1) should be less than given Size (" + xmlElement.getAttributeString("Size") + ")");

        return false;
    }

    // Set the size
    setSize(uiSizeInBits / 8);

    return base::fromXml(xmlElement, serializingContext);
}

bool CFixedPointParameterType::asInteger(const string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    int32_t iData;

    if (parameterAccessContext.valueSpaceIsRaw()) {

        iData = strtol(strValue.c_str(), NULL, 0);

    } else {
        double dData = strtod(strValue.c_str(), NULL);

        // Do the conversion
        iData = (int32_t)(dData * (1UL << _uiFractional) + 0.5F - (double)(dData < 0));
    }

    // Check for admitted range: [-2^m, 2^m - 2^n]
    uint32_t uiSizeInBits = getUtilSizeInBits();

    int32_t iMin = ((int32_t)1 << 31) >> (32 - uiSizeInBits);
    int32_t iMax = -iMin - 1;

    if (iData < iMin || iData > iMax) {
        ostringstream strStream;

        strStream << "Value " << strValue << " standing out of admitted ";

        if (!parameterAccessContext.valueSpaceIsRaw()) {

            strStream << "real range [" << (double)iMin / (1UL << _uiFractional) << ", "<< (double)iMax / (1UL << _uiFractional) << "]";
        } else {

            strStream << "raw range ["  << iMin << ", " << iMax << "]";
        }
        strStream <<  " for " << getKind();

        parameterAccessContext.setError(strStream.str());

        return false;
    }

    uiValue = (uint32_t)iData;

    return true;
}

void CFixedPointParameterType::asString(const uint32_t& uiValue, string& strValue, CParameterAccessContext& parameterAccessContext) const
{
    int32_t iData = (int32_t)uiValue;

    // Sign extend
    uint32_t uiShift = 32 - getUtilSizeInBits();

    if (uiShift) {

        iData = (iData << uiShift) >> uiShift;
    }

    // Format
    ostringstream strStream;

    if (parameterAccessContext.valueSpaceIsRaw()) {

        strStream << iData;
    } else {

        double dData = (double)iData / (1UL << _uiFractional);

        strStream << dData;
    }

    strValue = strStream.str();
}

// Util size
uint32_t CFixedPointParameterType::getUtilSizeInBits() const
{
    return _uiIntegral + _uiFractional + 1;
}
