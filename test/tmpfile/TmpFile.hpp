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

#include <exception>
#include <fstream>
#include <string>
#include <array>

#include <cerrno>
#include <cstring>

namespace parameterFramework
{
namespace utility
{

/** Create a temporary file with the given content. */
class TmpFile
{
public:
    TmpFile(std::string content) : mPath(mktmp())
    {
        std::ofstream file(mPath);
        file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        file << content;
        // Close explicitly to detect errors (fstream destructor does not throw)
        file.close();
    }

    TmpFile(TmpFile &&right) : mPath(std::move(right.mPath)) { right.mPath.clear(); }

    /** Forbid copy semantic as sharing the tmp file is not needed.
     * @{ */
    TmpFile(const TmpFile &right) = delete;
    TmpFile &operator=(const TmpFile &right) = delete;
    /** @} */

    TmpFile &operator=(TmpFile &&right)
    {
        remove();
        mPath = std::move(right.mPath);
        right.mPath.clear();
        return *this;
    }

    ~TmpFile() { remove(); }

    /** @return the path to the temporary file.
     *          "" if the file was moved from.
     */
    const std::string &getPath() const { return mPath; }
private:
    /** @return a valid unique file name. */
    std::string mktmp();

    /** Throw an std::runtime_error with a message constructed from the context and std::errno.
     *
     * Call it after a c standard function failure.
     */
    static void throwErrnoError(std::string context)
    {
        auto message = context + ": (" + std::to_string(errno) + ") " + std::strerror(errno);
        throw std::runtime_error(message);
    }

    void remove()
    {
        if (not mPath.empty()) {
            if (std::remove(mPath.c_str()) != 0) {
                throwErrnoError("Could not delete tmpfile");
            }
        }
    }
    std::string mPath;
};

} // utility
} // parameterFramework
