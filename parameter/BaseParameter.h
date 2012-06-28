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
#pragma once

#include <stdint.h>
#include <vector>

#include "InstanceConfigurableElement.h"

class CParameterAccessContext;
class CConfigurationAccessContext;

class CBaseParameter : public CInstanceConfigurableElement
{
public:
    CBaseParameter(const string& strName, const CTypeElement* pTypeElement);

    // XML configuration settings parsing/composing
    virtual bool serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const;

    // Check element is a parameter
    virtual bool isParameter() const;

    /// Value access
    // Boolean access
    virtual bool accessAsBoolean(bool& bValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    virtual bool accessAsBooleanArray(vector<bool>& abValues, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // Integer Access
    virtual bool accessAsInteger(uint32_t& uiValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    virtual bool accessAsIntegerArray(vector<uint32_t>& auiValues, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // Signed Integer Access
    virtual bool accessAsSignedInteger(int32_t& iValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    virtual bool accessAsSignedIntegerArray(vector<int32_t>& aiValues, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // Double Access
    virtual bool accessAsDouble(double& dValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    virtual bool accessAsDoubleArray(vector<double>& adValues, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // String Access
    bool accessAsString(string& strValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    virtual bool accessAsStringArray(vector<string>& astrValues, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // From IXmlSource
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

protected:
    // Parameter Access
    virtual bool accessValue(CPathNavigator& pathNavigator, string& strValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    virtual void logValue(string& strValue, CErrorContext& errorContext) const;

    // Actual value access (to be implemented by derived)
    virtual bool doSetValue(const string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const = 0;
    virtual void doGetValue(string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const = 0;
};
