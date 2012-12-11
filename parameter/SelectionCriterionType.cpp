/* 
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
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 */
#include "SelectionCriterionType.h"

#define base CElement

CSelectionCriterionType::CSelectionCriterionType(bool bIsInclusive) : _bInclusive(bIsInclusive)
{
}

string CSelectionCriterionType::getKind() const
{
    return "SelectionCriterionType";
}

// From ISelectionCriterionTypeInterface
bool CSelectionCriterionType::addValuePair(int iValue, const string& strValue)
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

bool CSelectionCriterionType::getNumericalValue(const string& strValue, int& iValue) const
{
    NumToLitMapConstIt it = _numToLitMap.find(strValue);

    if (it != _numToLitMap.end()) {

        iValue = it->second;

        return true;
    }
    return false;
}

bool CSelectionCriterionType::getLiteralValue(int iValue, string& strValue) const
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
string CSelectionCriterionType::listPossibleValues() const
{
    string strValueList = "{";

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
string CSelectionCriterionType::getFormattedState(int iValue) const
{
    string strFormattedState;

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
            string strSingleValue;

            getLiteralValue(iSingleBitValue, strSingleValue);

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
