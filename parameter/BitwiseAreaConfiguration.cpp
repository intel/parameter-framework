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
#include "BitwiseAreaConfiguration.h"
#include "BitParameter.h"
#include "Subsystem.h"

#define base CAreaConfiguration

CBitwiseAreaConfiguration::CBitwiseAreaConfiguration(const CConfigurableElement *pConfigurableElement, const CSyncerSet *pSyncerSet)
    : base(pConfigurableElement, pSyncerSet, static_cast<const CBitParameter*>(pConfigurableElement)->getBelongingBlockSize()),
      _bBigEndian(pConfigurableElement->getBelongingSubsystem()->isBigEndian())
{
}

// Blackboard copies
void CBitwiseAreaConfiguration::copyTo(CParameterBlackboard* pToBlackboard, uint32_t uiOffset) const
{
    // Beware this code works on little endian architectures only!
    const CBitParameter* pBitParameter = static_cast<const CBitParameter*>(_pConfigurableElement);

    uint64_t uiSrcData = 0;
    uint64_t uiDstData = 0;

    /// Read/modify/write

    // Read dst blackboard
    pToBlackboard->readInteger(&uiDstData, pBitParameter->getBelongingBlockSize(), uiOffset, _bBigEndian);

    // Read src blackboard
    _blackboard.readInteger(&uiSrcData, pBitParameter->getBelongingBlockSize(), 0, _bBigEndian);

    // Convert
    uiDstData = pBitParameter->merge(uiDstData, uiSrcData);

    // Write dst blackboard
    pToBlackboard->writeInteger(&uiDstData, pBitParameter->getBelongingBlockSize(), uiOffset, _bBigEndian);
}

void CBitwiseAreaConfiguration::copyFrom(const CParameterBlackboard* pFromBlackboard, uint32_t uiOffset)
{
    // Beware this code works on little endian architectures only!
    const CBitParameter* pBitParameter = static_cast<const CBitParameter*>(_pConfigurableElement);

    uint64_t uiSrcData = 0;
    uint64_t uiDstData = 0;

    /// Read/modify/write

    // Read dst blackboard
    _blackboard.readInteger(&uiDstData, pBitParameter->getBelongingBlockSize(), 0, _bBigEndian);

    // Read src blackboard
    pFromBlackboard->readInteger(&uiSrcData, pBitParameter->getBelongingBlockSize(), uiOffset, _bBigEndian);

    // Convert
    uiDstData = pBitParameter->merge(uiDstData, uiSrcData);

    // Write dst blackboard
    _blackboard.writeInteger(&uiDstData, pBitParameter->getBelongingBlockSize(), 0, _bBigEndian);
}

