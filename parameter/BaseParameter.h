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
#pragma once

#include <stdint.h>
#include <vector>

#include "InstanceConfigurableElement.h"

class CParameterAccessContext;
class CConfigurationAccessContext;

class CBaseParameter : public CInstanceConfigurableElement
{
public:
    CBaseParameter(const std::string& strName, const CTypeElement* pTypeElement);

    // XML configuration settings parsing/composing
    virtual bool serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const;

    // Check element is a parameter
    virtual bool isParameter() const;

    /// Value access
    // Boolean access
    virtual bool accessAsBoolean(bool& bValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    virtual bool accessAsBooleanArray(std::vector<bool>& abValues, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // Integer Access
    virtual bool accessAsInteger(uint32_t& uiValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    virtual bool accessAsIntegerArray(std::vector<uint32_t>& auiValues, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // Signed Integer Access
    virtual bool accessAsSignedInteger(int32_t& iValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    virtual bool accessAsSignedIntegerArray(std::vector<int32_t>& aiValues, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // Double Access
    virtual bool accessAsDouble(double& dValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    virtual bool accessAsDoubleArray(std::vector<double>& adValues, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // String Access
    bool accessAsString(std::string& strValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    virtual bool accessAsStringArray(std::vector<std::string>& astrValues, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // From IXmlSource
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

protected:
    // Parameter Access
    virtual bool accessValue(CPathNavigator& pathNavigator, std::string& strValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    virtual void logValue(std::string& strValue, CErrorContext& errorContext) const;

    // Actual value access (to be implemented by derived)
    virtual bool doSetValue(const std::string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const = 0;
    virtual void doGetValue(std::string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const = 0;

    /**
     * Append the parameter path to the error.
     *
     * @param[in:out] parameterAccessContext Parameter Access Context object.
     */
    void appendParameterPathToError(CParameterAccessContext& parameterAccessContext) const;
};
