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

#include <DynamicLibrary.hpp>

#include "windows.h"

#include <stdexcept>

const std::string DynamicLibrary::_osLibraryPrefix = "";
const std::string DynamicLibrary::_osLibrarySuffix = ".dll";

DynamicLibrary::DynamicLibrary(const std::string &path) : _path(osSanitizePathName(path))
{
    static_assert(sizeof(void *) == sizeof(HMODULE), "Incompatible object size");

    HMODULE module = LoadLibrary(_path.c_str());

    _handle = reinterpret_cast<void *>(module);

    if (_handle == nullptr) {

        throw std::runtime_error(_path + ": cannot open shared object file.");
    }
}

DynamicLibrary::~DynamicLibrary()
{
    HMODULE module = reinterpret_cast<HMODULE>(_handle);

    FreeLibrary(module);
}

void *DynamicLibrary::osGetSymbol(const std::string &symbol) const
{
    HMODULE module = reinterpret_cast<HMODULE>(_handle);

    void *sym = reinterpret_cast<void *>(GetProcAddress(module, symbol.c_str()));

    if (sym == nullptr) {

        throw std::runtime_error(_path + ": undefined symbol: " + symbol);
    }

    return sym;
}
