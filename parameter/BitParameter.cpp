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
#include "BitParameter.h"
#include "BitParameterType.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "ParameterBlackboard.h"
#include "BitParameterBlock.h"
#include "BitwiseAreaConfiguration.h"

#define base CBaseParameter

using std::string;

CBitParameter::CBitParameter(const string &strName, const CTypeElement *pTypeElement)
    : base(strName, pTypeElement)
{
}

// Type
CInstanceConfigurableElement::Type CBitParameter::getType() const
{
    return EBitParameter;
}

// Size
size_t CBitParameter::getBelongingBlockSize() const
{
    return static_cast<const CBitParameterBlock *>(getParent())->getSize();
}

// Instantiation, allocation
size_t CBitParameter::getFootPrint() const
{
    // Allocation done at parent level
    return 0;
}

// Actual parameter access (tuning)
bool CBitParameter::doSetValue(const string &strValue, size_t offset,
                               CParameterAccessContext &parameterAccessContext) const
{
    return doSet(strValue, offset, parameterAccessContext);
}

void CBitParameter::doGetValue(string &strValue, size_t offset,
                               CParameterAccessContext &parameterAccessContext) const
{
    doGet(strValue, offset, parameterAccessContext);
}

/// Value access
bool CBitParameter::access(bool &bValue, bool bSet,
                           CParameterAccessContext &parameterAccessContext) const
{
    // Check boolean access validity here
    if (static_cast<const CBitParameterType *>(getTypeElement())->getBitSize() != 1) {

        parameterAccessContext.setError("Type mismatch");
        appendParameterPathToError(parameterAccessContext);

        return false;
    }

    // Rely on integer access
    uint32_t uiValue;

    if (bSet) {

        uiValue = bValue;
    }

    if (!access(uiValue, bSet, parameterAccessContext)) {

        return false;
    }

    if (!bSet) {

        bValue = uiValue != 0;
    }

    return true;
}

bool CBitParameter::access(uint32_t &uiValue, bool bSet,
                           CParameterAccessContext &parameterAccessContext) const
{
    size_t offset = getOffset();

    if (bSet) {

        // Set Value
        if (!doSet(uiValue, offset, parameterAccessContext)) {

            appendParameterPathToError(parameterAccessContext);
            return false;
        }
        // Synchronize
        if (!sync(parameterAccessContext)) {

            appendParameterPathToError(parameterAccessContext);
            return false;
        }
    } else {

        // Convert
        doGet(uiValue, offset, parameterAccessContext);
    }
    return true;
}

template <typename type>
bool CBitParameter::doSet(type value, size_t offset,
                          CParameterAccessContext &parameterAccessContext) const
{
    uint64_t uiData = 0;

    // Read/modify/write
    CParameterBlackboard *pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->readInteger(&uiData, getBelongingBlockSize(), offset);

    // Convert
    if (!static_cast<const CBitParameterType *>(getTypeElement())
             ->toBlackboard(value, uiData, parameterAccessContext)) {

        return false;
    }
    // Write blackboard
    pBlackboard->writeInteger(&uiData, getBelongingBlockSize(), offset);

    return true;
}

template <typename type>
void CBitParameter::doGet(type &value, size_t offset,
                          CParameterAccessContext &parameterAccessContext) const
{
    uint64_t uiData = 0;

    // Read blackboard
    const CParameterBlackboard *pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->readInteger(&uiData, getBelongingBlockSize(), offset);

    // Convert
    static_cast<const CBitParameterType *>(getTypeElement())
        ->fromBlackboard(value, uiData, parameterAccessContext);
}

// AreaConfiguration creation
CAreaConfiguration *CBitParameter::createAreaConfiguration(const CSyncerSet *pSyncerSet) const
{
    return new CBitwiseAreaConfiguration(this, pSyncerSet);
}

// Access from area configuration
uint64_t CBitParameter::merge(uint64_t uiOriginData, uint64_t uiNewData) const
{
    // Convert
    return static_cast<const CBitParameterType *>(getTypeElement())->merge(uiOriginData, uiNewData);
}
