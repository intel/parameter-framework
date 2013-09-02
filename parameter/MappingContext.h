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
#pragma once

#include <stdint.h>
#include <string>

using namespace std;

class CMappingContext
{
    // Item structure
    struct SItem {
        const string* strKey;
        const string* strItem;
        bool bSet;
    };

public:
    // Regular Constructor
    CMappingContext(uint32_t uiNbItemTypes);
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
    bool setItem(uint32_t uiItemType, const string* pStrKey, const string* pStrItem);
    const string& getItem(uint32_t uiItemType) const;
    uint32_t getItemAsInteger(uint32_t uiItemType) const;
    /**
     * Get mapping item value from its key name.
     *
     * @param[in] strKey Mapping item key name.
     *
     * @return Mapping item value pointer if found, NULL else.
     */
    const string* getItem(const string& strKey) const;
    bool iSet(uint32_t uiItemType) const;

private:
    // Item array
    SItem* _pstItemArray;
    // Items array size
    uint32_t _uiNbItemTypes;
};

