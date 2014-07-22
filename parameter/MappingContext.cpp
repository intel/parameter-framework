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
#include "MappingContext.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

using std::string;

CMappingContext::CMappingContext(size_t uiNbItemTypes) : _pstItemArray(new CMappingContext::SItem[uiNbItemTypes]), _uiNbItemTypes(uiNbItemTypes)
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
CMappingContext& CMappingContext::operator=(const CMappingContext& right)
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
bool CMappingContext::setItem(uint32_t uiItemType, const string* pStrKey, const string* pStrItem)
{
    size_t uiIndex;

    // Do some checks
    for (uiIndex = 0; uiIndex < _uiNbItemTypes; uiIndex++) {

        // Does key already exist ?
        assert(_pstItemArray[uiIndex].strKey != pStrKey);
    }

    if (_pstItemArray[uiItemType].bSet) {

        // Already set!
        return false;
    }

    // Set item key
    _pstItemArray[uiItemType].strKey = pStrKey;

    // Set item value
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

const string* CMappingContext::getItem(const string& strKey) const
{
    size_t uiItemType;

    for (uiItemType = 0; uiItemType < _uiNbItemTypes; uiItemType++) {

        if (_pstItemArray[uiItemType].strKey != NULL &&
            strKey == *_pstItemArray[uiItemType].strKey) {

            return _pstItemArray[uiItemType].strItem;
        }
    }

    return NULL;
}

bool CMappingContext::iSet(uint32_t uiItemType) const
{
    return _pstItemArray[uiItemType].bSet;
}
