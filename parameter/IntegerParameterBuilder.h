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

#include "ElementBuilder.h"
#include "IntegerParameterType.h"

/** Specialized element builder for IntegerParameterType
 *
 * Dispatch to the correct template instance according to the signedness and
 * size.
 */
class IntegerParameterBuilder : public CElementBuilder
{
public:
    CElement *createElement(const CXmlElement &xmlElement) const override
    {
        size_t sizeInBits;
        sizeInBits = xmlElement.getAttribute("Size", sizeInBits) ? sizeInBits : 32;

        bool isSigned = false;
        xmlElement.getAttribute("Signed", isSigned);

        auto name = xmlElement.getNameAttribute();

        switch (sizeInBits) {
        case 8:
            if (isSigned) {
                return new CIntegerParameterType<true, 8>(name);
            }
            return new CIntegerParameterType<false, 8>(name);
        case 16:
            if (isSigned) {
                return new CIntegerParameterType<true, 16>(name);
            }
            return new CIntegerParameterType<false, 16>(name);
        case 32:
            if (isSigned) {
                return new CIntegerParameterType<true, 32>(name);
            }
            return new CIntegerParameterType<false, 32>(name);
        default:
            return nullptr;
        }
    }
};
