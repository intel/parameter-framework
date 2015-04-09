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

#include <stdint.h>
#include <string>
#include <fstream>

class CBinaryStream
{
public:
    CBinaryStream(const std::string& strFileName, bool bOut, size_t uiDataSize, uint8_t uiStructureChecksum);
    ~CBinaryStream();

    // Open close
    bool open(std::string& strError);
    void close();

    // Seek
    void reset();

    // Read/Write
    void write(const uint8_t* puiData, size_t uiSize);
    void read(uint8_t* puiData, size_t uiSize);

    // Direction
    bool isOut() const;
private:
    CBinaryStream(const CBinaryStream&);
    CBinaryStream& operator=(const CBinaryStream&);
    // Checksum
    uint8_t computeChecksum() const;

    // File name
    std::string _strFileName;
    // Serialization direction
    bool _bOut;
    // Data size
    size_t _uiDataSize;
    // System structure checksum
    uint8_t _uiStructureChecksum;
    // Read/Write data
    uint8_t* _puiData;
    // File
    std::fstream _fileStream;
    // Ops in faile
    size_t _uiPos;
    // File state
    bool _bOpen;
};

