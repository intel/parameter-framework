/* 
 * INTEL CONFIDENTIAL
 * Copyright © 2011 Intel 
 * Corporation All Rights Reserved.
 * 
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel’s prior express written permission.
 * 
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 * 
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 */
#include "BitParameterBlock.h"
#include "BitParameterBlockType.h"
#include "ParameterAccessContext.h"
#include "ParameterBlackboard.h"

#define base CInstanceConfigurableElement

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

