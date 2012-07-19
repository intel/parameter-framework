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
#include "BaseParameter.h"
#include "ParameterType.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "ParameterBlackboard.h"
#include <assert.h>

#define base CInstanceConfigurableElement

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

            // Append parameter path to error
            configurationAccessContext.appendToError(" " + getPath());

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

bool CBaseParameter::accessAsBooleanArray(vector<bool>& abValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
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

bool CBaseParameter::accessAsIntegerArray(vector<uint32_t>& auiValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
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

bool CBaseParameter::accessAsSignedIntegerArray(vector<int32_t>& aiValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
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

bool CBaseParameter::accessAsDoubleArray(vector<double>& adValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
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
        if (!doSetValue(strValue, getOffset(), parameterAccessContext)) {

            // Append parameter path to error
            parameterAccessContext.appendToError(" " + getPath());

            return false;
        }
        // Synchronize
        if (parameterAccessContext.getAutoSync() && !sync(parameterAccessContext)) {

            // Append parameter path to error
            parameterAccessContext.appendToError(" " + getPath());

            return false;
        }

    } else {
        // Get Value
        doGetValue(strValue, getOffset(), parameterAccessContext);
    }

    return true;
}

bool CBaseParameter::accessAsStringArray(vector<string>& astrValues, bool bSet, CParameterAccessContext& parameterAccessContext) const
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
