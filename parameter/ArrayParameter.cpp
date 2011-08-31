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
#include "ArrayParameter.h"
#include <sstream> // for istringstream
#include "Tokenizer.h"
#include "ParameterType.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "ParameterBlackboard.h"

#define base CParameter

CArrayParameter::CArrayParameter(const string& strName, const CTypeElement* pTypeElement) : base(strName, pTypeElement)
{
}

uint32_t CArrayParameter::getFootPrint() const
{
    return getSize() * getArrayLength();
}

// Array length
uint32_t CArrayParameter::getArrayLength() const
{
    return getTypeElement()->getArrayLength();
}

// Element properties
void CArrayParameter::showProperties(string& strResult) const
{
    base::showProperties(strResult);

    // Array length
    strResult += "Array length: ";
    strResult += toString(getArrayLength());
    strResult += "\n";
}

// XML configuration settings parsing
bool CArrayParameter::serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const
{
    // Check for value space
    handleValueSpaceAttribute(xmlConfigurationSettingsElementContent, configurationAccessContext);

    // Handle access
    if (!configurationAccessContext.serializeOut()) {

        // Actually set values to blackboard
        if (!setValues(0, configurationAccessContext.getBaseOffset(), xmlConfigurationSettingsElementContent.getTextContent(), configurationAccessContext)) {

            return false;
        }
    } else {

        // Get string value
        string strValue;

        // Whole array requested
        getValues(configurationAccessContext.getBaseOffset(), strValue, configurationAccessContext);

        // Populate value into xml text node
        xmlConfigurationSettingsElementContent.setTextContent(strValue);
    }

    // Done
    return true;
}

// User set/get
bool CArrayParameter::setValue(CPathNavigator& pathNavigator, const string& strValue, CParameterAccessContext& parameterContext) const
{
    uint32_t uiStartIndex;

    if (!getIndex(pathNavigator, uiStartIndex, parameterContext)) {

        return false;
    }

    if (uiStartIndex == (uint32_t)-1) {

        // No index provided, start with 0
        uiStartIndex = 0;
    }

    // Actually set values
    if (!setValues(uiStartIndex, 0, strValue, parameterContext)) {

        return false;
    }

    // Synchronize
    if (parameterContext.getAutoSync() && !sync(parameterContext)) {

        // Append parameter path to error
        parameterContext.appendToError(" " + getPath());

        return false;
    }

    return true;
}

bool CArrayParameter::getValue(CPathNavigator& pathNavigator, string& strValue, CParameterAccessContext& parameterContext) const
{
    uint32_t uiIndex;

    if (!getIndex(pathNavigator, uiIndex, parameterContext)) {

        return false;
    }
    if (uiIndex == (uint32_t)-1) {

        // Whole array requested
        getValues(0, strValue, parameterContext);

    } else {
        // Scalar requested
        doGetValue(strValue, getOffset() + uiIndex * getSize(), parameterContext);
    }

    return true;
}

void CArrayParameter::logValue(string& strValue, CErrorContext& errorContext) const
{
    // Parameter context
    CParameterAccessContext& parameterContext = static_cast<CParameterAccessContext&>(errorContext);

    // Dump values
    getValues(0, strValue, parameterContext);
}

// Used for simulation only
void CArrayParameter::setDefaultValues(CParameterAccessContext& parameterAccessContext) const
{
    // Get default value from type
    uint32_t uiDefaultValue = static_cast<const CParameterType*>(getTypeElement())->getDefaultValue();

    // Write blackboard
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Process
    uint32_t uiValueIndex;
    uint32_t uiSize = getSize();
    uint32_t uiOffset = getOffset();
    bool bSubsystemIsBigEndian = parameterAccessContext.isBigEndianSubsystem();
    uint32_t uiArrayLength = getArrayLength();

    for (uiValueIndex = 0; uiValueIndex < uiArrayLength; uiValueIndex++) {

        // Beware this code works on little endian architectures only!
        pBlackboard->write(&uiDefaultValue, uiSize, uiOffset, bSubsystemIsBigEndian);

        uiOffset += uiSize;
    }
}

// Index from path
bool CArrayParameter::getIndex(CPathNavigator& pathNavigator, uint32_t& uiIndex, CParameterAccessContext& parameterContext) const
{
    uiIndex = (uint32_t)-1;

    string* pStrChildName = pathNavigator.next();

    if (pStrChildName) {

        // Check index is numeric
        istringstream iss(*pStrChildName);

        iss >> uiIndex;

        if (!iss) {

            parameterContext.setError("Expected numerical expression as last item in " + pathNavigator.getCurrentPath());

            return false;
        }

        if (uiIndex >= getArrayLength()) {
            ostringstream oss;

            oss << "Provided index out of range (max is " << getArrayLength() - 1 << ")";

            parameterContext.setError(oss.str());

            return false;
        }

        // Check no other item provided in path
        pStrChildName = pathNavigator.next();

        if (pStrChildName) {

            // Should be leaf element
            parameterContext.setError("Path not found: " + pathNavigator.getCurrentPath());

            return false;
        }
    }

    return true;
}

// Common set value processing
bool CArrayParameter::setValues(uint32_t uiStartIndex, uint32_t uiBaseOffset, const string& strValue, CParameterAccessContext& parameterContext) const
{
    // Deal with value(s)
    Tokenizer tok(strValue, DEFAULT_DELIMITER + ",");

    vector<string> astrValues = tok.split();
    uint32_t uiNbValues = astrValues.size();

    // Check number of provided values
    if (uiNbValues + uiStartIndex >  getArrayLength()) {

        // Out of bounds
        parameterContext.setError("Too many values provided");

        return false;
    }

    // Process
    uint32_t uiValueIndex;
    uint32_t uiSize = getSize();
    uint32_t uiOffset = getOffset() + uiStartIndex * uiSize - uiBaseOffset;

    for (uiValueIndex = 0; uiValueIndex < uiNbValues; uiValueIndex++) {

        if (!doSetValue(astrValues[uiValueIndex], uiOffset, parameterContext)) {

            // Append parameter path to error
            parameterContext.appendToError(" " + getPath() + "/" + toString(uiValueIndex + uiStartIndex));

            return false;
        }

        uiOffset += uiSize;
    }
    return true;
}

// Common get value processing
void CArrayParameter::getValues(uint32_t uiBaseOffset, string& strValues, CParameterAccessContext& parameterContext) const
{
    uint32_t uiValueIndex;
    uint32_t uiSize = getSize();
    uint32_t uiOffset = getOffset() - uiBaseOffset;
    uint32_t uiArrayLength = getArrayLength();

    bool bFirst = true;

    for (uiValueIndex = 0; uiValueIndex < uiArrayLength; uiValueIndex++) {
        string strReadValue;

        doGetValue(strReadValue, uiOffset, parameterContext);

        if (!bFirst) {

            strValues += " ";
        } else {

            bFirst = false;
        }

        strValues += strReadValue;

        uiOffset += uiSize;
    }
}
