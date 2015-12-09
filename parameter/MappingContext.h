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
#pragma once

#include "parameter_export.h"

#include <stdint.h>
#include <string>
#include <vector>

class PARAMETER_EXPORT CMappingContext
{
private:
    // Item structure
    struct SItem
    {
        const std::string *strKey{nullptr};
        const std::string *strItem{nullptr};
    };

public:
    CMappingContext(size_t uiNbItemTypes) : mItems(uiNbItemTypes) {}

    // Item access
    /**
     * Set context mapping item key and value.
     *
     * @param[in] itemType Mapping item index.
     * @param[in] pStrKey Mapping item key name.
     * @param[in] pStrItem Mapping item value.
     *
     * @return False if already set, true else.
     */
    bool setItem(size_t itemType, const std::string *pStrKey, const std::string *pStrItem);
    const std::string &getItem(size_t itemType) const;
    size_t getItemAsInteger(size_t itemType) const;
    /**
     * Get mapping item value from its key name.
     *
     * @param[in] strKey Mapping item key name.
     *
     * @return Mapping item value pointer if found, NULL else.
     */
    const std::string *getItem(const std::string &strKey) const;
    bool iSet(size_t itemType) const;

private:
    size_t getNbItems() const { return mItems.size(); }

    using Items = std::vector<SItem>;
    Items mItems;
};
