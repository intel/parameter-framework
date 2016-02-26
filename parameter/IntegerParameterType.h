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

#include <string>
#include <limits>

class CParameterAdaptation;

class CIntegerParameterType : public CParameterType
{
public:
    CIntegerParameterType(const std::string &strName);

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
    bool toBlackboard(uint32_t uiUserValue, uint32_t &uiValue,
                      CParameterAccessContext &parameterAccessContext) const override;
    bool fromBlackboard(uint32_t &uiUserValue, uint32_t uiValue,
                        CParameterAccessContext &parameterAccessContext) const override;
    // Signed Integer
    bool toBlackboard(int32_t iUserValue, uint32_t &uiValue,
                      CParameterAccessContext &parameterAccessContext) const override;
    bool fromBlackboard(int32_t &iUserValue, uint32_t uiValue,
                        CParameterAccessContext &parameterAccessContext) const override;
    // Double
    bool toBlackboard(double dUserValue, uint32_t &uiValue,
                      CParameterAccessContext &parameterAccessContext) const override;
    bool fromBlackboard(double &dUserValue, uint32_t uiValue,
                        CParameterAccessContext &parameterAccessContext) const override;

    // Default value handling (simulation only)
    uint32_t getDefaultValue() const override;

    // Element properties
    void showProperties(std::string &strResult) const override;

    // Integer conversion
    int toPlainInteger(int iSizeOptimizedData) const override;

    // CElement
    std::string getKind() const override;

private:
    // Returns true if children dynamic creation is to be dealt with
    bool childrenAreDynamic() const override;

    // Conversion from std::string
    bool convertValueFromString(const std::string &strValue, int64_t &iData,
                                CParameterAccessContext &parameterAccessContext) const;

    // Range checking
    template <typename type>
    bool checkValueAgainstRange(const std::string &strValue, type value, type minValue,
                                type maxValue, CParameterAccessContext &parameterAccessContext,
                                bool bHexaValue) const;

    // Adaptation element retrieval
    const CParameterAdaptation *getParameterAdaptation() const;

    // Signing
    bool _bSigned{false};
    // Range
    uint32_t _uiMin{0};
    uint32_t _uiMax{std::numeric_limits<uint32_t>::max()};
};
