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
#include "MappingContext.h"
#include <cassert>
#include <algorithm>
#include <cstdlib>

using std::string;

// Item access
bool CMappingContext::setItem(size_t itemType, const string *pStrKey, const string *pStrItem)
{
    if (iSet(itemType)) {
        // Already set!
        return false;
    }

    // Set item key
    mItems[itemType].strKey = pStrKey;

    // Set item value
    mItems[itemType].strItem = pStrItem;

    return true;
}

const string &CMappingContext::getItem(size_t itemType) const
{
    return *mItems[itemType].strItem;
}

size_t CMappingContext::getItemAsInteger(size_t itemType) const
{
    if (!mItems[itemType].strItem) {

        return 0;
    }

    return strtoul(mItems[itemType].strItem->c_str(), nullptr, 0);
}

const string *CMappingContext::getItem(const string &strKey) const
{
    auto itemFound = find_if(begin(mItems), end(mItems), [&](const SItem &item) {
        return item.strKey != nullptr && strKey == *item.strKey;
    });
    return (itemFound != end(mItems)) ? itemFound->strKey : nullptr;
}

bool CMappingContext::iSet(size_t itemType) const
{
    return mItems[itemType].strItem != nullptr;
}
