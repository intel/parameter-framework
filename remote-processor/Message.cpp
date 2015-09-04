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
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <vector>

using std::string;

CMessage::Result CMessage::send(const std::vector<uint8_t> &data)
{
    asio::error_code error;

    uint16_t size = data.size();
    if (!asio::write(_socket, asio::buffer(&size, sizeof(size)), error)) {

        return make_pair(Code::error, string("Size write failed: ") + strerror(errno));
    }

    // Data
    if (!asio::write(_socket, asio::buffer(data), error)) {
        return make_pair(Code::error, string("Data write failed: ") + strerror(errno));
    }

    return make_pair(Code::success, std::string("Success"));
}

CMessage::Result CMessage::recv(std::vector<uint8_t> &data)
{
    uint16_t size;
    asio::error_code error;

    if (!asio::read(_socket, asio::buffer(&size, sizeof(size)), error)) {

        return make_pair(error == asio::error::eof ? Code::peerDisconnected : Code::error,
                         string("Size read failed: ") + strerror(errno));
    }

    /* discard spurious content if any */
    data.clear();
    data.resize(size);

    // Data receive
    if (!asio::read(_socket, asio::buffer(data), error)) {

        return make_pair(error == asio::error::eof ? Code::peerDisconnected : Code::error,
                         string("Data read failed: ") + strerror(errno));
    }

    return make_pair(Code::success, std::string("Success"));
}
