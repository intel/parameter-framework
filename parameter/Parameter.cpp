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
#include "Parameter.h"
#include "ParameterType.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "ParameterBlackboard.h"

#define base CBaseParameter

using std::string;

CParameter::CParameter(const string& strName, const CTypeElement* pTypeElement) : base(strName, pTypeElement)
{
}

CInstanceConfigurableElement::Type CParameter::getType() const
{
    return EParameter;
}

// XML configuration settings parsing/composing
bool CParameter::serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const
{
    // Check for value space
    handleValueSpaceAttribute(xmlConfigurationSettingsElementContent, configurationAccessContext);

    // Base
    return base::serializeXmlSettings(xmlConfigurationSettingsElementContent, configurationAccessContext);
}

// Value space handling for configuration import
void CParameter::handleValueSpaceAttribute(CXmlElement& xmlConfigurableElementSettingsElement, CConfigurationAccessContext& configurationAccessContext) const
{
    // Delegate to type
    static_cast<const CParameterType*>(getTypeElement())->handleValueSpaceAttribute(xmlConfigurableElementSettingsElement, configurationAccessContext);
}

uint32_t CParameter::getFootPrint() const
{
    return getSize();
}

uint32_t CParameter::getSize() const
{
    return static_cast<const CParameterType*>(getTypeElement())->getSize();
}

// Used for simulation and virtual subsystems
void CParameter::setDefaultValues(CParameterAccessContext& parameterAccessContext) const
{
    // Get default value from type
    uint32_t uiDefaultValue = static_cast<const CParameterType*>(getTypeElement())->getDefaultValue();

    // Write blackboard
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->writeInteger(&uiDefaultValue, getSize(), getOffset() - parameterAccessContext.getBaseOffset(), parameterAccessContext.isBigEndianSubsystem());
}

/// Actual parameter access
// String access
bool CParameter::doSetValue(const string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    return doSet(strValue, uiOffset, parameterAccessContext);
}

void CParameter::doGetValue(string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    doGet(strValue, uiOffset, parameterAccessContext);
}

// Boolean access
bool CParameter::accessAsBoolean(bool& bValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    return doAccess(bValue, bSet, parameterAccessContext);
}

// Integer Access
bool CParameter::accessAsInteger(uint32_t& uiValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    return doAccess(uiValue, bSet, parameterAccessContext);
}

// Signed Integer Access
bool CParameter::accessAsSignedInteger(int32_t& iValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    return doAccess(iValue, bSet, parameterAccessContext);
}

// Double Access
bool CParameter::accessAsDouble(double& dValue, bool bSet, CParameterAccessContext& parameterAccessContext) const
{
    return doAccess(dValue, bSet, parameterAccessContext);
}

// Generic Access
template <typename type>
bool CParameter::doAccess(type& value, bool bSet,
                          CParameterAccessContext& parameterAccessContext) const
{
    if (bSet) {
        // set value
        if (!doSet(value, getOffset() - parameterAccessContext.getBaseOffset(),
                   parameterAccessContext)) {

            appendParameterPathToError(parameterAccessContext);
            return false;

        }
        // Synchronize
        if (!sync(parameterAccessContext)){

            appendParameterPathToError(parameterAccessContext);
            return false;
        }
    } else {
        // get value
        if (!doGet(value, getOffset() - parameterAccessContext.getBaseOffset(),
                   parameterAccessContext)) {

            appendParameterPathToError(parameterAccessContext);
            return false;
        }
    }
    return true;
}

template <typename type>
bool CParameter::doSet(type value, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiData;

    if (!static_cast<const CParameterType*>(getTypeElement())->toBlackboard(value, uiData, parameterAccessContext)) {

        return false;
    }
    // Write blackboard
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->writeInteger(&uiData, getSize(), uiOffset, parameterAccessContext.isBigEndianSubsystem());

    return true;
}

template <typename type>
bool CParameter::doGet(type& value, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiData = 0;

    // Read blackboard
    const CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->readInteger(&uiData, getSize(), uiOffset, parameterAccessContext.isBigEndianSubsystem());

    return static_cast<const CParameterType*>(getTypeElement())->fromBlackboard(value, uiData, parameterAccessContext);
}
