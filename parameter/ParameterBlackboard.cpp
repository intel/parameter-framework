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

void CParameterBlackboard::writeString(const char* pcSrcData, uint32_t uiOffset)
{
    strcpy((char*)_pucData + uiOffset, pcSrcData);
}

void CParameterBlackboard::readString(char* pcDstData, uint32_t uiOffset) const
{
    strcpy(pcDstData, (const char*)_pucData + uiOffset);
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
