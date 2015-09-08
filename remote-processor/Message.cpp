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
#include <type_traits>
#include <assert.h>
#include <errno.h>
#include <vector>

using std::string;

struct Header {
    Header() : magic(0), version(0), size(0) {}
    Header(uint32_t hsize) : magic(MAGIC), version(VERSION), size(hsize) {}

    static const uint16_t MAGIC = 0xC0EA;
    static const uint16_t VERSION = 2;

    const uint16_t magic;
    const uint16_t version;
    const uint32_t size;
};

static_assert(std::is_standard_layout<Header>::value, "Bad header layout");

CMessage::Result CMessage::send(const std::vector<uint8_t> &data)
{
    asio::error_code error;

    Header h(data.size());

    if (!asio::write(_socket, asio::buffer(&h, sizeof(h)), error)) {
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
    Header h;
    asio::error_code error;

    if (!asio::read(_socket, asio::buffer(&h, sizeof(h)), error)) {
        return make_pair(error == asio::error::eof ? Code::peerDisconnected : Code::error,
                         string("Size read failed: ") + strerror(errno));
    }

    /* detect former 'protocol' */
    if (h.magic == 0xBABE) {
        const uint8_t payload[] = {
            /* this wonderful array stands for "Wrong Protocol Version: got 'legacy' expected '2'"
             * in the previous protocol encoding */
            0xbe, 0xba, 0x36, 0x00, 0x00, 0x00, 0x02, 0x31, 0x00, 0x00, 0x00, 0x57, 0x72, 0x6f,
            0x6e, 0x67, 0x20, 0x50, 0x72, 0x6f, 0x74, 0x6f, 0x63, 0x6f, 0x6c, 0x20, 0x56, 0x65,
            0x72, 0x73, 0x69, 0x6f, 0x6e, 0x3a, 0x20, 0x67, 0x6f, 0x74, 0x20, 0x27, 0x6c, 0x65,
            0x67, 0x61, 0x63, 0x79, 0x27, 0x20, 0x65, 0x78, 0x70, 0x65, 0x63, 0x74, 0x65, 0x64,
            0x20, 0x27, 0x32, 0x27, 0x51 };

        asio::write(_socket, asio::buffer(payload, sizeof(payload)), error);
        return make_pair(Code::error,  string("Message of invalid protocol version received"));
    }

    if (h.magic != Header::MAGIC) {
        return make_pair(Code::error, string("Invalid magic in message header: expected ")
                + std::to_string(Header::MAGIC) + " got " + std::to_string(h.magic));
    }

    if (h.version != Header::VERSION) {
        return make_pair(Code::error, string("Invalid version in message header: expected ")
                + std::to_string(Header::VERSION) + " got " + std::to_string(h.version));
    }

    /* discard spurious content if any */
    data.clear();
    data.resize(h.size);

    // Data receive
    if (!asio::read(_socket, asio::buffer(data), error)) {

        return make_pair(error == asio::error::eof ? Code::peerDisconnected : Code::error,
                         string("Data read failed: ") + strerror(errno));
    }

    return make_pair(Code::success, std::string("Success"));
}
