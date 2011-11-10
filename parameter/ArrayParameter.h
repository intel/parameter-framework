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
#pragma once

#include "Parameter.h"

class CArrayParameter : public CParameter
{
public:
    CArrayParameter(const string& strName, const CTypeElement* pTypeElement);

    // Instantiation, allocation
    virtual uint32_t getFootPrint() const;

    // XML configuration settings parsing
    virtual bool serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const;
protected:
    // User set/get
    virtual bool setValue(CPathNavigator& pathNavigator, const string& strValue, CParameterAccessContext& parameterContext) const;
    virtual bool getValue(CPathNavigator& pathNavigator, string& strValue, CParameterAccessContext& parameterContext) const;
    virtual void logValue(string& strValue, CErrorContext& errorContext) const;
    // Used for simulation and virtual subsystems
    virtual void setDefaultValues(CParameterAccessContext& parameterAccessContext) const;

    // Element properties
    virtual void showProperties(string& strResult) const;
private:
    // Array length
    uint32_t getArrayLength() const;
    // Common set value processing
    bool setValues(uint32_t uiStartIndex, uint32_t uiBaseOffset, const string& strValue, CParameterAccessContext& parameterContext) const;
    // Log / get values common
    void getValues(uint32_t uiBaseOffset, string& strValues, CParameterAccessContext& parameterContext) const;
    // Index retrieval from user set/get request
    bool getIndex(CPathNavigator& pathNavigator, uint32_t& uiIndex, CParameterAccessContext& parameterContext) const;
};
