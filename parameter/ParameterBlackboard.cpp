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
#include <assert.h>
#include <algorithm>

CParameterBlackboard::CParameterBlackboard() : _pucData(NULL), _uiSize(0)
{
}

CParameterBlackboard::~CParameterBlackboard()
{
    delete [] _pucData;
}

// Size
void CParameterBlackboard::setSize(uint32_t uiSize)
{
    if (_pucData) {

        delete [] _pucData;
    }

    //Initializer is an empty pair of parentheses,
    //each element is value-initialized.
    _pucData = new uint8_t[uiSize]();

    _uiSize = uiSize;
}

uint32_t CParameterBlackboard::getSize() const
{
    return _uiSize;
}

// Single parameter access
void CParameterBlackboard::writeInteger(const void* pvSrcData, uint32_t uiSize, uint32_t uiOffset, bool bBigEndian)
{
    assert(uiSize + uiOffset <= _uiSize);

    auto first = static_cast<const char *>(pvSrcData);
    auto last = first + uiSize;
    auto dest_first = _pucData + uiOffset;

    if (!bBigEndian) {
        std::copy(first, last, dest_first);
    } else {
        std::reverse_copy(first, last, dest_first);
    }
}

void CParameterBlackboard::readInteger(void* pvDstData, uint32_t uiSize, uint32_t uiOffset, bool bBigEndian) const
{
    assert(uiSize + uiOffset <= _uiSize);

    auto first = _pucData + uiOffset;
    auto last = first + uiSize;
    auto dest_first = static_cast<char *>(pvDstData);

    if (!bBigEndian) {
        std::copy(first, last, dest_first);
    } else {
        std::reverse_copy(first, last, dest_first);
    }
}

void CParameterBlackboard::writeString(const std::string &input, uint32_t uiOffset)
{
    assert(input.size() + 1 + uiOffset <= _uiSize);
    auto dest_last = std::copy(begin(input), end(input), _pucData + uiOffset);
    *dest_last = '\0';
}

void CParameterBlackboard::readString(std::string &output, uint32_t uiOffset) const
{
    output = std::string((const char*)_pucData + uiOffset);
}

// Access from/to subsystems
uint8_t* CParameterBlackboard::getLocation(uint32_t uiOffset)
{
    return _pucData + uiOffset;
}

// Configuration handling
void CParameterBlackboard::restoreFrom(const CParameterBlackboard* pFromBlackboard, uint32_t uiOffset)
{
    std::copy_n(pFromBlackboard->_pucData, pFromBlackboard->_uiSize, _pucData + uiOffset);
}

void CParameterBlackboard::saveTo(CParameterBlackboard* pToBlackboard, uint32_t uiOffset) const
{
    std::copy_n(_pucData + uiOffset, pToBlackboard->_uiSize, pToBlackboard->_pucData);
}

// Serialization
void CParameterBlackboard::serialize(CBinaryStream& binaryStream)
{
    if (binaryStream.isOut()) {

        binaryStream.write(_pucData, _uiSize);
    } else {

        binaryStream.read(_pucData, _uiSize);
    }
}
