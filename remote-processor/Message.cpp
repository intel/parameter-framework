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
#include "Message.h"
#include <assert.h>
#include "Socket.h"
#include "RemoteProcessorProtocol.h"
#include <string.h>
#include <assert.h>

CMessage::CMessage(uint8_t ucMsgId) : _ucMsgId(ucMsgId), _pucData(NULL), _uiDataSize(0), _uiIndex(0)
{
}

CMessage::CMessage() : _ucMsgId(-1), _pucData(NULL), _uiDataSize(0), _uiIndex(0)
{
}

CMessage::~CMessage()
{
    delete [] _pucData;
}

// Msg Id
uint8_t CMessage::getMsgId() const
{
    return _ucMsgId;
}

// Data
void CMessage::writeData(const void* pvData, uint32_t uiSize)
{
    assert(_uiIndex + uiSize <= _uiDataSize);

    // Copy
    memcpy(&_pucData[_uiIndex], pvData, uiSize);

    // Index
    _uiIndex += uiSize;
}

void CMessage::readData(void* pvData, uint32_t uiSize)
{
    assert(_uiIndex + uiSize <= _uiDataSize);

    // Copy
    memcpy(pvData, &_pucData[_uiIndex], uiSize);

    // Index
    _uiIndex += uiSize;
}

void CMessage::writeString(const string& strData)
{
    // Size
    uint32_t uiSize = strData.length();

    writeData(&uiSize, sizeof(uiSize));

    // Content
    writeData(strData.c_str(), uiSize);
}

void CMessage::readString(string& strData)
{
    // Size
    uint32_t uiSize;

    readData(&uiSize, sizeof(uiSize));

    // Data
    char* pcData = new char[uiSize + 1];

    // Content
    readData(pcData, uiSize);

    // NULL-terminate string
    pcData[uiSize] = '\0';

    // Output
    strData = pcData;

    // Delete
    delete [] pcData;
}

uint32_t CMessage::getStringSize(const string& strData) const
{
    // Return string length plus room to store its length
    return strData.length() + sizeof(uint32_t);
}

// Remaining data size
uint32_t CMessage::getRemainingDataSize() const
{
    return _uiDataSize - _uiIndex;
}

// Send/Receive
bool CMessage::serialize(CSocket* pSocket, bool bOut)
{
    if (bOut) {

        // Make room for data to send
        allocateData(getDataSize());

        // Get data from derived
        fillDataToSend();

        // Finished providing data?
        assert(_uiIndex == _uiDataSize);

        // First send sync word
        uint16_t uiSyncWord = SYNC_WORD;

        if (!pSocket->write(&uiSyncWord, sizeof(uiSyncWord))) {

            return false;
        }

        // Size
        uint32_t uiSize = sizeof(_ucMsgId) + _uiDataSize;

        if (!pSocket->write(&uiSize, sizeof(uiSize))) {

            return false;
        }

        // Msg Id
        if (!pSocket->write(&_ucMsgId, sizeof(_ucMsgId))) {

            return false;
        }

        // Data
        if (!pSocket->write(_pucData, _uiDataSize)) {

            return false;
        }

        // Checksum
        uint8_t ucChecksum = computeChecksum();

        if (!pSocket->write(&ucChecksum, sizeof(ucChecksum))) {

            return false;
        }

    } else {
        // First read sync word
        uint16_t uiSyncWord;

        if (!pSocket->read(&uiSyncWord, sizeof(uiSyncWord))) {

            return false;
        }

        // Check Sync word
        if (uiSyncWord != SYNC_WORD) {

            return false;
        }

        // Size
        uint32_t uiSize;

        if (!pSocket->read(&uiSize, sizeof(uiSize))) {

            return false;
        }

        // Msg Id
        if (!pSocket->read(&_ucMsgId, sizeof(_ucMsgId))) {

            return false;
        }

        // Data

        // Allocate
        allocateData(uiSize - sizeof(_ucMsgId));

        // Data receive
        if (!pSocket->read(_pucData, _uiDataSize)) {

            return false;
        }

        // Checksum
        uint8_t ucChecksum;

        if (!pSocket->read(&ucChecksum, sizeof(ucChecksum))) {

            return false;
        }
        // Compare
        if (ucChecksum != computeChecksum()) {

            return false;
        }

        // Collect data in derived
        collectReceivedData();
    }

    return true;
}

// Checksum
uint8_t CMessage::computeChecksum() const
{
    uint8_t uiChecksum = (_ucMsgId & 0xFF) + (_ucMsgId >> 8);

    uint32_t uiIndex;

    for (uiIndex = 0; uiIndex < _uiDataSize; uiIndex++) {

        uiChecksum += _pucData[uiIndex];
    }

    return uiChecksum;
}

// Data allocation
void CMessage::allocateData(uint32_t uiSize)
{
    // Remove previous one
    if (_pucData) {

        delete [] _pucData;
    }
    // Do allocate
    _pucData = new uint8_t[uiSize];

    // Record size
    _uiDataSize = uiSize;

    // Reset Index
    _uiIndex = 0;
}
