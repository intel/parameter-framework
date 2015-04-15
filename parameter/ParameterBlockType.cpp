/*
 * Copyright (c) 2011-2014, Intel Corporation
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
#include "ParameterBlockType.h"
#include "ParameterBlock.h"
#include "Utility.h"

#define base CTypeElement

CParameterBlockType::CParameterBlockType(const std::string& strName) : base(strName)
{
}

std::string CParameterBlockType::getKind() const
{
    return "ParameterBlock";
}

bool CParameterBlockType::childrenAreDynamic() const
{
    return true;
}

CInstanceConfigurableElement* CParameterBlockType::doInstantiate() const
{
    return new CParameterBlock(getName(), this);
}

void CParameterBlockType::populate(CElement* pElement) const
{
    uint32_t uiArrayLength = getArrayLength();

    if (uiArrayLength) {

        // Create child elements
        size_t uiChild;

        for (uiChild = 0; uiChild < uiArrayLength; uiChild++) {

            CParameterBlock* pChildParameterBlock = new CParameterBlock(CUtility::toString(uiChild),
                                                                        this);

            pElement->addChild(pChildParameterBlock);

            base::populate(pChildParameterBlock);
        }
    } else {
        // Regular block
        base::populate(pElement);
    }
}
