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
#include "Parameter.h"
#include "ParameterType.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "ParameterBlackboard.h"

#define base CInstanceConfigurableElement

CParameter::CParameter(const string& strName, const CTypeElement* pTypeElement) : base(strName, pTypeElement)
{
}

CParameter::~CParameter()
{
}

CInstanceConfigurableElement::Type CParameter::getType() const
{
    return EParameter;
}

// XML configuration settings parsing/composing
bool CParameter::serializeXmlSettings(CXmlElement& xmlConfigurableElementSettingsElement, CConfigurationAccessContext& configurationAccessContext) const
{
    // Check for value space
    handleValueSpaceAttribute(xmlConfigurableElementSettingsElement, configurationAccessContext);

    // Handle access
    if (!configurationAccessContext.serializeOut()) {

        // Write to blackboard
        if (!doSetValue(xmlConfigurableElementSettingsElement.getTextContent(), getOffset() - configurationAccessContext.getBaseOffset(), configurationAccessContext)) {

            // Append parameter path to error
            configurationAccessContext.appendToError(" " + getPath());

            return false;
        }
    } else {

        // Get string value
        string strValue;

        doGetValue(strValue, getOffset() - configurationAccessContext.getBaseOffset(), configurationAccessContext);

        // Populate value into xml text node
        xmlConfigurableElementSettingsElement.setTextContent(strValue);
    }

    // Done
    return true;
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

// Dump
void CParameter::logValue(string& strValue, CErrorContext& errorContext) const
{
    CParameterAccessContext& parameterContext = static_cast<CParameterAccessContext&>(errorContext);

    // Dump value
    doGetValue(strValue, getOffset(), parameterContext);

    // Prepend unit if any
    prependUnit(strValue);
}

// Used for simulation only
void CParameter::setDefaultValues(CParameterAccessContext& parameterAccessContext) const
{
    // Get default value from type
    uint32_t uiDefaultValue = static_cast<const CParameterType*>(getTypeElement())->getDefaultValue();

    // Write blackboard
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->write(&uiDefaultValue, getSize(), getOffset(), parameterAccessContext.isBigEndianSubsystem());
}

// Unit
void CParameter::prependUnit(string& strValue) const
{
    string strUnit = static_cast<const CParameterType*>(getTypeElement())->getUnit();

    if (!strUnit.empty()) {

        strValue = "(" + strUnit + ") " + strValue;
    }
}

// Parameter Access
bool CParameter::setValue(CPathNavigator& pathNavigator, const string& strValue, CErrorContext& errorContext) const
{
    // Check path validity
    if (!checkPathExhausted(pathNavigator, errorContext)) {

        return false;
    }
    // Parameter context
    CParameterAccessContext& parameterContext = static_cast<CParameterAccessContext&>(errorContext);

    // Set Value
    if (!doSetValue(strValue, getOffset(), parameterContext)) {

        // Append parameter path to error
        parameterContext.appendToError(" " + getPath());

        return false;
    }
    // Synchronize
    if (parameterContext.getAutoSync() && !sync(parameterContext)) {

        // Append parameter path to error
        parameterContext.appendToError(" " + getPath());

        return false;
    }
    return true;
}

bool CParameter::getValue(CPathNavigator& pathNavigator, string& strValue, CErrorContext& errorContext) const
{
    // Check path validity
    if (!checkPathExhausted(pathNavigator, errorContext)) {

        return false;
    }
    // Parameter context
    CParameterAccessContext& parameterContext = static_cast<CParameterAccessContext&>(errorContext);

    // Get Value
    doGetValue(strValue, getOffset(), parameterContext);

    return true;
}

bool CParameter::doSetValue(const string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiData;

    if (!static_cast<const CParameterType*>(getTypeElement())->asInteger(strValue, uiData, parameterAccessContext)) {

        return false;
    }

    // Write blackboard
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->write(&uiData, getSize(), uiOffset, parameterAccessContext.isBigEndianSubsystem());

    return true;
}

void CParameter::doGetValue(string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const
{
    uint32_t uiData = 0;

    // Read blackboard
    CParameterBlackboard* pBlackboard = parameterAccessContext.getParameterBlackboard();

    // Beware this code works on little endian architectures only!
    pBlackboard->read(&uiData, getSize(), uiOffset, parameterAccessContext.isBigEndianSubsystem());

    static_cast<const CParameterType*>(getTypeElement())->asString(uiData, strValue, parameterAccessContext);
}

