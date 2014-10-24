/*
 * Copyright (c) 2011-2014, Intel Corporation
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
#include "BaseParameter.h"
#include "ParameterType.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "ParameterBlackboard.h"
#include <assert.h>

#define base CInstanceConfigurableElement

using std::string;

CBaseParameter::CBaseParameter(const string& strName, const CTypeElement* pTypeElement) : base(strName, pTypeElement)
{
}

// XML configuration settings parsing/composing
bool CBaseParameter::serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const
{
    // Handle access
    if (!configurationAccessContext.serializeOut()) {

        // Write to blackboard
        if (!doSetValue(xmlConfigurationSettingsElementContent.getTextContent(), getOffset() - configurationAccessContext.getBaseOffset(), configurationAccessContext)) {

            appendParameterPathToError(configurationAccessContext);
            return false;
        }
    } else {

        // Get string value
        string strValue;

        doGetValue(strValue, getOffset() - configurationAccessContext.getBaseOffset(), configurationAccessContext);

        // Populate value into xml text node
        xmlConfigurationSettingsElementContent.setTextContent(strValue);
    }

    // Done
    return true;
}

// Dump
void CBaseParameter::logValue(string& strValue, CErrorContext& errorContext) const
{
    // Parameter context
    CParameterAccessContext& parameterAccessContext = static_cast<CParameterAccessContext&>(errorContext);

    // Dump value
    doGetValue(strValue, getOffset(), parameterAccessContext);
}

// Check element is a parameter
bool CBaseParameter::isParameter() const
{
    return true;
}

/// Value access
// Boolean access
bool CBaseParameter::accessAsBoolean(bool& bValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    (void)bValue;
    (void)bSet;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

bool CBaseParameter::accessAsBooleanArray(std::vector<bool>& abValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    (void)abValues;
    (void)bSet;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

// Integer Access
bool CBaseParameter::accessAsInteger(uint32_t& uiValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    (void)uiValue;
    (void)bSet;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

bool CBaseParameter::accessAsIntegerArray(std::vector<uint32_t>& auiValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    (void)auiValues;
    (void)bSet;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

// Signed Integer Access
bool CBaseParameter::accessAsSignedInteger(int32_t& iValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    (void)iValue;
    (void)bSet;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

bool CBaseParameter::accessAsSignedIntegerArray(std::vector<int32_t>& aiValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    (void)aiValues;
    (void)bSet;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

// Double Access
bool CBaseParameter::accessAsDouble(double& dValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    (void)dValue;
    (void)bSet;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

bool CBaseParameter::accessAsDoubleArray(std::vector<double>& adValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    (void)adValues;
    (void)bSet;

    parameterAccessContext.setError("Unsupported conversion");

    return false;
}

// String Access
bool CBaseParameter::accessAsString(string& strValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    if (bSet) {

        // Set Value
        if (!doSetValue(strValue, getOffset() - parameterAccessContext.getBaseOffset(), parameterAccessContext)) {

            appendParameterPathToError(parameterAccessContext);
            return false;
        }
        // Synchronize
        if (!sync(parameterAccessContext)) {

            appendParameterPathToError(parameterAccessContext);
            return false;
        }

    } else {
        // Get Value
        doGetValue(strValue, getOffset() - parameterAccessContext.getBaseOffset(), parameterAccessContext);
    }

    return true;
}

bool CBaseParameter::accessAsStringArray(std::vector<string>& astrValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    (void)astrValues;
    (void)bSet;
    (void)parameterAccessContext;

    // Generic string array access to scalar parameter must have been filtered out before
    assert(0);

    return false;
}

// Parameter Access
bool CBaseParameter::accessValue(CPathNavigator& pathNavigator, string& strValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    // Check path validity
    if (!checkPathExhausted(pathNavigator, parameterAccessContext)) {

        return false;
    }

    return accessAsString(strValue, bSet, parameterAccessContext);
}

void CBaseParameter::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{

    // Delegate to type element
    getTypeElement()->toXml(xmlElement, serializingContext);
}


void CBaseParameter::appendParameterPathToError(CParameterAccessContext& parameterAccessContext)
const
{
    parameterAccessContext.appendToError(" " + getPath());
}
