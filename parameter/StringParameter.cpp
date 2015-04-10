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
#include "StringParameter.h"
#include "StringParameterType.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "ParameterBlackboard.h"

#define base CBaseParameter

using std::string;

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

// Actual parameter access (tuning)
bool CStringParameter::doSetValue(const string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    if (strValue.length() >= getSize()) {

        parameterAccessContext.setError("Maximum length exceeded");

        return false;
    }

    // Write blackboard
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    pBlackboard->writeString(strValue, uiOffset);

    return true;
}

void CStringParameter::doGetValue(string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    parameterAccessContext.getParameterBlackboard()->readString(strValue, uiOffset);
}
