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

using namespace std;

class CSocket;

class CMessage
{
public:
    CMessage(uint8_t ucMsgId);
    CMessage();
    virtual ~CMessage();

    enum Result {
        success,
        peerDisconnected,
        error
    };

    /** Write or read the message on pSocket.
     *
     * @param[in,out] pSocket is the socket on wich IO operation will be made.
     * @param[in] bOut if true message should be read,
     *                 if false it should be written.
     * @param[out] strError on failure, a string explaining the error,
     *                      on success, undefined.
     *
     * @return success if a correct message could be recv/send
     *         peerDisconnected if the peer disconnected before the first socket access.
     *         error if the message could not be read/write for any other reason
     */
    Result serialize(CSocket* pSocket, bool bOut, std::string &strError);

protected:
    // Msg Id
    uint8_t getMsgId() const;
    // Data
    void writeData(const void* pvData, uint32_t uiSize);
    void readData(void* pvData, uint32_t uiSize);
    void writeString(const string& strData);
    void readString(string& strData);
    uint32_t getStringSize(const string& strData) const;
    // Remaining data size
    uint32_t getRemainingDataSize() const;
private:
    CMessage(const CMessage&);
    CMessage& operator=(const CMessage&);
    // Data allocation
    void allocateData(uint32_t uiDataSize);
    // Fill data to send
    virtual void fillDataToSend() = 0;
    // Collect received data
    virtual void collectReceivedData() = 0;
    // Size
    virtual uint32_t getDataSize() const = 0;
    // Checksum
    uint8_t computeChecksum() const;

    // MsgId
    uint8_t _ucMsgId;
    // Data
    uint8_t* _pucData;
    // Data size
    uint32_t _uiDataSize;
    // Read/Write Index
    uint32_t _uiIndex;
};
