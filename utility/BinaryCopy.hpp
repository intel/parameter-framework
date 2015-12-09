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

#include <algorithm>
#include <type_traits>
#include "Iterator.hpp"
#include <cassert>

namespace utility
{

/**
 * Raw copy of one variable to another of the same size
 *
 * This can be regarder as a reinterpret_cast but does a copy and does not
 * break strict-aliasing rules.
 *
 * The source and the destination must have the same storage size (e.g. copying
 * a uint8_t into a uint32_t won't compile)
 *
 * @tparam Source The source type
 * @tparam Destination the destination type (even if it is a reference, this
 *         function returns by copy)
 * @param source Source variable
 * @returns the source, reinterpreted as the destination type
 */
template <class Destination, class Source>
typename std::remove_reference<Destination>::type binaryCopy(const Source source)
{
    static_assert(sizeof(Source) == sizeof(Destination),
                  "Source and Destination must have the same size");

    using Destination_ = decltype(binaryCopy<Destination>(source));

    union
    {
        Source source;
        Destination_ destination;
    } hack;

    hack.source = source;
    return hack.destination;
}

} // namespace utility
