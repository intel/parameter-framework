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
#include "ElementLibrarySet.h"
#include <assert.h>

CElementLibrarySet::CElementLibrarySet()
{
}

CElementLibrarySet::~CElementLibrarySet()
{
    uint32_t uiIndex;

    for (uiIndex = 0; uiIndex < _elementLibraryArray.size(); uiIndex++) {

        delete _elementLibraryArray[uiIndex];
    }
}

void CElementLibrarySet::addElementLibrary(CElementLibrary* pElementLibrary)
{
    _elementLibraryArray.push_back(pElementLibrary);
}

CElementLibrary* CElementLibrarySet::getElementLibrary(uint32_t uiIndex) const
{
    assert(uiIndex <= _elementLibraryArray.size());

    return _elementLibraryArray[uiIndex];
}
