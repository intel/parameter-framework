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
#include "ElementLocator.h"
#include "PathNavigator.h"

CElementLocator::CElementLocator(CElement* pSubRootElement, bool bStrict) : _pSubRootElement(pSubRootElement), _bStrict(bStrict)
{
}

// Locate element
bool CElementLocator::locate(const string& strPath, CElement** ppElement, string& strError)
{
    CPathNavigator pathNavigator(strPath);

    if (!pathNavigator.isPathValid()) {

        strError = "Invalid Path";

        return false;
    }

    // Sub root element?
    string* pStrChildName = pathNavigator.next();

    if (!pStrChildName) {

        if (_bStrict) {

            strError = "Path not found: " + strPath;

            return false;
        }
        // Empty path
        return true;
    }

    if (*pStrChildName != _pSubRootElement->getName()) {

        strError = "Path not found: " + strPath;

        return false;
    }

    // Find in tree
    *ppElement = _pSubRootElement->findDescendant(pathNavigator);

    if (!*ppElement) {

        strError = "Path not found: " + strPath;

        return false;
    }

    return true;
}

