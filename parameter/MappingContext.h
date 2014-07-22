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
#pragma once

#include <stdint.h>
#include <string>

class CMappingContext
{
    // Item structure
    struct SItem {
        const std::string* strKey;
        const std::string* strItem;
        bool bSet;
    };

public:
    // Regular Constructor
    CMappingContext(size_t uiNbItemTypes);
    ~CMappingContext();

    // Copy constructor
    CMappingContext(const CMappingContext& from);

    // Affectation
    CMappingContext& operator=(const CMappingContext& right);

    // Item access
    /**
     * Set context mapping item key and value.
     *
     * @param[in] uiItemType Mapping item index.
     * @param[in] pStrKey Mapping item key name.
     * @param[in] pStrItem Mapping item value.
     *
     * @return False if already set, true else.
     */
    bool setItem(uint32_t uiItemType, const std::string* pStrKey, const std::string* pStrItem);
    const std::string& getItem(uint32_t uiItemType) const;
    uint32_t getItemAsInteger(uint32_t uiItemType) const;
    /**
     * Get mapping item value from its key name.
     *
     * @param[in] strKey Mapping item key name.
     *
     * @return Mapping item value pointer if found, NULL else.
     */
    const std::string* getItem(const std::string& strKey) const;
    bool iSet(uint32_t uiItemType) const;

private:
    // Item array
    SItem* _pstItemArray;
    // Items array size
    size_t _uiNbItemTypes;
};

