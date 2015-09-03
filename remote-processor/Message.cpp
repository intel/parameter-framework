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

CMessage::Result CMessage::send(const std::vector<uint8_t> &data)
{
    // Size
    uint16_t size = data.size();
    if (!_socket->write(&size, sizeof(size))) {

        return make_pair(Code::error, string("Size write failed: ") + strerror(errno));
    }

    // Data
    if (!_socket->write((const char *)&data[0], data.size())) {

        return make_pair(Code::error, string("Data write failed: ") + strerror(errno));
    }

    return make_pair(Code::success, std::string("Success"));
}

CMessage::Result CMessage::recv(std::vector<uint8_t> &data)
{
    uint16_t size;
    if (!_socket->read(&size, sizeof(size))) {

        return make_pair(errno == ECONNRESET ? Code::peerDisconnected : Code::error,
                         string("Size read failed: ") + strerror(errno));
    }

    /* discard spurious content if any */
    data.clear();
    data.resize(size);

    // Data receive
    if (!_socket->read((char *)&data[0], data.size())) {

        return make_pair(errno == ECONNRESET ? Code::peerDisconnected : Code::error,
                         string("Data read failed: ") + strerror(errno));
    }

    return make_pair(Code::success, std::string("Success"));
}
