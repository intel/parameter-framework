/*
* Copyright (c) 2014-2015, Intel Corporation
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

#include "NonCopyable.hpp"

#include <string>

class DynamicLibrary : private utility::NonCopyable
{
public:
    /**
    * @param[in] path the library path which can be provided either in absolute path or
    *            or OS agnostic (ie. generic) name.
    * Note:
    * If generic name provided, OS specific prefix, suffix are added automatically
    */
    DynamicLibrary(const std::string &path);
    ~DynamicLibrary();

    /**
    * Get a symbol from library
    *
    * @param[in] symbol the symbol name
    * @return a symbol's address in the library if it exists, NULL otherwise
    */
    template <typename SymbolType>
    SymbolType getSymbol(const std::string &symbol) const
    {
        return reinterpret_cast<SymbolType>(osGetSymbol(symbol));
    }

private:
    /**
    * OS secific helper to get a symbol from library
    *
    * @param[in] symbol the symbol name
    * @return symbol's address in the library if it exists, NULL otherwise
    */
    void *osGetSymbol(const std::string &symbol) const;

    /**
    * Sanitize library path
    *
    * @param[in] path library stripped path (eg. no prefix, no suffix)
    * @return OS specific library path including prefix and suffix
    */
    static std::string osSanitizePathName(const std::string &path);

    /**
    * Opaque object for library handling
    */
    void *_handle;

    /**
     * Path to the library
     */
    const std::string _path;

    /**
    * OS Specific library name patterns
    * @{
    */
    static const std::string _osLibraryPrefix;
    static const std::string _osLibrarySuffix;
    /** @}*/
};
