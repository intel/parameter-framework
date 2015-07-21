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

#include "NonCopyable.hpp"

#include <cstdint>
#include <string>
#include <vector>

class CParameterBlackboard : private utility::NonCopyable
{
public:
    // Size
    void setSize(size_t size);
    size_t getSize() const;

    // Single parameter access
    void writeInteger(const void* pvSrcData, size_t size, size_t offset, bool bBigEndian);
    void readInteger(void* pvDstData, size_t size, size_t offset, bool bBigEndian) const;

    void writeString(const std::string &input, size_t offset);
    void readString(std::string &output, size_t offset) const;

    void writeBuffer(const void* pvSrcData, size_t size, size_t offset);
    void readBuffer(void* pvDstData, size_t size, size_t offset) const;

    /**
     * Raw write the blackboard memory.
     * May be used to write a configurable element's settings
     *
     * @param[in] bytes the source data bytes vector.
     * @param[in] offset the destination offset in the blackboard.
     *
     * Notes:
     *    - Endianness is not handled by the method and thus data are written in the blackboard
     *      with same representation as the one passed.
     *    - This function will assert if the bytes size + offset exceeds the size of the blackboard iself.
     */
    void writeBytes(const std::vector<uint8_t>& bytes, uint32_t offset);

    /**
     * Raw read the blackboard memory.
     * May be used to read a configurable element's settings
     *
     * @param[in] bytes the destinatin data bytes vector.
     * @param[in] offset the source offset in the blackboard.
     *
     * Notes:
     *    - Endianness is not handled by the method and thus data are read in the blackboard's representation.
     *    - This function will assert if the bytes size + offset exceeds the size of the blackboard iself.
     *    - The user MUST reserve exactly as many elements as the amount to read
     */
    void readBytes(std::vector<uint8_t>& bytes, uint32_t offset) const;

    // Access from/to subsystems
    uint8_t* getLocation(size_t offset);

    // Configuration handling
    void restoreFrom(const CParameterBlackboard* pFromBlackboard, size_t offset);
    void saveTo(CParameterBlackboard* pToBlackboard, size_t offset) const;

private:
    void assertValidAccess(size_t offset, size_t size) const;

    using Blackboard = std::vector<uint8_t>;
    Blackboard mBlackboard;

    Blackboard::iterator atOffset(size_t offset) { return begin(mBlackboard) + offset; }
    Blackboard::const_iterator atOffset(size_t offset) const { return begin(mBlackboard) + offset; }
};

