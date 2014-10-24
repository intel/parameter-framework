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

class CFixedPointParameterType : public CParameterType
{
public:
    CFixedPointParameterType(const std::string& strName);

    // From IXmlSink
    virtual bool fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext);

    // From IXmlSource
    virtual void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const;

    // XML Serialization value space handling
    // Value space handling for configuration import
    virtual void handleValueSpaceAttribute(CXmlElement& xmlConfigurableElementSettingsElement, CConfigurationAccessContext& configurationAccessContext) const;

    /// Conversion
    // String
    virtual bool toBlackboard(const std::string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    virtual bool fromBlackboard(std::string& strValue, const uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    // Double
    virtual bool toBlackboard(double dUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;
    virtual bool fromBlackboard(double& dUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const;

    // Element properties
    virtual void showProperties(std::string& strResult) const;

    // CElement
    virtual std::string getKind() const;
private:
    // Util size
    uint32_t getUtilSizeInBits() const;
    // Range computation
    void getRange(double& dMin, double& dMax) const;

    /**
     * Checks if a string has the written representation of an hexadecimal number (Which is
     * the prefix "Ox" in C++).
     *
     * @param[in] strValue Parameter read from the XML file representated as a string.
     *
     * @return true if the string is written as hexa, false otherwise.
     */
    bool isHexadecimal(const std::string& strValue) const;

    /**
     * Convert a decimal raw represented string into an unsigned long integer.
     * In case of a failing conversion or encodability, this function set the error to
     * illegal value provided and gives the range allowed for the parameter.
     *
     * @param[in] strValue Parameter read from the XML file representated as a string in decimal
     *                     raw format
     * @param[out] uiValue Parameter representated as a long unsigned integer.
     * @param[in:out] parameterAccessContext Parameter access context.
     *
     * @return true if the string was successfully converted, false otherwise.
     */
    bool convertFromDecimal(const std::string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;

    /**
     * Convert an hexadecimal raw represented string into an unsigned long integer.
     * In case of a failing conversion or encodability, this function set the error to
     * illegal value provided and gives the range allowed for the parameter.
     *
     * @param[in] strValue Parameter read from the XML file representated as a string in hexadecimal
     *                     raw format
     * @param[out] uiValue Parameter representated as a long unsigned integer.
     * @param[in:out] parameterAccessContext Parameter access context.
     *
     * @return true if the string was successfully converted, false otherwise.
     */
    bool convertFromHexadecimal(const std::string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;

    /**
     * Convert a Qn.m represented string into an unsigned long integer.
     * In case of a failing conversion or encodability, this function set the error to
     * illegal value provided and gives the range allowed for the parameter.
     *
     * @param[in] strValue Parameter read from the XML file representated as a string in Qn.m
     *                     representation.
     * @param[out] uiValue Parameter representated as a long unsigned integer.
     * @param[in:out] parameterAccessContext Parameter access context.
     *
     * @return true if the string was successfully converted, false otherwise.
     */
    bool convertFromQnm(const std::string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const;

    /**
     * Set the out of range error.
     * In case of a failing conversion or encodability, this function set the error to
     * illegal value provided and gives the range allowed for the parameter.
     *
     * @param[in] strValue Parameter read from the XML file representated as a string
     * @param[in:out] parameterAccessContext Parameter Access Context
     */
    void setOutOfRangeError(const std::string& strValue, CParameterAccessContext& parameterAccessContext) const;

    // Check if data is encodable
    bool checkValueAgainstRange(double dValue) const;

    /**
     * Convert a double towards a Qn.m representation which is stored in binary format.
     * This value is rounded if the double is not encodable in the corresponding Qn.m format.
     *
     * @param[in] dValue the double which should be converted.
     *
     * @return the integer which contains the converted Qn.m number.
     */
    int32_t doubleToBinaryQnm(double dValue) const;

    /**
     * Convert a Qn.m binary number towards its double representation.
     *
     * @param[in] iValue the integer which contains the Qn.m number which should be converted.
     *
     * @return the double which contains the double representation of iValue.
     */
    double binaryQnmToDouble(int32_t iValue) const;

    // Integral part in Q notation
    uint32_t _uiIntegral;
    // Fractional part in Q notation
    uint32_t _uiFractional;
};
