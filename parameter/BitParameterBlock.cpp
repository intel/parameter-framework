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
#include "BitParameterBlock.h"
#include "BitParameterBlockType.h"
#include "ParameterAccessContext.h"
#include "ParameterBlackboard.h"

#define base CInstanceConfigurableElement

using std::string;

CBitParameterBlock::CBitParameterBlock(const string& strName, const CTypeElement* pTypeElement) : base(strName, pTypeElement)
{
}

CInstanceConfigurableElement::Type CBitParameterBlock::getType() const
{
    return EBitParameterBlock;
}

// Instantiation, allocation
uint32_t CBitParameterBlock::getFootPrint() const
{
    return getSize();
}

// Size
uint32_t CBitParameterBlock::getSize() const
{
    return static_cast<const CBitParameterBlockType*>(getTypeElement())->getSize();
}

// Used for simulation and virtual subsystems
void CBitParameterBlock::setDefaultValues(CParameterAccessContext& parameterAccessContext) const
{
    // Get default value from type
    uint32_t uiDefaultValue = 0;

    // Write blackboard
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->writeInteger(&uiDefaultValue, getSize(), getOffset() - parameterAccessContext.getBaseOffset(), parameterAccessContext.isBigEndianSubsystem());
}

