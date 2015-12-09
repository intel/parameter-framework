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
#include "EnumValuePair.h"
#include "ParameterAccessContext.h"
#include "convert.hpp"

#define base CParameterType

using std::string;

CEnumParameterType::CEnumParameterType(const string &strName) : base(strName)
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
void CEnumParameterType::showProperties(string &strResult) const
{
    base::showProperties(strResult);

    strResult += "Value Pairs:\n";

    // Show all value pairs
    size_t uiChild;
    size_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CEnumValuePair *pValuePair = static_cast<const CEnumValuePair *>(getChild(uiChild));

        strResult += "\tLiteral: \"";
        strResult += pValuePair->getName();
        strResult += "\", Numerical: ";
        strResult += pValuePair->getNumericalAsString();
        strResult += "\n";
    }
}

bool CEnumParameterType::fromXml(const CXmlElement &xmlElement,
                                 CXmlSerializingContext &serializingContext)
{
    // Size in bits
    size_t sizeInBits = 0;
    if (not xmlElement.getAttribute("Size", sizeInBits)) {
        return false;
    }

    // Size
    setSize(sizeInBits / 8);

    // Base
    return base::fromXml(xmlElement, serializingContext);
}

// Conversion (tuning)
bool CEnumParameterType::toBlackboard(const string &strValue, uint32_t &uiValue,
                                      CParameterAccessContext &parameterAccessContext) const
{
    int32_t iParsedUserValue = 0;

    // Try to read the user-provided string as an integer
    if (not convertTo(strValue, iParsedUserValue)) {
        // If it fails to parse as an integer, first try to convert it from
        // lexical to numerical space.
        int32_t iNumerical;
        if (not getNumerical(strValue, iNumerical)) {

            parameterAccessContext.setError("Provided value '" + strValue +
                                            "' is not part of the lexical space"
                                            " or not within the numerical range.");

            return false;
        }
        iParsedUserValue = iNumerical;
    }

    // Once it has been converted to a number (either through parsing or
    // through lexical->numerical conversion), call the numerical overload of
    // toBlackboard.
    return toBlackboard(iParsedUserValue, uiValue, parameterAccessContext);
}

int32_t CEnumParameterType::getMin() const
{
    // Enums are always signed, it means we have one less util bit
    return -getMax() - 1;
}

int32_t CEnumParameterType::getMax() const
{
    return getMaxValue<int32_t>();
}

bool CEnumParameterType::fromBlackboard(string &userValue, const uint32_t &value,
                                        CParameterAccessContext & /*ctx*/) const
{
    // Convert the raw value from the blackboard
    int32_t signedValue = static_cast<int32_t>(value);
    signExtend(signedValue);

    // Convert from numerical space to literal space
    return getLiteral(signedValue, userValue);
}

// Value access
bool CEnumParameterType::toBlackboard(int32_t userValue, uint32_t &value,
                                      CParameterAccessContext &parameterAccessContext) const
{
    if (!checkValueAgainstSpace(userValue)) {

        parameterAccessContext.setError(std::to_string(userValue) +
                                        " is not part of numerical space.");

        return false;
    }

    if (userValue < getMin() or userValue > getMax()) {

        // FIXME: values provided as hexa (either on command line or in a config
        // file will appear in decimal base instead of hexa base...
        parameterAccessContext.setError(
            "Value " + std::to_string(userValue) + " standing out of admitted range [" +
            std::to_string(getMin()) + ", " + std::to_string(getMax()) + "] for " + getKind());
        return false;
    }

    value = static_cast<uint32_t>(userValue);

    return true;
}

bool CEnumParameterType::fromBlackboard(int32_t &userValue, uint32_t value,
                                        CParameterAccessContext & /*ctx*/) const
{
    int32_t signedValue = static_cast<int32_t>(value);

    // Sign extend
    signExtend(signedValue);

    userValue = signedValue;

    return true;
}

// Default value handling (simulation only)
uint32_t CEnumParameterType::getDefaultValue() const
{
    if (!getNbChildren()) {

        return 0;
    }

    // Return first available numerical
    return static_cast<const CEnumValuePair *>(getChild(0))->getNumerical();
}

// Literal - numerical conversions
bool CEnumParameterType::getLiteral(int32_t iNumerical, string &strLiteral) const
{
    size_t uiChild;
    size_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CEnumValuePair *pValuePair = static_cast<const CEnumValuePair *>(getChild(uiChild));

        if (pValuePair->getNumerical() == iNumerical) {

            strLiteral = pValuePair->getName();

            return true;
        }
    }

    return false;
}

bool CEnumParameterType::getNumerical(const string &strLiteral, int32_t &iNumerical) const
{
    size_t uiChild;
    size_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CEnumValuePair *pValuePair = static_cast<const CEnumValuePair *>(getChild(uiChild));

        if (pValuePair->getName() == strLiteral) {

            iNumerical = pValuePair->getNumerical();

            return true;
        }
    }

    return false;
}

// Numerical validity of the enum value
bool CEnumParameterType::checkValueAgainstSpace(int32_t iNumerical) const
{
    // Check that the value is part of the allowed values for this kind of enum
    size_t uiChild;
    size_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CEnumValuePair *pValuePair = static_cast<const CEnumValuePair *>(getChild(uiChild));

        if (pValuePair->getNumerical() == iNumerical) {

            return true;
        }
    }

    return false;
}

// From IXmlSource
void CEnumParameterType::toXml(CXmlElement &xmlElement,
                               CXmlSerializingContext &serializingContext) const
{
    // Size
    xmlElement.setAttribute("Size", getSize() * 8);

    base::toXml(xmlElement, serializingContext);
}
