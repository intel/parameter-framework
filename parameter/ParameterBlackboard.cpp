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
#include "ParameterBlackboard.h"
#include "Iterator.hpp"
#include "AlwaysAssert.hpp"
#include <algorithm>

// Size
void CParameterBlackboard::setSize(size_t size)
{
    mBlackboard.resize(size);
}

size_t CParameterBlackboard::getSize() const
{
    return mBlackboard.size();
}

// Single parameter access
void CParameterBlackboard::writeInteger(const void *pvSrcData, size_t size, size_t offset)
{
    assertValidAccess(offset, size);

    auto first = MAKE_ARRAY_ITERATOR(static_cast<const uint8_t *>(pvSrcData), size);
    auto last = first + size;
    auto dest_first = atOffset(offset);

    std::copy(first, last, dest_first);
}

void CParameterBlackboard::readInteger(void *pvDstData, size_t size, size_t offset) const
{
    assertValidAccess(offset, size);

    auto first = atOffset(offset);
    auto last = first + size;
    auto dest_first = MAKE_ARRAY_ITERATOR(static_cast<uint8_t *>(pvDstData), size);

    std::copy(first, last, dest_first);
}

void CParameterBlackboard::writeString(const std::string &input, size_t offset)
{
    assertValidAccess(offset, input.size() + 1);

    auto dest_last = std::copy(begin(input), end(input), atOffset(offset));
    *dest_last = '\0';
}

void CParameterBlackboard::readString(std::string &output, size_t offset) const
{
    // As the string is null terminated in the blackboard,
    // the size that will be read is not known. (>= 1)
    assertValidAccess(offset, sizeof('\0'));

    // Get the pointer to the null terminated string
    const uint8_t *first = &mBlackboard[offset];
    output = reinterpret_cast<const char *>(first);
}

void CParameterBlackboard::writeBuffer(const void *pvSrcData, size_t size, size_t offset)
{
    writeInteger(pvSrcData, size, offset);
}
void CParameterBlackboard::readBuffer(void *pvDstData, size_t size, size_t offset) const
{
    readInteger(pvDstData, size, offset);
}

// Element access
void CParameterBlackboard::writeBytes(const std::vector<uint8_t> &bytes, size_t offset)
{
    assertValidAccess(offset, bytes.size());

    std::copy(begin(bytes), end(bytes), atOffset(offset));
}

void CParameterBlackboard::readBytes(std::vector<uint8_t> &bytes, size_t offset) const
{
    assertValidAccess(offset, bytes.size());

    std::copy_n(atOffset(offset), bytes.size(), begin(bytes));
}

// Access from/to subsystems
uint8_t *CParameterBlackboard::getLocation(size_t offset)
{
    assertValidAccess(offset, 1);
    return &mBlackboard[offset];
}

// Configuration handling
void CParameterBlackboard::restoreFrom(const CParameterBlackboard *pFromBlackboard, size_t offset)
{
    const auto &fromBB = pFromBlackboard->mBlackboard;
    assertValidAccess(offset, fromBB.size());
    std::copy(begin(fromBB), end(fromBB), atOffset(offset));
}

void CParameterBlackboard::saveTo(CParameterBlackboard *pToBlackboard, size_t offset) const
{
    auto &toBB = pToBlackboard->mBlackboard;
    assertValidAccess(offset, toBB.size());
    std::copy_n(atOffset(offset), toBB.size(), begin(toBB));
}

void CParameterBlackboard::assertValidAccess(size_t offset, size_t size) const
{
    ALWAYS_ASSERT(offset + size <= getSize(),
                  "Invalid data size access: offset=" << offset << " size=" << size
                                                      << "reference size=" << getSize());
}
