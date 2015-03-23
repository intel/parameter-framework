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

#include "TypeElement.h"

#include <string>

class CParameterAccessContext;

class CBitParameterType : public CTypeElement
{
public:
    CBitParameterType(const std::string& strName);

    // From IXmlSink
    virtual bool fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);

    // From IXmlSource
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;
    /// Conversion
    // String
    bool toBlackboard(const std::string& strValue, uint64_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    void fromBlackboard(std::string& strValue, const uint64_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    // Integer
    bool toBlackboard(uint64_t uiUserValue, uint64_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    void fromBlackboard(uint32_t& uiUserValue, uint64_t uiValue, CParameterAccessContext& parameterAccessContext) const;
    // Access from area configuration
    uint64_t merge(uint64_t uiOriginData, uint64_t uiNewData) const;

    // Bit Size
    uint32_t getBitSize() const;

    // Element properties
    virtual void showProperties(std::string& strResult) const;

    // CElement
    virtual std::string getKind() const;

    /**
     * Get the position of the bit within the bit parameter block.
     * It corresponds to the "pos" attribute found in the XML file.
     *
     * @return position of the bit.
     */
    uint32_t getBitPos() const { return _uiBitPos; }
private:
    // Instantiation
    virtual CInstanceConfigurableElement* doInstantiate() const;
    // Max encodable value
    uint64_t getMaxEncodableValue() const;
    // Biwise mask
    uint64_t getMask() const;
    // Check data has no bit set outside available range
    bool isEncodable(uint64_t uiData) const;

    // Pos in bits
    uint32_t _uiBitPos;
    // Size in bits
    uint32_t _uiBitSize;
    // Max value
    uint64_t _uiMax;
};
