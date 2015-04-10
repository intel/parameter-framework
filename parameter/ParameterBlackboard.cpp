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
#include <string.h>
#include <assert.h>

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

    _pucData = new uint8_t[uiSize];

    memset(_pucData, 0, uiSize);

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

    if (!bBigEndian) {

        memcpy(_pucData + uiOffset, pvSrcData, uiSize);
    } else {

        uint32_t uiIndex;
        const uint8_t* puiSrcData = (const uint8_t*)pvSrcData;

        for (uiIndex = 0; uiIndex < uiSize; uiIndex++) {

            _pucData[uiIndex + uiOffset] = puiSrcData[uiSize - uiIndex - 1];
        }
    }
}

void CParameterBlackboard::readInteger(void* pvDstData, uint32_t uiSize, uint32_t uiOffset, bool bBigEndian) const
{
    assert(uiSize + uiOffset <= _uiSize);

    if (!bBigEndian) {

        memcpy(pvDstData, _pucData + uiOffset, uiSize);
    } else {

        uint32_t uiIndex;
        uint8_t* puiDstData = (uint8_t*)pvDstData;

        for (uiIndex = 0; uiIndex < uiSize; uiIndex++) {

            puiDstData[uiSize - uiIndex - 1] = _pucData[uiIndex + uiOffset];
        }
    }
}

void CParameterBlackboard::writeString(const std::string &input, uint32_t uiOffset)
{
    strcpy((char*)_pucData + uiOffset, input.c_str());
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
    memcpy(_pucData + uiOffset, pFromBlackboard->_pucData, pFromBlackboard->_uiSize);
}

void CParameterBlackboard::saveTo(CParameterBlackboard* pToBlackboard, uint32_t uiOffset) const
{
    memcpy(pToBlackboard->_pucData, _pucData + uiOffset, pToBlackboard->_uiSize);
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
