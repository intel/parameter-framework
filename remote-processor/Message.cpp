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
#include "Message.h"
#include "Socket.h"
#include "Iterator.hpp"
#include <asio.hpp>
#include <vector>
#include <numeric>
#include <cassert>

using std::string;

CMessage::CMessage(MsgType ucMsgId) : _ucMsgId(ucMsgId), _uiIndex(0)
{
}

CMessage::CMessage() : _ucMsgId(MsgType::EInvalid), _uiIndex(0)
{
}

// Msg Id
CMessage::MsgType CMessage::getMsgId() const
{
    return _ucMsgId;
}

bool CMessage::isValidAccess(size_t offset, size_t size) const
{
    return offset + size <= getMessageDataSize();
}

// Data
void CMessage::writeData(const void *pvData, size_t size)
{
    assert(isValidAccess(_uiIndex, size));

    auto first = MAKE_ARRAY_ITERATOR(static_cast<const uint8_t *>(pvData), size);
    auto last = first + size;
    auto destFirst = begin(mData) + _uiIndex;

    std::copy(first, last, destFirst);

    _uiIndex += size;
}

void CMessage::readData(void *pvData, size_t size)
{
    assert(isValidAccess(_uiIndex, size));

    auto first = begin(mData) + _uiIndex;
    auto last = first + size;
    auto destFirst = MAKE_ARRAY_ITERATOR(static_cast<uint8_t *>(pvData), size);

    std::copy(first, last, destFirst);

    _uiIndex += size;
}

void CMessage::writeString(const string &strData)
{
    // Size
    uint32_t size = static_cast<uint32_t>(strData.length());

    writeData(&size, sizeof(size));

    // Content
    writeData(strData.c_str(), size);
}

void CMessage::readString(string &strData)
{
    // Size
    uint32_t uiSize;

    readData(&uiSize, sizeof(uiSize));

    // Data
    std::vector<char> string(uiSize + 1);

    // Content
    readData(string.data(), uiSize);

    // NULL-terminate string
    string.back() = '\0';

    // Output
    strData = string.data();
}

size_t CMessage::getStringSize(const string &strData) const
{
    // Return string length plus room to store its length
    return strData.length() + sizeof(uint32_t);
}

// Remaining data size
size_t CMessage::getRemainingDataSize() const
{
    return getMessageDataSize() - _uiIndex;
}

// Send/Receive
CMessage::Result CMessage::serialize(Socket &&socket, bool bOut, string &strError)
{
    asio::ip::tcp::socket &asioSocket = socket.get();

    if (bOut) {
        asio::error_code ec;

        // Make room for data to send
        allocateData(getDataSize());

        // Get data from derived
        fillDataToSend();

        // Finished providing data?
        assert(_uiIndex == getMessageDataSize());

        // First send sync word
        uint16_t uiSyncWord = SYNC_WORD;

        if (!asio::write(asioSocket, asio::buffer(&uiSyncWord, sizeof(uiSyncWord)), ec)) {

            if (ec == asio::error::eof) {
                return peerDisconnected;
            }
            return error;
        }

        // Size
        uint32_t uiSize = (uint32_t)(sizeof(_ucMsgId) + getMessageDataSize());

        if (!asio::write(asioSocket, asio::buffer(&uiSize, sizeof(uiSize)), ec)) {

            strError += string("Size write failed: ") + ec.message();
            return error;
        }

        // Msg Id
        if (!asio::write(asioSocket, asio::buffer(&_ucMsgId, sizeof(_ucMsgId)), ec)) {

            strError += string("Msg write failed: ") + ec.message();
            return error;
        }

        // Data
        if (!asio::write(asioSocket, asio::buffer(mData), ec)) {

            strError = string("Data write failed: ") + ec.message();
            return error;
        }

        // Checksum
        uint8_t ucChecksum = computeChecksum();

        if (!asio::write(asioSocket, asio::buffer(&ucChecksum, sizeof(ucChecksum)), ec)) {

            strError = string("Checksum write failed: ") + ec.message();
            return error;
        }

    } else {
        // First read sync word
        uint16_t uiSyncWord = 0;
        asio::error_code ec;

        if (!asio::read(asioSocket, asio::buffer(&uiSyncWord, sizeof(uiSyncWord)), ec)) {
            strError = string("Sync read failed: ") + ec.message();
            if (ec == asio::error::eof) {
                return peerDisconnected;
            }
            return error;
        }

        // Check Sync word
        if (uiSyncWord != SYNC_WORD) {

            strError = "Sync word incorrect";
            return error;
        }

        // Size
        uint32_t uiSize = 0;

        if (!asio::read(asioSocket, asio::buffer(&uiSize, sizeof(uiSize)), ec)) {
            strError = string("Size read failed: ") + ec.message();
            return error;
        }

        // Msg Id
        if (!asio::read(asioSocket, asio::buffer(&_ucMsgId, sizeof(_ucMsgId)), ec)) {
            strError = string("Msg id read failed: ") + ec.message();
            return error;
        }

        // Data

        // Allocate
        allocateData(uiSize - sizeof(_ucMsgId));

        // Data receive
        if (!asio::read(asioSocket, asio::buffer(mData), ec)) {
            strError = string("Data read failed: ") + ec.message();
            return error;
        }

        // Checksum
        uint8_t ucChecksum = 0;

        if (!asio::read(asioSocket, asio::buffer(&ucChecksum, sizeof(ucChecksum)), ec)) {
            strError = string("Checksum read failed: ") + ec.message();
            return error;
        }
        // Compare
        if (ucChecksum != computeChecksum()) {

            strError = "Received checksum != computed checksum";
            return error;
        }

        // Collect data in derived
        collectReceivedData();
    }

    return success;
}

// Checksum
uint8_t CMessage::computeChecksum() const
{
    return accumulate(begin(mData), end(mData), static_cast<uint8_t>(_ucMsgId));
}

// Allocation of room to store the message
void CMessage::allocateData(size_t size)
{
    // Remove previous one
    mData.clear();

    // Do allocate
    mData.resize(size);

    // Reset Index
    _uiIndex = 0;
}
