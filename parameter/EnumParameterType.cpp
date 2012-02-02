/* <auto_header>
 * <FILENAME>
 *
 * INTEL CONFIDENTIAL
 * Copyright © 2011 Intel
 * Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel’s prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 *
 *  AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 *
 *
 * </auto_header>
 */
#include "EnumParameterType.h"
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <assert.h>
#include "ParameterAccessContext.h"
#include "EnumValuePair.h"

#define base CParameterType

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
    uint32_t uiChild;
    uint32_t uiNbChildren = getNbChildren();

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
    int32_t iData;

    if (isNumber(strValue)) {

        // Numerical value provided

        // Hexa
        bool bValueProvidedAsHexa = !strValue.compare(0, 2, "0x");

        // Get value
        iData = strtol(strValue.c_str(), NULL, 0);

        if (bValueProvidedAsHexa && isEncodable(iData)) {

            // Sign extend
            signExtend(iData);
        }
        // Check validity
        if (!isValid(iData)) {

            parameterAccessContext.setError("Provided value not part of numerical space");

            return false;
        }
    } else {
        // Literal value provided

        // Check validity
        if (!getNumerical(strValue, iData)) {

            parameterAccessContext.setError("Provided value not part of lexical space");

            return false;
        }
    }

    // Return data
    uiValue = iData;

    return true;
}

bool CEnumParameterType::fromBlackboard(string& strValue, const uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
{
    // Take care of format
    if (parameterAccessContext.valueSpaceIsRaw()) {

        // Format
        ostringstream strStream;

        // Numerical format requested
        if (parameterAccessContext.outputRawFormatIsHex()) {

            // Hexa display with unecessary bits cleared out
            strStream << "0x" << hex << uppercase << setw(getSize()*2) << setfill('0') << makeEncodable(uiValue);

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
    if (!isValid(iUserValue)) {

        parameterAccessContext.setError("Invalid value");

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
    uint32_t uiChild;
    uint32_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CEnumValuePair* pValuePair = static_cast<const CEnumValuePair*>(getChild(uiChild));

        if (pValuePair->getNumerical() == iNumerical) {

            strLiteral = pValuePair->getName();

            return true;
        }
    }

    return false;
}

bool CEnumParameterType::getNumerical(const string& strLiteral, int32_t& iNumerical) const
{
    uint32_t uiChild;
    uint32_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CEnumValuePair* pValuePair = static_cast<const CEnumValuePair*>(getChild(uiChild));

        if (pValuePair->getName() == strLiteral) {

            iNumerical = pValuePair->getNumerical();

            return true;
        }
    }

    return false;
}

// Numerical validity
bool CEnumParameterType::isValid(int32_t iNumerical) const
{
    uint32_t uiChild;
    uint32_t uiNbChildren = getNbChildren();

    for (uiChild = 0; uiChild < uiNbChildren; uiChild++) {

        const CEnumValuePair* pValuePair = static_cast<const CEnumValuePair*>(getChild(uiChild));

        if (pValuePair->getNumerical() == iNumerical) {

            return true;
        }
    }

    return false;
}
