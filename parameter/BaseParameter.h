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
    CBaseParameter(const std::string &strName, const CTypeElement *pTypeElement);

    // XML configuration settings parsing/composing
    bool serializeXmlSettings(
        CXmlElement &xmlConfigurationSettingsElementContent,
        CConfigurationAccessContext &configurationAccessContext) const override;

    // Check element is a parameter
    bool isParameter() const override;

    // Boolean access
    virtual bool access(bool &bValue, bool bSet,
                        CParameterAccessContext &parameterAccessContext) const;
    virtual bool access(std::vector<bool> &abValues, bool bSet,
                        CParameterAccessContext &parameterAccessContext) const;

    // Integer Access
    virtual bool access(uint32_t &uiValue, bool bSet,
                        CParameterAccessContext &parameterAccessContext) const;
    virtual bool access(std::vector<uint32_t> &auiValues, bool bSet,
                        CParameterAccessContext &parameterAccessContext) const;

    // Signed Integer Access
    virtual bool access(int32_t &iValue, bool bSet,
                        CParameterAccessContext &parameterAccessContext) const;
    virtual bool access(std::vector<int32_t> &aiValues, bool bSet,
                        CParameterAccessContext &parameterAccessContext) const;

    // Double Access
    virtual bool access(double &dValue, bool bSet,
                        CParameterAccessContext &parameterAccessContext) const;
    virtual bool access(std::vector<double> &adValues, bool bSet,
                        CParameterAccessContext &parameterAccessContext) const;

    // String Access
    // This one is not virtual because it is very generic. You can think if it
    // as the client saying: "I don't care about the type, here's the value as
    // a string - convert it yourself". Then, string-to-anything and
    // anything-to-string methods are used to convert it into a suitable type.
    bool access(std::string &strValue, bool bSet,
                CParameterAccessContext &parameterAccessContext) const;
    virtual bool access(std::vector<std::string> &astrValues, bool bSet,
                        CParameterAccessContext &parameterAccessContext) const;

    void structureToXml(CXmlElement &xmlElement,
                        CXmlSerializingContext &serializingContext) const final;

protected:
    // Parameter Access
    bool accessValue(CPathNavigator &pathNavigator, std::string &strValue, bool bSet,
                     CParameterAccessContext &parameterAccessContext) const override;

    // Actual value access (to be implemented by derived)
    virtual bool doSetValue(const std::string &strValue, size_t offset,
                            CParameterAccessContext &parameterAccessContext) const = 0;
    virtual void doGetValue(std::string &strValue, size_t offset,
                            CParameterAccessContext &parameterAccessContext) const = 0;

    /**
     * Append the parameter path to the error.
     *
     * @param[in,out] parameterAccessContext Parameter Access Context object.
     */
    void appendParameterPathToError(CParameterAccessContext &parameterAccessContext) const;

private:
    std::string logValue(CParameterAccessContext &context) const override;
};
