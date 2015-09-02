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
#include "Message.h"
#include <assert.h>
#include "Socket.h"
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <vector>

using std::string;

CMessage::Result CMessage::send(CSocket* pSocket, string& strError)
{
    // Get data from derived
    std::vector<uint8_t> data = getDataToSend();

    // Size
    uint16_t size = data.size();
    if (!pSocket->write(&size, sizeof(size))) {

        strError += string("Size write failed: ") + strerror(errno);
        return error;
    }

    // Data
    if (!pSocket->write((const char *)&data[0], data.size())) {

        strError = string("Data write failed: ") + strerror(errno);
        return error;
    }

    return success;
}

CMessage::Result CMessage::recv(CSocket* pSocket, string& strError)
{
    // Size
    uint16_t size;
    if (!pSocket->read(&size, sizeof(size))) {

        strError = string("Size read failed: ") + strerror(errno);
        return error;
    }

    // Data

    std::vector<uint8_t> data(size);

    // Data receive
    if (!pSocket->read((char *)&data[0], data.size())) {

        strError = string("Data read failed: ") + strerror(errno);
        return error;
    }

    processData(data);

    return success;
}
