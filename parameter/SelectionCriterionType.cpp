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
#include "SelectionCriterionType.h"
#include "Tokenizer.h"

#define base CElement

const std::string CSelectionCriterionType::_strDelimiter = "|";

CSelectionCriterionType::CSelectionCriterionType(bool bIsInclusive) : _bInclusive(bIsInclusive)
{
    // For inclusive criterion type, appends the pair none,0 by default.
    if (_bInclusive) {

        _numToLitMap["none"] = 0;
    }
}

std::string CSelectionCriterionType::getKind() const
{
    return "SelectionCriterionType";
}

// From ISelectionCriterionTypeInterface
bool CSelectionCriterionType::addValuePair(int iValue, const std::string& strValue)
{
    // Check 1 bit set only for inclusive types
    if (_bInclusive && (!iValue || (iValue & (iValue - 1)))) {

        log_warning("Rejecting value pair association: 0x%X - %s for Selection Criterion Type %s", iValue, strValue.c_str(), getName().c_str());

        return false;
    }

    // Check already inserted
    if (_numToLitMap.find(strValue) != _numToLitMap.end()) {

        log_warning("Rejecting value pair association (literal already present): 0x%X - %s for Selection Criterion Type %s", iValue, strValue.c_str(), getName().c_str());

        return false;
    }
    _numToLitMap[strValue] = iValue;

    return true;
}

bool CSelectionCriterionType::getNumericalValue(const std::string& strValue, int& iValue) const
{
    if (_bInclusive) {

        Tokenizer tok(strValue, _strDelimiter);
        std::vector<std::string> astrValues = tok.split();
        size_t uiNbValues = astrValues.size();
        int iResult = 0;
        size_t uiValueIndex;
        iValue = 0;

        // Looping on each std::string delimited by "|" token and adding the associated value
        for (uiValueIndex = 0; uiValueIndex < uiNbValues; uiValueIndex++) {

            if (!getAtomicNumericalValue(astrValues[uiValueIndex], iResult)) {

                return false;
            }
            iValue |= iResult;
        }
        return true;
    }
    return getAtomicNumericalValue(strValue, iValue);
}

bool CSelectionCriterionType::getAtomicNumericalValue(const std::string& strValue, int& iValue) const
{
    NumToLitMapConstIt it = _numToLitMap.find(strValue);

    if (it != _numToLitMap.end()) {

        iValue = it->second;

        return true;
    }
    return false;
}

bool CSelectionCriterionType::getLiteralValue(int iValue, std::string& strValue) const
{
    NumToLitMapConstIt it;

    for (it = _numToLitMap.begin(); it != _numToLitMap.end(); ++it) {

        if (it->second == iValue) {

            strValue = it->first;

            return true;
        }
    }
    return false;
}

bool CSelectionCriterionType::isTypeInclusive() const
{
    return _bInclusive;
}

// Value list
std::string CSelectionCriterionType::listPossibleValues() const
{
    std::string strValueList = "{";

    // Get comma seprated list of values
    NumToLitMapConstIt it;
    bool bFirst = true;

    for (it = _numToLitMap.begin(); it != _numToLitMap.end(); ++it) {

        if (bFirst) {

            bFirst = false;
        } else {
            strValueList += ", ";
        }
        strValueList += it->first;
    }

    strValueList += "}";

    return strValueList;
}

// Formatted state
std::string CSelectionCriterionType::getFormattedState(int iValue) const
{
    std::string strFormattedState;

    if (_bInclusive) {

        // Need to go through all set bit
        uint32_t uiBit;
        bool bFirst = true;

        for (uiBit = 0; uiBit < sizeof(iValue) * 8; uiBit++) {

            int iSingleBitValue = iValue & (1 << uiBit);

            // Check if current bit is set
            if (!iSingleBitValue) {

                continue;
            }

            // Simple translation
            std::string strSingleValue;

            if (!getLiteralValue(iSingleBitValue, strSingleValue)) {
                // Numeric value not part supported values for this criterion type.
                continue;
            }

            if (bFirst) {

                bFirst = false;
            } else {
                strFormattedState += "|";
            }

            strFormattedState += strSingleValue;
        }

    } else {
        // Simple translation
        getLiteralValue(iValue, strFormattedState);
    }

    // Sometimes nothing is set
    if (strFormattedState.empty()) {

        strFormattedState = "<none>";
    }

    return strFormattedState;
}

// From IXmlSource
void CSelectionCriterionType::toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext) const
{
    // Type Kind
    xmlElement.setAttributeString("Kind", isTypeInclusive() ? "Inclusive" : "Exclusive");

    // Value pairs as children
    NumToLitMapConstIt it;

    for (it = _numToLitMap.begin(); it != _numToLitMap.end(); ++it) {

        CXmlElement childValuePairElement;

        xmlElement.createChild(childValuePairElement, "ValuePair");
        // Literal
        childValuePairElement.setAttributeString("Literal", it->first);
        // Numerical
        childValuePairElement.setAttributeSignedInteger("Numerical", it->second);
    }

    base::toXml(xmlElement, serializingContext);
}
