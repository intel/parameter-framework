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
#include "SyncerSet.h"
#include "Syncer.h"

CSyncerSet::CSyncerSet()
{
}

const CSyncerSet& CSyncerSet::operator+=(ISyncer* pRightSyncer)
{
    _syncerSet.insert(pRightSyncer);

    return *this;
}

const CSyncerSet& CSyncerSet::operator+=(const CSyncerSet& rightSyncerSet)
{
    if (&rightSyncerSet != this) {

        _syncerSet.insert(rightSyncerSet._syncerSet.begin(), rightSyncerSet._syncerSet.end());
    }

    return *this;
}

void CSyncerSet::clear()
{
    _syncerSet.clear();
}

bool CSyncerSet::sync(CParameterBlackboard& parameterBlackboard, bool bBack, list<string>* plstrError) const
{
    bool bSuccess = true;

    string strError;

    // Propagate
    SyncerSetConstIterator it;

    for (it = _syncerSet.begin(); it != _syncerSet.end(); ++it) {

        ISyncer* pSyncer = *it;

        if (!pSyncer->sync(parameterBlackboard, bBack, strError)) {

            if (plstrError) {

                plstrError->push_back(strError);
            }
            bSuccess = false;
        }
    }
    return bSuccess;
}
