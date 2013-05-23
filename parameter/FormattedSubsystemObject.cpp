/*
 * INTEL CONFIDENTIAL
 * Copyright © 2013 Intel
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
 */

#include "FormattedSubsystemObject.h"
#include "InstanceConfigurableElement.h"
#include "MappingContext.h"
#include <assert.h>

#define base CSubsystemObject

CFormattedSubsystemObject::CFormattedSubsystemObject(
        CInstanceConfigurableElement* pInstanceConfigurableElement)
    : base(pInstanceConfigurableElement)
{
}

CFormattedSubsystemObject::CFormattedSubsystemObject(
        CInstanceConfigurableElement* pInstanceConfigurableElement,
        const string& strMappingValue)
    : base(pInstanceConfigurableElement), _strFormattedMappingValue(strMappingValue)
{

}


CFormattedSubsystemObject::CFormattedSubsystemObject(
        CInstanceConfigurableElement* pInstanceConfigurableElement,
        const string& strMappingValue,
        uint32_t uiFirstAmendKey,
        uint32_t uiNbAmendKeys,
        const CMappingContext& context)
    : base(pInstanceConfigurableElement), _strFormattedMappingValue(strMappingValue)
{
    // Cope with quotes in the name
    if (strMappingValue[0] == '\'' && strMappingValue.length() >= 2) {

        _strFormattedMappingValue = strMappingValue.substr(1, strMappingValue.length() - 2);
    }
    _strFormattedMappingValue = formatMappingValue(_strFormattedMappingValue, uiFirstAmendKey,
                                                   uiNbAmendKeys, context);
}

CFormattedSubsystemObject::~CFormattedSubsystemObject()
{
}

string CFormattedSubsystemObject::getFormattedMappingValue() const
{
    return _strFormattedMappingValue;
}

bool CFormattedSubsystemObject::isAmendKeyValid(uint32_t uiAmendKey)
{

    return (uiAmendKey > 0) && (uiAmendKey <= 9);
}

string CFormattedSubsystemObject::formatMappingValue(const string& strMappingValue,
                                                     uint32_t uiFirstAmendKey,
                                                     uint32_t uiNbAmendKeys,
                                                     const CMappingContext& context)
{
    string strFormattedValue = strMappingValue;

    // Search for amendment (only one supported for now)
    size_t uiPercentPos = strFormattedValue.find('%', 0);

    // Amendment limited to one digit (values from 1 to 9)
    assert(isAmendKeyValid(uiNbAmendKeys));

    // Check we found one and that there's room for value
    if (uiPercentPos != string::npos && uiPercentPos < strFormattedValue.size() - 1) {

        // Get Amend number
        uint32_t uiAmendNumber = strFormattedValue[uiPercentPos + 1] - '0';

        // Check if current Amend number is Valid
        if ((uiAmendNumber > 0) && (uiAmendNumber <= uiNbAmendKeys)) {

            uint32_t uiAmendType = uiFirstAmendKey + uiAmendNumber - 1;

            // Check if current Amend type is Set in the context
            if (context.iSet(uiAmendType)) {

                // Make the amendment on the part of the string after the current Amend
                string strEndOfLine = strFormattedValue.substr(uiPercentPos + 2,
                                                               strFormattedValue.size()
                                                               - uiPercentPos - 2);
                string strEndOfLineAmended = formatMappingValue(strEndOfLine, uiFirstAmendKey,
                                                                uiNbAmendKeys, context);

                // Get current Amend value
                string strAmendValue = context.getItem(uiAmendType);

                // Make the amendment
                strFormattedValue = strFormattedValue.substr(0, uiPercentPos) + strAmendValue
                        + strEndOfLineAmended;

            }
        }
    }
    return strFormattedValue;
}
