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
#include "EnumParameterType.h"
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <assert.h>
#include "ParameterAccessContext.h"
#include "EnumValuePair.h"
#include "Utility.h"
#include <errno.h>

#define base CParameterType

using std::string;

CEnumParameterType::CEnumParameterType(const string& strName) : base(strName)
{
}

string CEnumParameterType::getKind() const
{
    return "EnumParameter";
}

bool CEnumParameterType::childrenAreDynamic() const
{
    return true;
}

// Element properties
void CEnumParameterType::showProperties(string& strResult) const
{
    base::showProperties(strResult);

    strResult += "Value Pairs:\n";

    // Show all value pairs
    size_t uiChild;
    size_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CEnumValuePair* pValuePair = static_cast<const CEnumValuePair*>(getChild(uiChild));

        strResult += "\tLiteral: \"";
        strResult += pValuePair->getName();
        strResult += "\", Numerical: ";
        strResult += pValuePair->getNumericalAsString();
        strResult += "\n";
    }
}

bool CEnumParameterType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Size in bits
    uint32_t uiSizeInBits = xmlElement.getAttributeInteger("Size");

    // Size
    setSize(uiSizeInBits / 8);

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// Conversion (tuning)
bool CEnumParameterType::toBlackboard(const string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    int64_t iData;

    if (isNumber(strValue)) {

        /// Numerical value provided

        // Hexa
        bool bValueProvidedAsHexa = !strValue.compare(0, 2, "0x");

        errno = 0;
        char *pcStrEnd;

        // Get value
        iData = strtoll(strValue.c_str(), &pcStrEnd, 0);

        // Conversion error when the input string does not contain any digit or the number is out of range (int32_t type)
        bool bConversionSucceeded = !errno && (strValue.c_str() != pcStrEnd);

        // Check validity against type
        if (!checkValueAgainstRange(strValue, iData, parameterAccessContext, bValueProvidedAsHexa, bConversionSucceeded)) {

            return false;
        }

        if (bValueProvidedAsHexa) {

            // Sign extend
            signExtend(iData);
        }

        // Check validity against lexical space
        string strError;
        if (!isValid(iData, parameterAccessContext)) {

            parameterAccessContext.setError(strError);

            return false;
        }
    } else {
        /// Literal value provided

        // Check validity against lexical space
        int iNumerical;
        if (!getNumerical(strValue, iNumerical)) {

            parameterAccessContext.setError("Provided value not part of lexical space");

            return false;
        }
        iData = iNumerical;

        // Check validity against type
        if (!checkValueAgainstRange(strValue, iData, parameterAccessContext, false, isEncodable((uint64_t)iData, true))) {

            return false;
        }
    }

    // Return data
    uiValue = (uint32_t)iData;

    return true;
}

// Range checking
bool CEnumParameterType::checkValueAgainstRange(const string& strValue, int64_t value, CParameterAccessContext& parameterAccessContext, bool bHexaValue, bool bConversionSucceeded) const
{
    // Enums are always signed, it means we have one less util bit
    int64_t maxValue = getMaxValue<uint64_t>();
    int64_t minValue = -maxValue - 1;

    if (!bConversionSucceeded || value < minValue || value > maxValue) {

	std::ostringstream strStream;

        strStream << "Value " << strValue << " standing out of admitted range [";

        if (bHexaValue) {

            // Format Min
            strStream << "0x" << std::hex << std::uppercase << std::setw(getSize()*2) << std::setfill('0') << makeEncodable(minValue);
            // Format Max
            strStream << ", 0x" << std::hex << std::uppercase << std::setw(getSize()*2) << std::setfill('0') << makeEncodable(maxValue);

        } else {

            strStream << minValue << ", " <<  maxValue;
        }

        strStream << "] for " << getKind();

        parameterAccessContext.setError(strStream.str());

        return false;
    }
    return true;
}

bool CEnumParameterType::fromBlackboard(string& strValue, const uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    // Take care of format
    if (parameterAccessContext.valueSpaceIsRaw()) {

        // Format
	std::ostringstream strStream;

        // Numerical format requested
        if (parameterAccessContext.outputRawFormatIsHex()) {

            // Hexa display with unecessary bits cleared out
            strStream << "0x" << std::hex << std::uppercase << std::setw(getSize()*2) << std::setfill('0') << makeEncodable(uiValue);

            strValue = strStream.str();
        } else {

            // Integer display
            int32_t iValue = uiValue;

            // Sign extend
            signExtend(iValue);

            strStream << iValue;

            strValue = strStream.str();
        }
    } else {

        // Integer display
        int32_t iValue = uiValue;

        // Sign extend
        signExtend(iValue);

        // Literal display requested (should succeed)
        getLiteral(iValue, strValue);
    }
    return true;
}

// Value access
bool CEnumParameterType::toBlackboard(int32_t iUserValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    if (!isValid(iUserValue, parameterAccessContext)) {

        return false;
    }
    uiValue = iUserValue;

    return true;
}

bool CEnumParameterType::fromBlackboard(int32_t& iUserValue, uint32_t uiValue, CParameterAccessContext& parameterAccessContext) const
{
    (void)parameterAccessContext;

    int32_t iValue = uiValue;

    // Sign extend
    signExtend(iValue);

    iUserValue = iValue;

    return true;
}

// Default value handling (simulation only)
uint32_t CEnumParameterType::getDefaultValue() const
{
    if (!getNbChildren()) {

        return 0;
    }

    // Return first available numerical
    return static_cast<const CEnumValuePair*>(getChild(0))->getNumerical();
}

// Check string is a number
bool CEnumParameterType::isNumber(const string& strValue)
{
    char cFirst = strValue[0];

    return isdigit(cFirst) || cFirst == '+' || cFirst == '-';
}

// Literal - numerical conversions
bool CEnumParameterType::getLiteral(int32_t iNumerical, string& strLiteral) const
{
    size_t uiChild;
    size_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CEnumValuePair* pValuePair = static_cast<const CEnumValuePair*>(getChild(uiChild));

        if (pValuePair->getNumerical() == iNumerical) {

            strLiteral = pValuePair->getName();

            return true;
        }
    }

    return false;
}

bool CEnumParameterType::getNumerical(const string& strLiteral, int& iNumerical) const
{
    size_t uiChild;
    size_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CEnumValuePair* pValuePair = static_cast<const CEnumValuePair*>(getChild(uiChild));

        if (pValuePair->getName() == strLiteral) {

            iNumerical = pValuePair->getNumerical();

            return true;
        }
    }

    return false;
}

// Numerical validity of the enum value
bool CEnumParameterType::isValid(int iNumerical, CParameterAccessContext& parameterAccessContext) const
{
    // Check that the value is part of the allowed values for this kind of enum
    size_t uiChild;
    size_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CEnumValuePair* pValuePair = static_cast<const CEnumValuePair*>(getChild(uiChild));

        if (pValuePair->getNumerical() == iNumerical) {

            return true;
        }
    }

    parameterAccessContext.setError("Provided value not part of numerical space");

    return false;
}
// From IXmlSource
void CEnumParameterType::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Size
    xmlElement.setAttributeString("Size", CUtility::toString(getSize() * 8));

    base::toXml(xmlElement, serializingContext);
}
