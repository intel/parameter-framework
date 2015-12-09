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

#include "TmpFile.hpp"

#include <memory>

#include <Windows.h>

using std::to_string;

namespace parameterFramework
{
namespace utility
{

/** Format an error code returned by GetLastError to a human readable string. */
static std::string formatError(DWORD error)
{
    LPTSTR formatedError = nullptr; // Pointer to the output buffer

    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM |         // use system message tables to retrieve error text
            FORMAT_MESSAGE_ALLOCATE_BUFFER | // allocate buffer on local heap for error text
            FORMAT_MESSAGE_IGNORE_INSERTS,   // no insertion parameters
        NULL,                                // unused with FORMAT_MESSAGE_FROM_SYSTEM
        error,                               // the error to format
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Use default language
        (LPTSTR)&formatedError,                    // output a pointer to the formated string
        0,                                         // minimum size for output buffer
        NULL);                                     // No arguments

    // Release memory allocated by FormatMessage() on exit
    // Ignore LocalFree failure
    auto localFree = [](char *ptr) { LocalFree(ptr); };
    std::unique_ptr<char, decltype(localFree)> guard{formatedError, localFree};

    if (formatedError == nullptr) {
        return "Could not format error " + to_string(error) + ": " + to_string(::GetLastError());
    }
    return formatedError;
}

std::string TmpFile::mktmp()
{
    char directory[] = ".";
    char prefix[] = "pfw"; // GetTempFileName uses up to the first three characters
    char path[MAX_PATH + 1];
    if (::GetTempFileName(directory, prefix, 0, path) == 0) {
        auto error = ::GetLastError();

        auto message = std::string() + "Could not create a tmp file in \"" + directory +
                       "\", with prefix \"" + prefix + "\": (" + to_string(error) + ") " +
                       formatError(error);
        throw std::runtime_error(message);
    }

    return path;
}

} // utility
} // parameterFramework
