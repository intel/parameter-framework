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
#include "BitwiseAreaConfiguration.h"

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
    // Allocation done at parent level
    return 0;
}

// Actual parameter access (tuning)
bool CBitParameter::doSetValue(const string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    return doSet(strValue, uiOffset, parameterAccessContext);
}

void CBitParameter::doGetValue(string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    doGet(strValue, uiOffset, parameterAccessContext);
}

/// Value access
// Boolean access
bool CBitParameter::accessAsBoolean(bool& bValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    // Check boolean access validity here
    if (static_cast<const CBitParameterType*>(getTypeElement())->getBitSize() != 1) {

        parameterAccessContext.setError("Type mismatch");
        // Append parameter path to error
        parameterAccessContext.appendToError(" " + getPath());

        return false;
    }

    // Rely on integer access
    uint32_t uiValue;

    if (bSet) {

        uiValue = bValue;
    }

    if (!accessAsInteger(uiValue, bSet, parameterAccessContext)) {

        return false;
    }

    if (!bSet) {

        bValue = uiValue != 0;
    }

    return true;
}

// Integer Access
bool CBitParameter::accessAsInteger(uint32_t& uiValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiOffset = getOffset();

    if (bSet) {

        // Set and sync
        if (!doSet(uiValue, uiOffset, parameterAccessContext) || !sync(parameterAccessContext)) {

            // Append parameter path to error
            parameterAccessContext.appendToError(" " + getPath());

            return false;
        }
    } else {

        // Convert
        doGet(uiValue, uiOffset, parameterAccessContext);
    }
    return true;
}

template <typename type>
bool CBitParameter::doSet(type value, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiData = 0;

    // Read/modify/write
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->readInteger(&uiData, getBelongingBlockSize(), uiOffset, parameterAccessContext.isBigEndianSubsystem());

    // Convert
    if (!static_cast<const CBitParameterType*>(getTypeElement())->toBlackboard(value, uiData, parameterAccessContext)) {

        return false;
    }
    // Write blackboard
    pBlackboard->writeInteger(&uiData, getBelongingBlockSize(), uiOffset, parameterAccessContext.isBigEndianSubsystem());

    return true;
}

template <typename type>
void CBitParameter::doGet(type& value, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiData = 0;

    // Read blackboard
    const CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->readInteger(&uiData, getBelongingBlockSize(), uiOffset, parameterAccessContext.isBigEndianSubsystem());

    // Convert
    static_cast<const CBitParameterType*>(getTypeElement())->fromBlackboard(value, uiData, parameterAccessContext);
}

// AreaConfiguration creation
CAreaConfiguration* CBitParameter::createAreaConfiguration(const CSyncerSet* pSyncerSet) const
{
    return new CBitwiseAreaConfiguration(this, pSyncerSet);
}

// Access from area configuration
uint32_t CBitParameter::merge(uint32_t uiOriginData, uint32_t uiNewData) const
{
    // Convert
    return static_cast<const CBitParameterType*>(getTypeElement())->merge(uiOriginData, uiNewData);
}
