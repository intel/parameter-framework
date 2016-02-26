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
#pragma once

#include "ParameterType.h"

#include <string>

class CEnumParameterType : public CParameterType
{
public:
    CEnumParameterType(const std::string &strName);

    // From IXmlSink
    bool fromXml(const CXmlElement &xmlElement,
                 CXmlSerializingContext &serializingContext) override;

    // From IXmlSource
    void toXml(CXmlElement &xmlElement, CXmlSerializingContext &serializingContext) const override;

    /// Conversion
    // String
    bool toBlackboard(const std::string &strValue, uint32_t &uiValue,
                      CParameterAccessContext &parameterAccessContext) const override;
    bool fromBlackboard(std::string &strValue, const uint32_t &uiValue,
                        CParameterAccessContext &parameterAccessContext) const override;
    // Integer
    bool toBlackboard(int32_t iUserValue, uint32_t &uiValue,
                      CParameterAccessContext &parameterAccessContext) const override;
    bool fromBlackboard(int32_t &iUserValue, uint32_t uiValue,
                        CParameterAccessContext &parameterAccessContext) const override;

    // Default value handling (simulation only)
    uint32_t getDefaultValue() const override;

    // Element properties
    void showProperties(std::string &strResult) const override;

    // CElement
    std::string getKind() const override;

private:
    // Specialized version of toBlackboard in case the access context is in raw
    // value space
    bool toBlackboardFromRaw(const std::string &strUserValue, uint32_t &uiValue,
                             CParameterAccessContext &parameterAccessContext) const;

    // Returns true if children dynamic creation is to be dealt with
    bool childrenAreDynamic() const override;

    // Literal - numerical conversions
    bool getLiteral(int32_t iNumerical, std::string &strLiteral) const;
    bool getNumerical(const std::string &strLiteral, int &iNumerical) const;

    // Numerical validity
    bool checkValueAgainstSpace(int32_t iNumerical) const;

    int32_t getMin() const;
    int32_t getMax() const;
};
