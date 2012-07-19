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
#include "MappingContext.h"
#include <string.h>
#include <stdlib.h>

CMappingContext::CMappingContext(uint32_t uiNbItemTypes) : _pstItemArray(new CMappingContext::SItem[uiNbItemTypes]), _uiNbItemTypes(uiNbItemTypes)
{
    // Clear items
    memset(_pstItemArray, 0, sizeof(*_pstItemArray) * uiNbItemTypes);
}

CMappingContext::~CMappingContext()
{
    delete [] _pstItemArray;
}

// Copy constructor
CMappingContext::CMappingContext(const CMappingContext& from) : _pstItemArray(new CMappingContext::SItem[from._uiNbItemTypes]), _uiNbItemTypes(from._uiNbItemTypes)
{
    // Copy content items
    memcpy(_pstItemArray, from._pstItemArray, sizeof(*_pstItemArray) * _uiNbItemTypes);
}

// Affectation
const CMappingContext& CMappingContext::operator=(const CMappingContext& right)
{
    if (&right != this) {

        // Size
        _uiNbItemTypes = right._uiNbItemTypes;

        // Content
        // Delete previous array
        delete [] _pstItemArray;

        // Reallocate it
        _pstItemArray = new CMappingContext::SItem[_uiNbItemTypes];

        // Copy content items
        memcpy(_pstItemArray, right._pstItemArray, sizeof(*_pstItemArray) * _uiNbItemTypes);
    }
    return *this;
}

// Item access
bool CMappingContext::setItem(uint32_t uiItemType, const string* pStrItem)
{
    // Do some checks
    if (_pstItemArray[uiItemType].bSet) {

        // Already set!
        return false;
    }
    // Get item value
    _pstItemArray[uiItemType].strItem = pStrItem;

    // Now is set
    _pstItemArray[uiItemType].bSet = true;

    return true;
}

const string&  CMappingContext::getItem(uint32_t uiItemType) const
{
    return *_pstItemArray[uiItemType].strItem;
}

uint32_t CMappingContext::getItemAsInteger(uint32_t uiItemType) const
{
    if (!_pstItemArray[uiItemType].strItem) {

        return 0;
    }

    return strtoul(_pstItemArray[uiItemType].strItem->c_str(), NULL, 0);
}

bool CMappingContext::iSet(uint32_t uiItemType) const
{
    return _pstItemArray[uiItemType].bSet;
}
