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

bool CSyncerSet::sync(CParameterBlackboard& parameterBlackboard, bool bBack, std::list<std::string>* plstrError) const
{
    bool bSuccess = true;

    std::string strError;

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
