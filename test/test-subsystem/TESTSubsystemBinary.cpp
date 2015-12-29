/*
* Copyright (c) 2011-2015, Intel Corporation
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
#include <convert.hpp>
#include <string.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <assert.h>
#include <algorithm>
#include <AlwaysAssert.hpp>
#include <Iterator.hpp>
#include "TESTSubsystemBinary.h"

#define base CTESTSubsystemObject

CTESTSubsystemBinary::CTESTSubsystemBinary(
    const std::string &strMappingValue, CInstanceConfigurableElement *pInstanceConfigurableElement,
    const CMappingContext &context, core::log::Logger &logger)
    : base(strMappingValue, pInstanceConfigurableElement, context, logger)
{
}

std::string CTESTSubsystemBinary::toString(const void *pvValue, size_t size) const
{
    std::ostringstream strStream;
    uint32_t uiValue = 0;

    assert(size <= sizeof(uiValue));

    auto first = MAKE_ARRAY_ITERATOR(static_cast<const uint8_t *>(pvValue), size);
    auto destination = MAKE_ARRAY_ITERATOR(reinterpret_cast<uint8_t *>(&uiValue), sizeof(uiValue));

    std::copy_n(first, size, destination);

    strStream << "0x" << std::hex << uiValue;

    return strStream.str();
}

void CTESTSubsystemBinary::fromString(const std::string &strValue, void *pvValue, size_t size)
{
    uint32_t uiValue;

    assert(size <= sizeof(uiValue));

    if (!convertTo(strValue, uiValue)) {
        throw std::runtime_error("Unable to convert \"" + strValue + "\" to uint32");
    }

    auto first = MAKE_ARRAY_ITERATOR(reinterpret_cast<const uint8_t *>(&uiValue), size);
    auto destination = MAKE_ARRAY_ITERATOR(static_cast<uint8_t *>(pvValue), size);
    std::copy_n(first, size, destination);
}
