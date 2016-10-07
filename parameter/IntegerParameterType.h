/*
 * Copyright (c) 2011-2016, Intel Corporation
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

#include "BaseIntegerParameterType.h"
#include "ParameterAdaptation.h"
#include "ParameterAccessContext.h"

#include <convert.hpp>

#include <type_traits>
#include <sstream>
#include <string>
#include <limits>
#include <iomanip>

namespace detail
{
template <bool isSigned, size_t size>
struct IntegerTraits
{
    static_assert(size == 8 or size == 16 or size == 32,
                  "IntegerParameterType size must be 8, 16 or 32.");

private:
    // Assumes that size is either 8, 16 or 32, which is ensured by the static_assert above.
    using Signed = typename std::conditional<
        size == 8, int8_t, typename std::conditional<size == 16, int16_t, int32_t>::type>::type;
    using Unsigned = typename std::make_unsigned<Signed>::type;

public:
    using CType = typename std::conditional<isSigned, Signed, Unsigned>::type;
};
} // namespace detail

template <bool isSigned, size_t bitSize>
class CIntegerParameterType : public CBaseIntegerParameterType
{
private:
    using Base = CBaseIntegerParameterType;
    using CType = typename detail::IntegerTraits<isSigned, bitSize>::CType;

    template <class UserType>
    bool doToBlackboard(UserType userValue, uint32_t &uiValue,
                        CParameterAccessContext &parameterAccessContext) const
    {
        {
            if (userValue < static_cast<UserType>(_min) ||
                userValue > static_cast<UserType>(_max)) {

                parameterAccessContext.setError("Value out of range");
                return false;
            }
            // Do assign
            uiValue = userValue;

            return true;
        }
    }

public:
    CIntegerParameterType(const std::string &name) : Base(name){};

    // From IXmlSink
    bool fromXml(const CXmlElement &xmlElement, CXmlSerializingContext &serializingContext) override
    {
        setSize(bitSize / 8);

        xmlElement.getAttribute("Min", _min);
        xmlElement.getAttribute("Max", _max);

        if (_min > _max) {
            serializingContext.setError("The range of allowed value is empty (" +
                                        std::to_string(_min) + " > " + std::to_string(_max) + ").");
            return false;
        }

        // Base
        return Base::fromXml(xmlElement, serializingContext);
    }

    // From IXmlSource
    void toXml(CXmlElement &xmlElement, CXmlSerializingContext &serializingContext) const override
    {
        xmlElement.setAttribute("Signed", isSigned);

        xmlElement.setAttribute("Min", _min);
        xmlElement.setAttribute("Max", _max);

        xmlElement.setAttribute("Size", bitSize);

        Base::toXml(xmlElement, serializingContext);
    }

    bool fromBlackboard(std::string &strValue, const uint32_t &value,
                        CParameterAccessContext &parameterAccessContext) const override
    {
        // Format
        std::ostringstream stream;

        // Take care of format
        if (parameterAccessContext.valueSpaceIsRaw() &&
            parameterAccessContext.outputRawFormatIsHex()) {

            // Hexa display with unecessary bits cleared out
            stream << "0x" << std::hex << std::uppercase
                   << std::setw(static_cast<int>(getSize() * 2)) << std::setfill('0') << value;
        } else {

            if (isSigned) {

                int32_t iValue = value;

                // Sign extend
                signExtend(iValue);

                stream << iValue;
            } else {

                stream << value;
            }
        }

        strValue = stream.str();

        return true;
    }

    // Value access
    // Integer
    bool toBlackboard(uint32_t uiUserValue, uint32_t &uiValue,
                      CParameterAccessContext &parameterAccessContext) const override
    {
        return doToBlackboard(uiUserValue, uiValue, parameterAccessContext);
    }

    // Signed Integer
    bool toBlackboard(int32_t iUserValue, uint32_t &uiValue,
                      CParameterAccessContext &parameterAccessContext) const override
    {
        return doToBlackboard(iUserValue, uiValue, parameterAccessContext);
    }

    // Double
    bool toBlackboard(double dUserValue, uint32_t &uiValue,
                      CParameterAccessContext &parameterAccessContext) const override
    {
        // Check if there's an adaptation object available
        const CParameterAdaptation *pParameterAdaption = getParameterAdaptation();

        if (!pParameterAdaption) {

            // Reject request and let upper class handle the error
            return Base::toBlackboard(dUserValue, uiValue, parameterAccessContext);
        }

        // Do the conversion
        int64_t iConvertedValue = pParameterAdaption->fromUserValue(dUserValue);

        if (iConvertedValue < _min || iConvertedValue > _max) {

            parameterAccessContext.setError("Value out of range");

            return false;
        }

        // Do assign
        uiValue = (uint32_t)iConvertedValue;

        return true;
    }

    template <class T>
    bool toBlackboard(const std::string &strValue, uint32_t &uiValue,
                      CParameterAccessContext &parameterAccessContext) const
    {
        T intermediate;
        if (not convertTo(strValue, intermediate)) {
            std::string strError;
            strError = "Impossible to convert value " + strValue + " for " + getKind();

            parameterAccessContext.setError(strError);
            return false;
        }

        CType value = static_cast<CType>(intermediate);
        if (!checkValueAgainstRange(strValue, intermediate, parameterAccessContext,
                                    utility::isHexadecimal(strValue))) {
            return false;
        }
        uiValue = (uint32_t)value;

        return true;
    }

    // String
    bool toBlackboard(const std::string &strValue, uint32_t &uiValue,
                      CParameterAccessContext &parameterAccessContext) const override
    {
        if (isSigned and utility::isHexadecimal(strValue)) {
            using Intermediate = typename std::make_unsigned<CType>::type;

            return toBlackboard<Intermediate>(strValue, uiValue, parameterAccessContext);
        } else {
            return toBlackboard<CType>(strValue, uiValue, parameterAccessContext);
        }
    }

    bool fromBlackboard(double &dUserValue, uint32_t uiValue,
                        CParameterAccessContext &parameterAccessContext) const override
    {
        // Check if there's an adaptation object available
        const CParameterAdaptation *pParameterAdaption = getParameterAdaptation();

        if (!pParameterAdaption) {

            // Reject request and let upper class handle the error
            return Base::fromBlackboard(dUserValue, uiValue, parameterAccessContext);
        }

        int64_t iValueToConvert;

        // Deal with signed data
        if (isSigned) {

            int32_t iValue = uiValue;

            signExtend(iValue);

            iValueToConvert = iValue;
        } else {

            iValueToConvert = uiValue;
        }

        // Do the conversion
        dUserValue = pParameterAdaption->toUserValue(iValueToConvert);

        return true;
    }

    // Default value handling (simulation only)
    uint32_t getDefaultValue() const override { return _min; }

    // Element properties
    void showProperties(std::string &strResult) const override
    {
        Base::showProperties(strResult);

        std::ostringstream stream;
        stream << "Signed: " << (isSigned ? "yes" : "no") << "\n"
               << "Min: " << _min << "\n"
               << "Max: " << _max << "\n";

        strResult += stream.str();

        // Check if there's an adaptation object available
        const CParameterAdaptation *pParameterAdaption = getParameterAdaptation();

        if (pParameterAdaption) {

            // Display adaptation properties
            strResult += "Adaptation:\n";

            pParameterAdaption->showProperties(strResult);
        }
    }

    // Integer conversion
    int toPlainInteger(int iSizeOptimizedData) const override
    {
        if (isSigned) {

            signExtend(iSizeOptimizedData);
        }

        return Base::toPlainInteger(iSizeOptimizedData);
    }

    // Range checking
    bool checkValueAgainstRange(const std::string &strValue, CType value,
                                CParameterAccessContext &parameterAccessContext,
                                bool bHexaValue) const
    {
        if (value < _min || value > _max) {

            std::ostringstream stream;

            stream << "Value " << strValue << " standing out of admitted range [";

            if (bHexaValue) {

                stream << "0x" << std::hex << std::uppercase
                       << std::setw(static_cast<int>(getSize() * 2)) << std::setfill('0');
                // Format Min
                stream << _min;
                // Format Max
                stream << _max;

            } else {

                stream << _min << ", " << _max;
            }

            stream << "] for " << getKind();

            parameterAccessContext.setError(stream.str());

            return false;
        }
        return true;
    }

private:
    // Range
    CType _min{std::numeric_limits<CType>::min()};
    CType _max{std::numeric_limits<CType>::max()};
};
