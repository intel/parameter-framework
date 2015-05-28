/*
 * Copyright (c) 2015, Intel Corporation
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

#include "FullIo.hpp"

#include <cerrno>
#include <unistd.h>

namespace utility
{

/** Workaround c++ `void *` arithmetic interdiction. */
template <class Ptr>
Ptr *add(Ptr *ptr, size_t count) {
    return (char *)ptr + count;
}

template <class Buff>
static bool fullAccess(ssize_t (&accessor)(int, Buff, size_t),
                       bool (&accessFailed)(ssize_t),
                       int fd, Buff buf, size_t count) {
    size_t done = 0; // Bytes already access in previous iterations
    while (done < count) {
        ssize_t accessed = accessor(fd, add(buf, done), count - done);
        if (accessFailed(accessed)) {
            return false;
        }
        done += accessed;
    }
    return true;
}

static bool accessFailed(ssize_t accessRes) {
    return accessRes == -1 and errno != EAGAIN and errno != EINTR;
}

bool fullWrite(int fd, const void *buf, size_t count) {
    return fullAccess(::write, accessFailed, fd, buf, count);
}

static bool readFailed(ssize_t readRes) {
    if (readRes == 0) { // read should not return 0 (EOF)
        errno = 0;
        return true;
    }
    return accessFailed(readRes);
}
bool fullRead(int fd, void *buf, size_t count) {
    return fullAccess(::read, readFailed, fd, buf, count);
}

} // namespace utility

