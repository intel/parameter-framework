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

#include "ParameterType.h"

#include <list>
#include <string>

class CEnumParameterType : public CParameterType
{
public:
    CEnumParameterType(const std::string& strName);

    // From IXmlSink
    virtual bool fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);

    // From IXmlSource
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

    /// Conversion
    // String
    virtual bool toBlackboard(const std::string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    virtual bool fromBlackboard(std::string& strValue, const uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    // Integer
    virtual bool toBlackboard(int32_t iUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    virtual bool fromBlackboard(int32_t& iUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const;

    // Default value handling (simulation only)
    virtual uint32_t getDefaultValue() const;

    // Element properties
    virtual void showProperties(std::string& strResult) const;

    // CElement
    virtual std::string getKind() const;
private:
    // Returns true if children dynamic creation is to be dealt with
    virtual bool childrenAreDynamic() const;
    // Check std::string is a number
    static bool isNumber(const std::string& strValue);

    // Literal - numerical conversions
    bool getLiteral(int32_t iNumerical, std::string& strLiteral) const;
    bool getNumerical(const std::string& strLiteral, int& iNumerical) const;

    // Numerical validity
    bool isValid(int iNumerical, CParameterAccessContext& parameterAccessContext) const;

    // Range validity
    bool checkValueAgainstRange(const std::string& strValue, int64_t value, CParameterAccessContext& parameterAccessContext, bool bHexaValue, bool bConversionSucceeded) const;
};
