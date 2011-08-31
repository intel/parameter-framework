/* <auto_header>
 * <FILENAME>
 * 
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
 *  AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 * 
 * 
 * </auto_header>
 */
#include "BitParameter.h"
#include "BitParameterType.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "ParameterBlackboard.h"
#include "BitParameterBlock.h"

#define base CBaseParameter

CBitParameter::CBitParameter(const string& strName, const CTypeElement* pTypeElement) : base(strName, pTypeElement)
{
}

// Type
CInstanceConfigurableElement::Type CBitParameter::getType() const
{
    return EBitParameter;
}

// Size
uint32_t CBitParameter::getBelongingBlockSize() const
{
    return static_cast<const CBitParameterBlock*>(getParent())->getSize();
}

// Instantiation, allocation
uint32_t CBitParameter::getFootPrint() const
{
    // Allocation made on parent side
    return 0;
}

// Actual parameter access
bool CBitParameter::doSetValue(const string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiData = 0;

    // Read/modify/write
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->read(&uiData, getBelongingBlockSize(), uiOffset, parameterAccessContext.isBigEndianSubsystem());

    // Convert
    if (!static_cast<const CBitParameterType*>(getTypeElement())->asInteger(strValue, uiData, parameterAccessContext)) {

        return false;
    }
    // Write blackboard
    pBlackboard->write(&uiData, getBelongingBlockSize(), uiOffset, parameterAccessContext.isBigEndianSubsystem());

    return true;
}

void CBitParameter::doGetValue(string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiData = 0;

    // Read blackboard
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->read(&uiData, getBelongingBlockSize(), uiOffset, parameterAccessContext.isBigEndianSubsystem());

    // Convert
    static_cast<const CBitParameterType*>(getTypeElement())->asString(uiData, strValue, parameterAccessContext);
}

