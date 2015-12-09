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

CArrayParameter::CArrayParameter(const string &strName, const CTypeElement *pTypeElement)
    : base(strName, pTypeElement)
{
}

size_t CArrayParameter::getFootPrint() const
{
    return getSize() * getArrayLength();
}

// Array length
size_t CArrayParameter::getArrayLength() const
{
    return getTypeElement()->getArrayLength();
}

// Element properties
void CArrayParameter::showProperties(string &strResult) const
{
    base::showProperties(strResult);

    // Array length
    strResult += "Array length: ";
    strResult += std::to_string(getArrayLength());
    strResult += "\n";
}

// User set/get
bool CArrayParameter::accessValue(CPathNavigator &pathNavigator, string &strValue, bool bSet,
                                  CParameterAccessContext &parameterAccessContext) const
{
    size_t index;

    if (!getIndex(pathNavigator, index, parameterAccessContext)) {

        return false;
    }

    if (bSet) {
        // Set
        if (index == (size_t)-1) {

            // No index provided, start with 0
            index = 0;
        }

        // Actually set values
        if (!setValues(index, getOffset() - parameterAccessContext.getBaseOffset(), strValue,
                       parameterAccessContext)) {
            return false;
        }

        // Synchronize
        if (!sync(parameterAccessContext)) {

            appendParameterPathToError(parameterAccessContext);
            return false;
        }
    } else {
        // Get
        if (index == (size_t)-1) {

            // Whole array requested
            strValue = getValues(getOffset() - parameterAccessContext.getBaseOffset(),
                                 parameterAccessContext);

        } else {
            // Scalar requested
            CParameter::doGetValue(strValue, getOffset() + index * getSize(),
                                   parameterAccessContext);
        }
    }

    return true;
}

/// Actual parameter access
// String access
bool CArrayParameter::doSetValue(const string &value, size_t offset,
                                 CParameterAccessContext &parameterAccessContext) const
{
    return setValues(0, offset, value, parameterAccessContext);
}

void CArrayParameter::doGetValue(string &value, size_t offset,
                                 CParameterAccessContext &parameterAccessContext) const
{
    // Whole array requested
    value = getValues(offset, parameterAccessContext);
}

// Boolean
bool CArrayParameter::access(std::vector<bool> &abValues, bool bSet,
                             CParameterAccessContext &parameterAccessContext) const
{
    return accessValues(abValues, bSet, parameterAccessContext);
}

// Integer
bool CArrayParameter::access(std::vector<uint32_t> &auiValues, bool bSet,
                             CParameterAccessContext &parameterAccessContext) const
{
    return accessValues(auiValues, bSet, parameterAccessContext);
}

// Signed Integer Access
bool CArrayParameter::access(std::vector<int32_t> &aiValues, bool bSet,
                             CParameterAccessContext &parameterAccessContext) const
{
    return accessValues(aiValues, bSet, parameterAccessContext);
}

// Double Access
bool CArrayParameter::access(std::vector<double> &adValues, bool bSet,
                             CParameterAccessContext &parameterAccessContext) const
{
    return accessValues(adValues, bSet, parameterAccessContext);
}

// String Access
bool CArrayParameter::access(std::vector<string> &astrValues, bool bSet,
                             CParameterAccessContext &parameterAccessContext) const
{
    return accessValues(astrValues, bSet, parameterAccessContext);
}

// Dump
string CArrayParameter::logValue(CParameterAccessContext &context) const
{
    // Dump values
    return getValues(0, context);
}

// Used for simulation and virtual subsystems
void CArrayParameter::setDefaultValues(CParameterAccessContext &parameterAccessContext) const
{
    // Get default value from type
    uint32_t uiDefaultValue =
        static_cast<const CParameterType *>(getTypeElement())->getDefaultValue();

    // Write blackboard
    CParameterBlackboard *pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Process
    size_t valueIndex;
    size_t size = getSize();
    size_t offset = getOffset();
    size_t arrayLength = getArrayLength();

    for (valueIndex = 0; valueIndex < arrayLength; valueIndex++) {

        // Beware this code works on little endian architectures only!
        pBlackboard->writeInteger(&uiDefaultValue, size, offset);

        offset += size;
    }
}

// Index from path
bool CArrayParameter::getIndex(CPathNavigator &pathNavigator, size_t &index,
                               CParameterAccessContext &parameterAccessContext) const
{
    index = (size_t)-1;

    string *pStrChildName = pathNavigator.next();

    if (pStrChildName) {

        // Check index is numeric
        std::istringstream iss(*pStrChildName);

        iss >> index;

        if (!iss) {

            parameterAccessContext.setError("Expected numerical expression as last item in " +
                                            pathNavigator.getCurrentPath());

            return false;
        }

        if (index >= getArrayLength()) {
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
bool CArrayParameter::setValues(size_t uiStartIndex, size_t offset, const string &strValue,
                                CParameterAccessContext &parameterAccessContext) const
{
    // Deal with value(s)
    Tokenizer tok(strValue, Tokenizer::defaultDelimiters + ",");

    std::vector<string> astrValues = tok.split();
    size_t nbValues = astrValues.size();

    // Check number of provided values
    if (nbValues + uiStartIndex > getArrayLength()) {

        // Out of bounds
        parameterAccessContext.setError("Too many values provided");

        return false;
    }

    // Process
    size_t valueIndex;
    size_t size = getSize();
    offset += uiStartIndex * size;

    for (valueIndex = 0; valueIndex < nbValues; valueIndex++) {

        if (!doSet(astrValues[valueIndex], offset, parameterAccessContext)) {

            // Append parameter path to error
            parameterAccessContext.appendToError(" " + getPath() + "/" +
                                                 std::to_string(valueIndex + uiStartIndex));

            return false;
        }

        offset += size;
    }
    return true;
}

// Common get value processing
string CArrayParameter::getValues(size_t offset,
                                  CParameterAccessContext &parameterAccessContext) const
{
    size_t size = getSize();
    size_t arrayLength = getArrayLength();

    string output;

    bool bFirst = true;

    for (size_t valueIndex = 0; valueIndex < arrayLength; valueIndex++) {
        string strReadValue;

        doGet(strReadValue, offset, parameterAccessContext);

        if (!bFirst) {

            output += " ";
        } else {

            bFirst = false;
        }

        output += strReadValue;

        offset += size;
    }

    return output;
}

// Generic Access
template <typename type>
bool CArrayParameter::accessValues(std::vector<type> &values, bool bSet,
                                   CParameterAccessContext &parameterAccessContext) const
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
bool CArrayParameter::setValues(const std::vector<type> &values,
                                CParameterAccessContext &parameterAccessContext) const
{
    size_t nbValues = getArrayLength();
    size_t size = getSize();
    size_t offset = getOffset();

    assert(values.size() == nbValues);

    // Process
    for (size_t valueIndex = 0; valueIndex < nbValues; valueIndex++) {

        if (!doSet(values[valueIndex], offset, parameterAccessContext)) {

            return false;
        }

        offset += size;
    }

    return true;
}

template <typename type>
bool CArrayParameter::getValues(std::vector<type> &values,
                                CParameterAccessContext &parameterAccessContext) const
{
    size_t nbValues = getArrayLength();
    size_t size = getSize();
    size_t offset = getOffset();

    values.clear();

    for (size_t valueIndex = 0; valueIndex < nbValues; valueIndex++) {
        type readValue;

        if (!doGet(readValue, offset, parameterAccessContext)) {

            return false;
        }

        values.push_back(readValue);

        offset += size;
    }
    return true;
}

template <typename type>
bool CArrayParameter::doSet(type value, size_t offset,
                            CParameterAccessContext &parameterAccessContext) const
{
    uint32_t uiData;

    if (!static_cast<const CParameterType *>(getTypeElement())
             ->toBlackboard(value, uiData, parameterAccessContext)) {

        return false;
    }
    // Write blackboard
    CParameterBlackboard *pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->writeInteger(&uiData, getSize(), offset);

    return true;
}

template <typename type>
bool CArrayParameter::doGet(type &value, size_t offset,
                            CParameterAccessContext &parameterAccessContext) const
{
    uint32_t uiData = 0;

    // Read blackboard
    const CParameterBlackboard *pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->readInteger(&uiData, getSize(), offset);

    return static_cast<const CParameterType *>(getTypeElement())
        ->fromBlackboard(value, uiData, parameterAccessContext);
}
