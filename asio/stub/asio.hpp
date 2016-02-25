/*
 * Copyright (c) 2016, Intel Corporation
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

/** @file
 *
 * Stubs ASIO interfaces called by libremote-processor. This is used when
 * the user asks for networking support to be compiled out.
 */

#include <system_error>

namespace asio
{
struct dummy_base
{
    template <class... Args>
    dummy_base(Args &&...)
    {
    }
    void set_option(const dummy_base &) const {};
};
inline bool write(const dummy_base &, const dummy_base &, const dummy_base &)
{
    return true;
}
inline bool read(const dummy_base &, const dummy_base &, const dummy_base &)
{
    return true;
}
using buffer = dummy_base;
struct io_service : dummy_base
{
    template <class... Args>
    io_service(Args &&...)
    {
        throw std::runtime_error("Stub constructor called. Did you forget to set the "
                                 "'TuningAllowed' attribute to 'false' in the Parameter "
                                 "Framework's toplevel configuration file?");
    }

    void run(const dummy_base &) const {};
    void stop() const {};
};
struct socket_base : dummy_base
{
    using dummy_base::dummy_base;

    using linger = dummy_base;
    using enable_connection_aborted = dummy_base;
    void close() const {};
};

bool write(const dummy_base &, const dummy_base &, const dummy_base &);
bool read(const dummy_base &, const dummy_base &, const dummy_base &);

struct error_code : dummy_base, std::error_code
{
};
namespace error
{
static const error_code eof{};
}

namespace ip
{
namespace tcp
{
using v6 = dummy_base;
using no_delay = dummy_base;
using socket = socket_base;
struct endpoint : dummy_base
{
    using dummy_base::dummy_base;

    dummy_base protocol() const { return {}; };
};
struct acceptor : dummy_base
{
    using dummy_base::dummy_base;

    using reuse_address = dummy_base;
    void open(const dummy_base &) const {};
    void bind(const dummy_base &) const {};
    void listen() const {};
    void async_accept(const dummy_base &, const dummy_base &) const {};
};
}
}
}
