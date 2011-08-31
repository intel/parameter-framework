/* <auto_header>
 * <FILENAME>
 * 
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
 *  AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 * 
 * 
 * </auto_header>
 */
#include "BinaryStream.h"
#include <string.h>
#include <assert.h>

using namespace std;

CBinaryStream::CBinaryStream(const string& strFileName, bool bOut, uint32_t uiDataSize, uint8_t uiStructureChecksum) :
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
    delete [] _puiData;

    if (_bOpen) {

        close();
    }
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
        ifstream::pos_type uiFileSize = _fileStream.tellg();

        // Validate file size
        if (_uiDataSize + sizeof(_uiStructureChecksum) != (uint32_t)uiFileSize) {

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

void CBinaryStream::write(const uint8_t* puiData, uint32_t uiSize)
{
    assert(_uiPos + uiSize <= _uiDataSize);

    memcpy(&_puiData[_uiPos], puiData, uiSize);

    _uiPos += uiSize;
}

void CBinaryStream::read(uint8_t* puiData, uint32_t uiSize)
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
