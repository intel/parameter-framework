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
#include <limits>

#include "TypeElement.h"

#include <string>

class CParameterAccessContext;
class CConfigurationAccessContext;

class CParameterType : public CTypeElement
{
public:
    CParameterType(const std::string& strName);
    virtual ~CParameterType();

    // Size
    uint32_t getSize() const;

    // Unit
    std::string getUnit() const;
    void setUnit(const std::string& strUnit);

    // From IXmlSink
    virtual bool fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);

    // From IXmlSource
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

    /// Conversions
    // String
    virtual bool toBlackboard(const std::string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const = 0;
    virtual bool fromBlackboard(std::string& strValue, const uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const = 0;
    // Boolean
    virtual bool toBlackboard(bool bUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    virtual bool fromBlackboard(bool& bUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const;
    // Integer
    virtual bool toBlackboard(uint32_t uiUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    virtual bool fromBlackboard(uint32_t& uiUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const;
    // Signed Integer
    virtual bool toBlackboard(int32_t iUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    virtual bool fromBlackboard(int32_t& iUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const;
    // Double
    virtual bool toBlackboard(double dUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    virtual bool fromBlackboard(double& dUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const;

    // XML Serialization value space handling
    // Value space handling for configuration import/export
    virtual void handleValueSpaceAttribute(CXmlElement& xmlConfigurableElementSettingsElement, CConfigurationAccessContext& configurationAccessContext) const;

    // Element properties
    virtual void showProperties(std::string& strResult) const;

    // Default value handling (simulation only)
    virtual uint32_t getDefaultValue() const;

    /**
     * Sign extension (32 bits)
     *
     * @param[in:out] iData the data which will be sign extended
     */
    void signExtend(int32_t& iData) const;

    /**
     * Sign extension (64 bits)
     *
     * @param[in:out] iData the data which will be sign extended
     */
    void signExtend(int64_t& iData) const;

protected:
    // Object creation
    virtual void populate(CElement* pElement) const;
    // Size
    void setSize(uint32_t uiSize);

    // Check data has no bit set outside available range (based on byte size) and
    // check data is consistent with available range, with respect to its sign
    bool isEncodable(uint32_t uiData, bool bIsSigned) const;
    bool isEncodable(uint64_t uiData, bool bIsSigned) const;
    // Remove all bits set outside available range
    uint32_t makeEncodable(uint32_t uiData) const;

    /** Compute max value according to the parameter type */
    template <typename type>
    type getMaxValue() const
    {
        return getSize() < sizeof(type) ?
                    (static_cast<type>(1) << (getSize() * std::numeric_limits<unsigned char>::digits - 1)) - 1 :
                    std::numeric_limits<type>::max();
    }

private:
    void setXmlUnitAttribute(CXmlElement& xmlElement) const;

    // Instantiation
    virtual CInstanceConfigurableElement* doInstantiate() const;
    // Generic Access
    template <typename type>
    void doSignExtend(type& data) const;
    template <typename type>
    bool doIsEncodable(type data, bool bIsSigned) const;

    // Size in bytes
    uint32_t _uiSize;
    // Unit
    std::string _strUnit;

    static const std::string gUnitPropertyName;
};
