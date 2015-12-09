/*
 * Copyright (c) 2014-2015, Intel Corporation
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

#include "ParameterType.h"
#include <string>

class CFloatingPointParameterType : public CParameterType
{
public:
    CFloatingPointParameterType(const std::string &strName);

    virtual bool fromXml(const CXmlElement &xmlElement, CXmlSerializingContext &serializingContext);
    virtual void toXml(CXmlElement &xmlElement, CXmlSerializingContext &serializingContext) const;

    virtual void handleValueSpaceAttribute(
        CXmlElement &xmlConfigurableElementSettingsElement,
        CConfigurationAccessContext &configurationAccessContext) const;

    virtual bool toBlackboard(const std::string &strValue, uint32_t &uiValue,
                              CParameterAccessContext &parameterAccessContext) const;
    virtual bool fromBlackboard(std::string &strValue, const uint32_t &uiValue,
                                CParameterAccessContext &parameterAccessContext) const;
    virtual bool toBlackboard(double dUserValue, uint32_t &uiValue,
                              CParameterAccessContext &parameterAccessContext) const;
    virtual bool fromBlackboard(double &dUserValue, uint32_t uiValue,
                                CParameterAccessContext &parameterAccessContext) const;

    virtual void showProperties(std::string &strResult) const;

    virtual std::string getKind() const;

private:
    typedef CParameterType base;

    /**
     * Format an out of range error.
     *
     * @param[in] strValue the user provided value
     * @param[in,out] parameterAccessContext Parameter Access Context
     */
    void setOutOfRangeError(const std::string &strValue,
                            CParameterAccessContext &parameterAccessContext) const;

    /**
     * Check value validity against range.
     *
     * @param[in] fValue the user interpreted value
     * @return true if data is valid against range, false otherwise
     */
    bool checkValueAgainstRange(float fValue) const;
    /**
     * Check value validity against range.
     *
     * @param[in] dValue the user interpreted value (as double)
     * @return true if data is valid against range, false otherwise
     */
    bool checkValueAgainstRange(double dValue) const;

    /** Bounds */
    float _fMin{std::numeric_limits<float>::lowest()};
    float _fMax{std::numeric_limits<float>::max()};
};
