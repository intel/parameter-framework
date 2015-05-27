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

#pragma once

#include <cstddef>

namespace utility
{

/** Write *completely* a buffer in a file descriptor.
 *
 * A wrapper around unistd::write that resumes write on incomplete access
 * and EAGAIN/EINTR error.
 *
 * @see man 2 write for the parameters.
 *
 * @return true if the buffer could be completely written,
 *        false on failure (see write's man errno section).
 */
bool fullWrite(int fd, const void *buf, size_t count);

/** Fill a buffer from a file descriptor.
 *
 * A wrapper around unistd::read that resumes read on incomplete access
 * and EAGAIN/EINTR error.
 *
 * @see man 2 read for the parameters.
 *
 * @return true if the buffer could be completely fill,
 *        false on failure (see read's man errno section).
 *
 * If the buffer could not be filled due to an EOF, return false but set
 * errno to 0.
 * @TODO Add a custom strerror to prevent logging "success" (`sterror(0)`) on
 *       EOF errors ?
 */
bool fullRead(int fd, void *buf, size_t count);

} // namespace utility

