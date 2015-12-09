/*
 * Copyright (c) 2011-2015, Intel Corporation
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

#include "FormattedSubsystemObject.h"
#include "InstanceConfigurableElement.h"
#include "MappingContext.h"
#include <assert.h>

#define base CSubsystemObject

using std::string;

CFormattedSubsystemObject::CFormattedSubsystemObject(
    CInstanceConfigurableElement *pInstanceConfigurableElement, core::log::Logger &logger)
    : base(pInstanceConfigurableElement, logger)
{
}

CFormattedSubsystemObject::CFormattedSubsystemObject(
    CInstanceConfigurableElement *pInstanceConfigurableElement, core::log::Logger &logger,
    const string &strMappingValue)
    : base(pInstanceConfigurableElement, logger), _strFormattedMappingValue(strMappingValue)
{
}

CFormattedSubsystemObject::CFormattedSubsystemObject(
    CInstanceConfigurableElement *pInstanceConfigurableElement, core::log::Logger &logger,
    const string &strMappingValue, size_t firstAmendKey, size_t nbAmendKeys,
    const CMappingContext &context)
    : base(pInstanceConfigurableElement, logger), _strFormattedMappingValue(strMappingValue)
{
    // Cope with quotes in the name
    if (strMappingValue[0] == '\'' && strMappingValue.length() >= 2) {

        _strFormattedMappingValue = strMappingValue.substr(1, strMappingValue.length() - 2);
    }
    _strFormattedMappingValue =
        formatMappingValue(_strFormattedMappingValue, firstAmendKey, nbAmendKeys, context);
}

string CFormattedSubsystemObject::getFormattedMappingValue() const
{
    return _strFormattedMappingValue;
}

bool CFormattedSubsystemObject::isAmendKeyValid(size_t uiAmendKey)
{

    return (uiAmendKey > 0) && (uiAmendKey <= 9);
}

string CFormattedSubsystemObject::formatMappingValue(const string &strMappingValue,
                                                     size_t firstAmendKey, size_t nbAmendKeys,
                                                     const CMappingContext &context)
{
    string strFormattedValue = strMappingValue;

    // Search for amendment (only one supported for now)
    string::size_type uiPercentPos = strFormattedValue.find('%', 0);

    // Amendment limited to one digit (values from 1 to 9)
    assert(isAmendKeyValid(nbAmendKeys));

    // Check we found one and that there's room for value
    if (uiPercentPos != string::npos && uiPercentPos < strFormattedValue.size() - 1) {

        // Get Amend number
        size_t uiAmendNumber = strFormattedValue[uiPercentPos + 1] - '0';

        // Check if current Amend number is Valid
        if ((uiAmendNumber > 0) && (uiAmendNumber <= nbAmendKeys)) {

            size_t uiAmendType = firstAmendKey + uiAmendNumber - 1;

            // Check if current Amend type is Set in the context
            if (context.iSet(uiAmendType)) {

                // Make the amendment on the part of the string after the current Amend
                string strEndOfLine = strFormattedValue.substr(
                    uiPercentPos + 2, strFormattedValue.size() - uiPercentPos - 2);
                string strEndOfLineAmended =
                    formatMappingValue(strEndOfLine, firstAmendKey, nbAmendKeys, context);

                // Get current Amend value
                string strAmendValue = context.getItem(uiAmendType);

                // Make the amendment
                strFormattedValue =
                    strFormattedValue.substr(0, uiPercentPos) + strAmendValue + strEndOfLineAmended;
            }
        }
    }
    return strFormattedValue;
}
