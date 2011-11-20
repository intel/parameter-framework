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

#include <stdint.h>

#include "BaseParameter.h"

class CParameter : public CBaseParameter
{
public:
    CParameter(const string& strName, const CTypeElement* pTypeElement);

    // Instantiation, allocation
    virtual uint32_t getFootPrint() const;

    // Type
    virtual Type getType() const;

    // XML configuration settings parsing/composing
    virtual bool serializeXmlSettings(CXmlElement& xmlConfigurationSettingsElementContent, CConfigurationAccessContext& configurationAccessContext) const;

    // Boolean access
    virtual bool accessAsBoolean(bool& bValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // Integer Access
    virtual bool accessAsInteger(uint32_t& uiValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // Signed Integer Access
    virtual bool accessAsSignedInteger(int32_t& iValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;

    // Double Access
    virtual bool accessAsDouble(double& dValue, bool bSet, CParameterAccessContext& parameterAccessContext) const;
protected:
    // Used for simulation and virtual subsystems
    virtual void setDefaultValues(CParameterAccessContext& parameterAccessContext) const;

    // Actual value access
    virtual bool doSetValue(const string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const;
    virtual void doGetValue(string& strValue, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const;

    // Value space handling for configuration import
    void handleValueSpaceAttribute(CXmlElement& xmlConfigurableElementSettingsElement, CConfigurationAccessContext& configurationAccessContext) const;

    // Size
    uint32_t getSize() const;
private:
    // Generic Access
    template <typename type>
    bool doAccess(type& value, bool bSet, CParameterAccessContext& parameterAccessContext) const;
    template <typename type>
    bool doSet(type value, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const;
    template <typename type>
    bool doGet(type& value, uint32_t uiOffset, CParameterAccessContext& parameterAccessContext) const;
};
