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
#pragma once

#include "parameter_export.h"

#include "SubsystemObject.h"

class PARAMETER_EXPORT CFormattedSubsystemObject : public CSubsystemObject
{
public:
    /**
     * Builds a new CFormattedSubsystemObject instance, without any mapping information.
     *
     * @param[in] pInstanceConfigurableElement Instance of the element linked to the SubsytemObject.
     * @param[in] logger the logger provided by the client
     */
    CFormattedSubsystemObject(CInstanceConfigurableElement *pInstanceConfigurableElement,
                              core::log::Logger &logger);

    /**
     * Builds a new CFormattedSubsystemObject instance, using a simple mapping value without Amends.
     *
     * @param[in] pInstanceConfigurableElement Instance of the element linked to the SubsytemObject.
     * @param[in] logger the logger provided by the client
     * @param[in] strFormattedMapping A std::string corresponding to the mapping of the element. The
     * std::string does not contain any Amend (%) and does not need to be formatted.
     */
    CFormattedSubsystemObject(CInstanceConfigurableElement *pInstanceConfigurableElement,
                              core::log::Logger &logger, const std::string &strFormattedMapping);

    /**
     * Builds a new CFormattedSubsystemObject instance, using a mapping value containing Amends.
     *
     * @param[in] pInstanceConfigurableElement Instance of the element linked to the SubsytemObject.
     * @param[in] logger the logger provided by the client
     * @param[in] strMappingValue A std::string corresponding to the mapping of the element. The
     * std::string contains Amend (%) and needs to be formatted with information from the context.
     * @param[in] firstAmendKey Index of the first Amend key
     * @param[in] nbAmendKeys Number of Amends
     * @param[in] context Contains values associated to Amend keys
     */
    CFormattedSubsystemObject(CInstanceConfigurableElement *pInstanceConfigurableElement,
                              core::log::Logger &logger, const std::string &strMappingValue,
                              size_t firstAmendKey, size_t nbAmendKeys,
                              const CMappingContext &context);
    ~CFormattedSubsystemObject() override = default;

    /**
     * Returns the formatted mapping value associated to the element.
     *
     * @return A std::string containing the mapping
     */
    std::string getFormattedMappingValue() const override;

private:
    /**
     * Check if the index of Amend key is valid.
     *
     * @param uiAmendKey Index of the Amend key
     *
     * @return true if the index of the Amend key is > 0 and <= 9.
     */
    static bool isAmendKeyValid(size_t uiAmendKey);

    /**
     * Generic mapping formatting
     *
     * Format a std::string from mapping data and its context, replacing amendments by their value
     *
     * @param[in] strMappingValue The input mapping std::string containing amendments
     * @param[in] context firstAmendKey The index of the first Amend key in the key list of the
     * context
     * @param[in] nbAmendKeys Number of Amend keys in the context
     * @param[in] context The context containing Amend values
     *
     * @return The formatted std::string, corresponding to the input strMappingValue where %n have
     * been
     * replaced by their value
     */
    static std::string formatMappingValue(const std::string &strMappingValue, size_t firstAmendKey,
                                          size_t nbAmendKeys, const CMappingContext &context);

    /**
     * std::string containing the formatted mapping value
     */
    std::string _strFormattedMappingValue;
};
