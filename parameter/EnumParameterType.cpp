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

#define base CParameterType

CEnumParameterType::CEnumParameterType(const string& strName) : base(strName)
{
}

string CEnumParameterType::getKind() const
{
    return "EnumParameter";
}

// Element properties
void CEnumParameterType::showProperties(string& strResult) const
{
    base::showProperties(strResult);

    strResult += "Value Pairs:\n";

    // Show all value pairs
    ValuePairListIterator it;

    for (it = _valuePairList.begin(); it != _valuePairList.end(); ++it) {

        strResult += "\tLiteral: \"";
        strResult += it->_strLiteral;
        strResult += "\", Numerical: ";
        strResult += toString(it->_iNumerical);
        strResult += "\n";
    }
}

bool CEnumParameterType::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // Size in bits
    uint32_t uiSizeInBits = xmlElement.getAttributeInteger("Size");

    // Size
    setSize(uiSizeInBits / 8);

    // Get value pairs
    CXmlElement::CChildIterator it(xmlElement);

    CXmlElement xmlValuePairElement;

    while (it.next(xmlValuePairElement)) {

        _valuePairList.push_back(SValuePair(xmlValuePairElement.getAttributeString("Literal"), xmlValuePairElement.getAttributeSignedInteger("Numerical")));
    }

    // Check value pair list
    if (_valuePairList.empty()) {

        serializingContext.setError("No Value pairs provided for element " + xmlElement.getPath());

        return false;
    }

    // Don't dig
    return true;
}

bool CEnumParameterType::asInteger(const string& strValue, uint32_t& uiValue, CParameterAccessContext& parameterAccessContext) const
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

void CEnumParameterType::asString(const uint32_t& uiValue, string& strValue, CParameterAccessContext& parameterAccessContext) const
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
}

// Default value handling (simulation only)
uint32_t CEnumParameterType::getDefaultValue() const
{
    assert(!_valuePairList.empty());

    // Return first item
    return _valuePairList.front()._iNumerical;
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
    ValuePairListIterator it;

    for (it = _valuePairList.begin(); it != _valuePairList.end(); ++it) {

        if (it->_iNumerical == iNumerical) {

            strLiteral = it->_strLiteral;

            return true;
        }
    }
    return false;
}

bool CEnumParameterType::getNumerical(const string& strLiteral, int32_t& iNumerical) const
{
    ValuePairListIterator it;

    for (it = _valuePairList.begin(); it != _valuePairList.end(); ++it) {

        if (it->_strLiteral == strLiteral) {

            iNumerical = it->_iNumerical;

            return true;
        }
    }
    return false;
}

// Numerical validity
bool CEnumParameterType::isValid(int32_t iNumerical) const
{
    ValuePairListIterator it;

    for (it = _valuePairList.begin(); it != _valuePairList.end(); ++it) {

        if (it->_iNumerical == iNumerical) {

            return true;
        }
    }
    return false;
}
