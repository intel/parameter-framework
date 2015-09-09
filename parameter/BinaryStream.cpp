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
#include "BinaryStream.h"
#include <string.h>
#include <assert.h>

using namespace std;

CBinaryStream::CBinaryStream(const string& strFileName, bool bOut, size_t uiDataSize, uint8_t uiStructureChecksum) :
    _strFileName(strFileName),
    _bOut(bOut),
    _uiDataSize(uiDataSize),
    _uiStructureChecksum(uiStructureChecksum),
    _puiData(new uint8_t[uiDataSize]),
    _uiPos(0),
    _bOpen(false)
{
}

CBinaryStream::~CBinaryStream()
{
    if (_bOpen) {

        close();
    }

    delete [] _puiData;
}

bool CBinaryStream::open(string& strError)
{
    assert(!_bOpen);

    _fileStream.open(_strFileName.c_str(), (_bOut ? ios::out : ios::in|ios::ate)|ios::binary);

    if (!_fileStream.is_open() || !_fileStream.good()) {

        strError = string("Failed to ") + (_bOut ? "write" : "read") + "-open";

        return false;
    }
    if (!_bOut) {

        // Get file size
        auto uiFileSize = _fileStream.tellg();

        // `istream::pos_type` represent the position in a file stream.
        // Thus when at the end of the file, it represent it's size.
        // As a result `pos_type` must be able to represent any file size.
        // 
        // `size_t` on the other hand must be able to represent any contiguous
        // memory array size.
        // 
        // Those two type are not necessary the same but it is safe to assume
        // that `pos_type` is equal or bigger than `size_t`, as supported
        // system can all have files a lot bigger than the usual memory size.
        // 
        // As a result when comparing a file size and a structure size, use
        // `pos_type`.
        decltype(uiFileSize) expectedSize = _uiDataSize + sizeof(_uiStructureChecksum);

        // Validate file size
        if (expectedSize != uiFileSize) {

            // Size different from expected
            strError = "Unexpected file size";

            return false;
        }

        // Back to beginning of file
        _fileStream.seekg(0, ios::beg);

        // Get data
        _fileStream.read((char*)_puiData, _uiDataSize);

        // File checksum
        uint8_t uiFileChecksum;
        _fileStream.read((char*)&uiFileChecksum, sizeof(uiFileChecksum));

        // Data checksum
        uint8_t uiDataChecksum = computeChecksum();

        // Validate checksum
        if (uiDataChecksum != uiFileChecksum) {

            strError = "Integrity checks failed";

            return false;
        }
    }

    // Keep track
    _bOpen = true;

    return true;
}

void CBinaryStream::close()
{
    assert(_bOpen);

    if (_bOut) {

        // Get data
        _fileStream.write((const char*)_puiData, _uiDataSize);

        // Compute checksum
        uint8_t uiDataChecksum = computeChecksum();

        // Write checksum
        _fileStream.write((const char*)&uiDataChecksum, sizeof(uiDataChecksum));
    }

    // Keep track
    _bOpen = false;

    // Close file
    _fileStream.close();
}

void CBinaryStream::reset()
{
    _uiPos = 0;
}

void CBinaryStream::write(const uint8_t* puiData, size_t uiSize)
{
    assert(_uiPos + uiSize <= _uiDataSize);

    memcpy(&_puiData[_uiPos], puiData, uiSize);

    _uiPos += uiSize;
}

void CBinaryStream::read(uint8_t* puiData, size_t uiSize)
{
    assert(_uiPos + uiSize <= _uiDataSize);

    memcpy(puiData, &_puiData[_uiPos], uiSize);

    _uiPos += uiSize;
}

uint8_t CBinaryStream::computeChecksum() const
{
    uint32_t uiIndex;
    uint8_t uiDataChecksum = _uiStructureChecksum;

    for (uiIndex = 0; uiIndex < _uiDataSize; uiIndex++) {

        uiDataChecksum += _puiData[uiIndex];
    }
    return uiDataChecksum;
}

bool CBinaryStream::isOut() const
{
    return _bOut;
}
