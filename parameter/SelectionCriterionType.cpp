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
std::string CSelectionCriterionType::getCriterionTypeName()
{
    return getName();
}

bool CSelectionCriterionType::addValuePair(int iValue, const string& strValue)
{
    if (_numToLitMap.find(strValue) != _numToLitMap.end()) {

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
