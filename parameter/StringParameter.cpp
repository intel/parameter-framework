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
#include "StringParameter.h"
#include "StringParameterType.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "ParameterBlackboard.h"
#include <alloca.h>

#define base CBaseParameter

CStringParameter::CStringParameter(const string& strName, const CTypeElement* pTypeElement) : base(strName, pTypeElement)
{
}

CInstanceConfigurableElement::Type CStringParameter::getType() const
{
    return EStringParameter;
}

uint32_t CStringParameter::getFootPrint() const
{
    return getSize();
}

uint32_t CStringParameter::getSize() const
{
    return static_cast<const CStringParameterType*>(getTypeElement())->getMaxLength() + 1;
}

// Used for simulation and virtual subsystems
void CStringParameter::setDefaultValues(CParameterAccessContext& parameterAccessContext) const
{
    // Write blackboard
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    pBlackboard->writeString("", getOffset());
}

// Actual parameter access
bool CStringParameter::doSetValue(const string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    if (strValue.length() >= getSize()) {

        parameterAccessContext.setError("Maximum length exceeded");

        return false;
    }

    // Write blackboard
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    pBlackboard->writeString(strValue.c_str(), uiOffset);

    return true;
}

void CStringParameter::doGetValue(string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    char* pcValue = (char*)alloca(getSize());

    // Read blackboard
    const CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    pBlackboard->readString(pcValue, uiOffset);

    strValue = pcValue;
}

