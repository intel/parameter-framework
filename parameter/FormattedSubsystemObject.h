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
#pragma once

#include "SubsystemObject.h"

class CFormattedSubsystemObject : public CSubsystemObject
{
public:
    /**
     * Builds a new CFormattedSubsystemObject instance, without any mapping information.
     *
     * @param[in] pInstanceConfigurableElement Instance of the element linked to the SubsytemObject.
     */
    CFormattedSubsystemObject(CInstanceConfigurableElement* pInstanceConfigurableElement);

    /**
     * Builds a new CFormattedSubsystemObject instance, using a simple mapping value without Amends.
     *
     * @param[in] pInstanceConfigurableElement Instance of the element linked to the SubsytemObject.
     * @param[in] strFormattedMapping A string corresponding to the mapping of the element. The
     * string does not contain any Amend (%) and does not need to be formatted.
     */
    CFormattedSubsystemObject(CInstanceConfigurableElement* pInstanceConfigurableElement,
                              const string& strFormattedMapping);

    /**
     * Builds a new CFormattedSubsystemObject instance, using a mapping value containing Amends.
     *
     * @param[in] pInstanceConfigurableElement Instance of the element linked to the SubsytemObject.
     * @param[in] strMappingValue A string corresponding to the mapping of the element. The
     * string contains Amend (%) and needs to be formatted with information from the context.
     * @param[in] uiFirstAmendKey Index of the first Amend key
     * @param[in] uiNbAmendKeys Number of Amends
     * @param[in] context Contains values associated to Amend keys
     */
    CFormattedSubsystemObject(CInstanceConfigurableElement* pInstanceConfigurableElement,
                              const string& strMappingValue,
                              uint32_t uiFirstAmendKey,
                              uint32_t uiNbAmendKeys,
                              const CMappingContext& context);
    virtual ~CFormattedSubsystemObject();

    /**
     * Returns the formatted mapping value associated to the element.
     *
     * @return A string containing the mapping
     */
    virtual string getFormattedMappingValue() const;

private:

    /**
     * Check if the index of Amend key is valid.
     *
     * @param uiAmendKey Index of the Amend key
     *
     * @return true if the index of the Amend key is > 0 and <= 9.
     */
    static bool isAmendKeyValid(uint32_t uiAmendKey);

    /**
     * Generic mapping formatting
     *
     * Format a string from mapping data and its context, replacing amendments by their value
     *
     * @param[in] strMappingValue The input mapping string containing amendments
     * @param[in] context uiFirstAmendKey The index of the first Amend key in the key list of the
     * context
     * @param[in] uiNbAmendKeys Number of Amend keys in the context
     * @param[in] context The context containing Amend values
     *
     * @return The formatted string, corresponding to the input strMappingValue where %n have been
     * replaced by their value
     */
    static string formatMappingValue(const string& strMappingValue,
                                     uint32_t uiFirstAmendKey,
                                     uint32_t uiNbAmendKeys,
                                     const CMappingContext& context);

    /**
     * string containing the formatted mapping value
     */
    string _strFormattedMappingValue;
};
