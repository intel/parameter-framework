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
#include "ParameterBlackboard.h"
#include <cassert>
#include <algorithm>


// Size
void CParameterBlackboard::setSize(uint32_t uiSize)
{
    mBlackboard.resize(uiSize);
}

uint32_t CParameterBlackboard::getSize() const
{
    return mBlackboard.size();
}

// Single parameter access
void CParameterBlackboard::writeInteger(const void* pvSrcData, uint32_t uiSize, uint32_t uiOffset, bool bBigEndian)
{
    assert(uiSize + uiOffset <= getSize());

    auto first = static_cast<const uint8_t *>(pvSrcData);
    auto last = first + uiSize;
    auto dest_first = atOffset(uiOffset);

    if (!bBigEndian) {
        std::copy(first, last, dest_first);
    } else {
        std::reverse_copy(first, last, dest_first);
    }
}

void CParameterBlackboard::readInteger(void* pvDstData, uint32_t uiSize, uint32_t uiOffset, bool bBigEndian) const
{
    assert(uiSize + uiOffset <= getSize());

    auto first = atOffset(uiOffset);
    auto last = first + uiSize;
    auto dest_first = static_cast<uint8_t *>(pvDstData);

    if (!bBigEndian) {
        std::copy(first, last, dest_first);
    } else {
        std::reverse_copy(first, last, dest_first);
    }
}

void CParameterBlackboard::writeString(const std::string &input, uint32_t uiOffset)
{
    assert(input.size() + 1 + uiOffset <= getSize());
    auto dest_last = std::copy(begin(input), end(input), atOffset(uiOffset));
    *dest_last = '\0';
}

void CParameterBlackboard::readString(std::string &output, uint32_t uiOffset) const
{
    // As the string is null terminated in the blackboard,
    // the size that will be read is known. (>= 1)
    assert(uiOffset + 1 <= getSize());

    // Get the pointer to the null terminated string
    const uint8_t *first = &mBlackboard[uiOffset];
    output = reinterpret_cast<const char *>(first);
}

// Access from/to subsystems
uint8_t* CParameterBlackboard::getLocation(uint32_t uiOffset)
{
    assert(uiOffset < getSize());
    return &mBlackboard[uiOffset];
}

// Configuration handling
void CParameterBlackboard::restoreFrom(const CParameterBlackboard* pFromBlackboard, uint32_t uiOffset)
{
    const auto &fromBB = pFromBlackboard->mBlackboard;
    assert(fromBB.size() + uiOffset <= getSize());
    std::copy(begin(fromBB), end(fromBB), atOffset(uiOffset));
}

void CParameterBlackboard::saveTo(CParameterBlackboard* pToBlackboard, uint32_t uiOffset) const
{
    auto &toBB = pToBlackboard->mBlackboard;
    assert(toBB.size() + uiOffset <= getSize());
    std::copy_n(atOffset(uiOffset), toBB.size(), begin(toBB));
}

// Serialization
void CParameterBlackboard::serialize(CBinaryStream& binaryStream)
{
    if (binaryStream.isOut()) {

        binaryStream.write(mBlackboard.data(), getSize());
    } else {

        binaryStream.read(mBlackboard.data(), getSize());
    }
}
