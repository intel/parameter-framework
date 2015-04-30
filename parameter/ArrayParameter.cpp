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
#include "ArrayParameter.h"
#include <sstream> // for istringstream
#include "Tokenizer.h"
#include "ParameterType.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "ParameterBlackboard.h"
#include "Utility.h"
#include <assert.h>

#define base CParameter

using std::string;

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
    strResult += CUtility::toString(getArrayLength());
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
bool CArrayParameter::accessValue(CPathNavigator& pathNavigator, string& strValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiIndex;

    if (!getIndex(pathNavigator, uiIndex, parameterAccessContext)) {

        return false;
    }

    if (bSet) {
        // Set
        if (uiIndex == (uint32_t)-1) {

            // No index provided, start with 0
            uiIndex = 0;
        }

        // Actually set values
        if (!setValues(uiIndex, parameterAccessContext.getBaseOffset(), strValue, parameterAccessContext)) {

            return false;
        }

        // Synchronize
        if (!sync(parameterAccessContext)) {

            appendParameterPathToError(parameterAccessContext);
            return false;
        }
    } else {
        // Get
        if (uiIndex == (uint32_t)-1) {

            // Whole array requested
            getValues(parameterAccessContext.getBaseOffset(), strValue, parameterAccessContext);

        } else {
            // Scalar requested
            doGetValue(strValue, getOffset() + uiIndex * getSize(), parameterAccessContext);
        }
    }

    return true;
}

// Boolean
bool CArrayParameter::accessAsBooleanArray(std::vector<bool>& abValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    return accessValues(abValues, bSet, parameterAccessContext);
}

// Integer
bool CArrayParameter::accessAsIntegerArray(std::vector<uint32_t>& auiValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    return accessValues(auiValues, bSet, parameterAccessContext);
}

// Signed Integer Access
bool CArrayParameter::accessAsSignedIntegerArray(std::vector<int32_t>& aiValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    return accessValues(aiValues, bSet, parameterAccessContext);
}

// Double Access
bool CArrayParameter::accessAsDoubleArray(std::vector<double>& adValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    return accessValues(adValues, bSet, parameterAccessContext);
}

// String Access
bool CArrayParameter::accessAsStringArray(std::vector<string>& astrValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    return accessValues(astrValues, bSet, parameterAccessContext);
}

// Dump
void CArrayParameter::logValue(string& strValue, CErrorContext& errorContext) const
{
    // Parameter context
    CParameterAccessContext& parameterAccessContext = static_cast<CParameterAccessContext&>(errorContext);

    // Dump values
    getValues(0, strValue, parameterAccessContext);
}

// Used for simulation and virtual subsystems
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
        pBlackboard->writeInteger(&uiDefaultValue, uiSize, uiOffset, bSubsystemIsBigEndian);

        uiOffset += uiSize;
    }
}

// Index from path
bool CArrayParameter::getIndex(CPathNavigator& pathNavigator, uint32_t& uiIndex, CParameterAccessContext& parameterAccessContext) const
{
    uiIndex = (uint32_t)-1;

    string* pStrChildName = pathNavigator.next();

    if (pStrChildName) {

        // Check index is numeric
	std::istringstream iss(*pStrChildName);

        iss >> uiIndex;

        if (!iss) {

            parameterAccessContext.setError("Expected numerical expression as last item in " + pathNavigator.getCurrentPath());

            return false;
        }

        if (uiIndex >= getArrayLength()) {
	    std::ostringstream oss;

            oss << "Provided index out of range (max is " << getArrayLength() - 1 << ")";

            parameterAccessContext.setError(oss.str());

            return false;
        }

        // Check no other item provided in path
        pStrChildName = pathNavigator.next();

        if (pStrChildName) {

            // Should be leaf element
            parameterAccessContext.setError("Path not found: " + pathNavigator.getCurrentPath());

            return false;
        }
    }

    return true;
}

// Common set value processing
bool CArrayParameter::setValues(uint32_t uiStartIndex, uint32_t uiBaseOffset, const string& strValue, CParameterAccessContext& parameterAccessContext) const
{
    // Deal with value(s)
    Tokenizer tok(strValue, Tokenizer::defaultDelimiters + ",");

    std::vector<string> astrValues = tok.split();
    size_t uiNbValues = astrValues.size();

    // Check number of provided values
    if (uiNbValues + uiStartIndex > getArrayLength()) {

        // Out of bounds
        parameterAccessContext.setError("Too many values provided");

        return false;
    }

    // Process
    uint32_t uiValueIndex;
    uint32_t uiSize = getSize();
    uint32_t uiOffset = getOffset() + uiStartIndex * uiSize - uiBaseOffset;

    for (uiValueIndex = 0; uiValueIndex < uiNbValues; uiValueIndex++) {

        if (!doSetValue(astrValues[uiValueIndex], uiOffset, parameterAccessContext)) {

            // Append parameter path to error
            parameterAccessContext.appendToError(" " + getPath() + "/" +
                                                 CUtility::toString(uiValueIndex + uiStartIndex));

            return false;
        }

        uiOffset += uiSize;
    }
    return true;
}

// Common get value processing
void CArrayParameter::getValues(uint32_t uiBaseOffset, string& strValues, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiValueIndex;
    uint32_t uiSize = getSize();
    uint32_t uiOffset = getOffset() - uiBaseOffset;
    uint32_t uiArrayLength = getArrayLength();

    strValues.clear();

    bool bFirst = true;

    for (uiValueIndex = 0; uiValueIndex < uiArrayLength; uiValueIndex++) {
        string strReadValue;

        doGetValue(strReadValue, uiOffset, parameterAccessContext);

        if (!bFirst) {

            strValues += " ";
        } else {

            bFirst = false;
        }

        strValues += strReadValue;

        uiOffset += uiSize;
    }
}

// Generic Access
template <typename type>
bool CArrayParameter::accessValues(std::vector<type>& values, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    if (bSet) {

        // Set Value
        if (!setValues(values, parameterAccessContext)) {

            appendParameterPathToError(parameterAccessContext);
            return false;
        }
        if (!sync(parameterAccessContext)) {

            appendParameterPathToError(parameterAccessContext);
            return false;
        }
    } else {
        // Get Value
        if (!getValues(values, parameterAccessContext)) {

            appendParameterPathToError(parameterAccessContext);
            return false;
        }
    }
    return true;
}

template <typename type>
bool CArrayParameter::setValues(const std::vector<type>& values, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiNbValues = getArrayLength();
    uint32_t uiValueIndex;
    uint32_t uiSize = getSize();
    uint32_t uiOffset = getOffset();

    assert(values.size() == uiNbValues);

    // Process
    for (uiValueIndex = 0; uiValueIndex < uiNbValues; uiValueIndex++) {

        if (!doSet(values[uiValueIndex], uiOffset, parameterAccessContext)) {

            return false;
        }

        uiOffset += uiSize;
    }

   return true;
}

template <typename type>
bool CArrayParameter::getValues(std::vector<type>& values, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiNbValues = getArrayLength();
    uint32_t uiValueIndex;
    uint32_t uiSize = getSize();
    uint32_t uiOffset = getOffset();

    values.clear();

    for (uiValueIndex = 0; uiValueIndex < uiNbValues; uiValueIndex++) {
        type readValue;

        if (!doGet(readValue, uiOffset, parameterAccessContext)) {

            return false;
        }

        values.push_back(readValue);

        uiOffset += uiSize;
    }
    return true;
}

template <typename type>
bool CArrayParameter::doSet(type value, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiData;

    if (!static_cast<const CParameterType*>(getTypeElement())->toBlackboard(value, uiData, parameterAccessContext)) {

        return false;
    }
    // Write blackboard
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->writeInteger(&uiData, getSize(), uiOffset, parameterAccessContext.isBigEndianSubsystem());

    return true;
}

template <typename type>
bool CArrayParameter::doGet(type& value, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiData = 0;

    // Read blackboard
    const CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->readInteger(&uiData, getSize(), uiOffset, parameterAccessContext.isBigEndianSubsystem());

    return static_cast<const CParameterType*>(getTypeElement())->fromBlackboard(value, uiData, parameterAccessContext);
}

