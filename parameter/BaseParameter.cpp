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
#include "BaseParameter.h"
#include "ParameterType.h"
#include "ParameterAccessContext.h"
#include "ConfigurationAccessContext.h"
#include "ParameterBlackboard.h"

#define base CInstanceConfigurableElement

CBaseParameter::CBaseParameter(const string& strName, const CTypeElement* pTypeElement) : base(strName, pTypeElement)
{
}

// XML configuration settings parsing/composing
bool CBaseParameter::serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const
{
    // Handle access
    if (!configurationAccessContext.serializeOut()) {

        // Write to blackboard
        if (!doSetValue(xmlConfigurationSettingsElementContent.getTextContent(), getOffset() - configurationAccessContext.getBaseOffset(), configurationAccessContext)) {

            // Append parameter path to error
            configurationAccessContext.appendToError(" " + getPath());

            return false;
        }
    } else {

        // Get string value
        string strValue;

        doGetValue(strValue, getOffset() - configurationAccessContext.getBaseOffset(), configurationAccessContext);

        // Populate value into xml text node
        xmlConfigurationSettingsElementContent.setTextContent(strValue);
    }

    // Done
    return true;
}

// Dump
void CBaseParameter::logValue(string& strValue, CErrorContext& errorContext) const
{
    // Parameter context
    CParameterAccessContext& parameterContext = static_cast<CParameterAccessContext&>(errorContext);

    // Dump value
    doGetValue(strValue, getOffset(), parameterContext);
}

// Parameter Access
bool CBaseParameter::setValue(CPathNavigator& pathNavigator, const string& strValue, CParameterAccessContext& parameterContext) const
{
    // Check path validity
    if (!checkPathExhausted(pathNavigator, parameterContext)) {

        return false;
    }

    // Check for dynamic access
    if (!checkForDynamicAccess(parameterContext)) {

        return false;
    }

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

bool CBaseParameter::getValue(CPathNavigator& pathNavigator, string& strValue, CParameterAccessContext& parameterContext) const
{
    // Check path validity
    if (!checkPathExhausted(pathNavigator, parameterContext)) {

        return false;
    }

    // Check for dynamic access
    if (!checkForDynamicAccess(parameterContext)) {

        return false;
    }

    // Get Value
    doGetValue(strValue, getOffset(), parameterContext);

    return true;
}

// Dynamic access checking
bool CBaseParameter::checkForDynamicAccess(CParameterAccessContext& parameterAccessContext) const
{
    // Check for dynamic access
    if (parameterAccessContext.isDynamicAccess() && !isRogue()) {

        // Parameter is not rogue
        parameterAccessContext.setError("Parameter " + getPath() + " is not rogue");

        return false;
    }

    return true;
}
